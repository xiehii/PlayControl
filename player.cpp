#include "player.h"
#include <QWidget>

// 避免 ssize_t 引发的错误
#if defined(_MSC_VER)
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <vlc/vlc.h>

static void *lock(void * opaque, void ** planes)
{
	Context *ctx = static_cast<Context *>(opaque);
	ctx->mutex.lock();

	//告诉VLC将解码的数据放到缓冲区中
	*planes = ctx->pixels;
	return nullptr;
}
//获取argb 图片并保存到文件中
static void unlock(void * opaque, void * picture, void * const * planes)
{
	Q_UNUSED(picture);

	Context *ctx = static_cast<Context *>(opaque);
	unsigned char *data = static_cast<unsigned char *>(*planes);
	static int frameCount = 1;

	QImage image(data, VIDEO_WIDTH, VIDEO_HEIGHT, QImage::Format_ARGB32);
	image.save(QString(u8"./提取的视频帧/frame_%1.png").arg(frameCount++));

	ctx->mutex.unlock();
}

static void display(void * opaque, void * picture)
{
	Q_UNUSED(picture);

	(void)opaque;
}

// 回调函数，用于事件处理
static void handleEvents(const libvlc_event_t *event, void *userData)
{
    Player *player = static_cast<Player *>(userData);
    switch (event->type) {
    // 播放状态改变
    case libvlc_MediaPlayerOpening:
    case libvlc_MediaPlayerBuffering:
        break;
    case libvlc_MediaPlayerPlaying: {
        emit player->stateChanged(Player::Playing);
        break;
    }
    case libvlc_MediaPlayerPaused: {
        emit player->stateChanged(Player::Paused);
        break;
    }
    case libvlc_MediaPlayerStopped: {
        emit player->stateChanged(Player::Stopped);
        break;
    }
    case libvlc_MediaPlayerEncounteredError: {
        emit player->stateChanged(Player::Error);
        break;
    }
    // 时长改变
    case libvlc_MediaPlayerLengthChanged: {
        qint64 dur = event->u.media_player_length_changed.new_length;
        emit player->durationChanged(dur);
        break;
    }
    // 播放时间改变
    case libvlc_MediaPlayerTimeChanged: {
        qint64 time = event->u.media_player_time_changed.new_time;
        emit player->timeChanged(time);
        break;
    }
    // 播放位置改变
    case libvlc_MediaPlayerPositionChanged: {
        float pos = event->u.media_player_position_changed.new_position;
        emit player->positionChanged(pos);
        break;
    }
    // 媒体信息改变
    case libvlc_MediaParsedChanged: {
        int state = event->u.media_parsed_changed.new_status;
        if (int(libvlc_media_parsed_status_done) == state)
            player->getMeta(player->getMedia());
        break;
    }
    default:
        break;
    }
}


Player::Player(QObject *parent)
    : QObject(parent)
{
    m_instance = libvlc_new(0, nullptr);
    m_player = libvlc_media_player_new(m_instance);
    m_eventManager = libvlc_media_player_event_manager(m_player);

    attachEvents();
}

Player::~Player()
{
    libvlc_media_player_stop(m_player);
    libvlc_media_player_release(m_player);
    libvlc_release(m_instance);
}

// 读取元数据
void Player::getMeta(libvlc_media_t *media)
{
    // 读取标题、艺术家、专辑
    char *title = libvlc_media_get_meta(media, libvlc_meta_Title);
    char *artist = libvlc_media_get_meta(media, libvlc_meta_Artist);
    char *album = libvlc_media_get_meta(media, libvlc_meta_Album);

    if (nullptr != title)
        qDebug() << "title: " << title << endl;

    if (nullptr != artist)
        qDebug() << "artist: " << artist << endl;

    if (nullptr != album)
        qDebug() << "album: " << album << endl;
}

libvlc_media_t *Player::getMedia()
{
    return m_media;
}

void Player::attachEvents()
{
    // 事件列表
    QList<libvlc_event_e> events;
    events << libvlc_MediaPlayerOpening
           << libvlc_MediaPlayerBuffering
           << libvlc_MediaPlayerPlaying
           << libvlc_MediaPlayerPaused
           << libvlc_MediaPlayerStopped
           << libvlc_MediaPlayerEncounteredError
           << libvlc_MediaPlayerMuted
           << libvlc_MediaPlayerUnmuted
           << libvlc_MediaPlayerAudioVolume
           << libvlc_MediaPlayerLengthChanged
           << libvlc_MediaPlayerTimeChanged
           << libvlc_MediaPlayerPositionChanged
           << libvlc_MediaParsedChanged;

    // 订阅事件
    foreach (const libvlc_event_e &e, events) {
        libvlc_event_attach(m_eventManager, e, handleEvents, this);
    }
}

void Player::setVideoWindow(QWidget *window)
{
    m_videoWindow = window;
}

Player::State Player::state()
{
    libvlc_state_t state = libvlc_media_player_get_state(m_player);
    return Player::State(state);
}

void Player::firstVideo()
{
	const char * localMrl = "F:\\Videos\\Sample.mkv";
	const char * rtspMrl = "http://ivi.bupt.edu.cn/hls/cctv6hd.m3u8";
	m_url = QString(rtspMrl);
	// 创建并初始化 libvlc 实例
	m_instance = libvlc_new(0, nullptr);

	// 创建一个媒体
//    media = libvlc_media_new_path(instance, localMrl);
	m_media = libvlc_media_new_location(m_instance, rtspMrl);
	libvlc_media_player_set_media(m_player, m_media);
    libvlc_media_parse_with_options(m_media, libvlc_media_parse_network, 1000);
    play();
}

void Player::openURL(const QString &url)
{
	m_url = url;
    libvlc_media_player_pause(m_player);

    m_media = libvlc_media_new_location(m_instance, url.toUtf8().data());
    libvlc_media_player_set_media(m_player, m_media);

    libvlc_media_add_option(m_media, ":avcodec-hw=none");
    libvlc_media_parse_with_options(m_media, libvlc_media_parse_network, 1000);
    play();
}

void Player::openFile(const QString &file)
{
    libvlc_media_player_pause(m_player);

    m_media = libvlc_media_new_path(m_instance, file.toUtf8().data());
    libvlc_media_player_set_media(m_player, m_media);

    libvlc_media_add_option(m_media, ":avcodec-hw=none");
    libvlc_media_parse_with_options(m_media, libvlc_media_parse_local, 1000);
    play();
}

void Player::play()
{
    WId curWId = 0;
    if (nullptr != m_videoWindow)
        curWId = m_videoWindow->winId();

    // 指定输出窗口
#if defined (Q_OS_WIN)
    libvlc_media_player_set_hwnd(m_player, (void*)curWId);
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(m_player, (void *)curWId);
#else
    libvlc_media_player_set_xwindow(m_player, curWId);
#endif

    // 恢复或者播放
    if (state() == Player::Paused) {
        libvlc_media_player_set_pause(m_player, false);
    } else {
        libvlc_media_player_play(m_player);
    }
}

void Player::pause()
{
    if (libvlc_media_player_can_pause(m_player))
        libvlc_media_player_set_pause(m_player, true);
}

void Player::stop()
{
    libvlc_media_player_stop(m_player);
}

void Player::screenShot()
{
	QDir file;
	file.mkdir(u8"./截图");
	QString name = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    int value = libvlc_video_take_snapshot(m_player,screenShotNum,QString(u8"./截图/%1.png").arg(name).toStdString().c_str(),
                                           static_cast<unsigned int>(libvlc_video_get_width(m_player)),
                                           static_cast<unsigned int>(libvlc_video_get_height(m_player)));
	if (value != -1)
	{
		qDebug() << "success";
		screenShotNum++;
	}      
    else {
        qDebug()<<"this video was not found";
    }
}

QString Player::GetCurrentAspectRadio()
{
    return libvlc_video_get_aspect_ratio(m_player);
}

void Player::SetCurrentAspectRadio(QString str)
{
    if(str.indexOf("Default") != -1)
        libvlc_video_set_aspect_ratio(m_player,"1:1");
    else
        libvlc_video_set_aspect_ratio(m_player,str.toLatin1());
}

void Player::GetVideoTracksList()
{
	qDebug()<<"track count:"<< libvlc_video_get_track_count(m_player);
	libvlc_track_description_t *temp = libvlc_video_get_track_description(m_player);
    while (temp != nullptr)
	{
        qDebug()<<"track description :"<< QString(temp->psz_name);
        temp = temp->p_next;
	}
    qDebug()<<"current video track:"<<libvlc_video_get_track(m_player);
}

void Player::update()
{
	if (!m_url.isEmpty())
	{
		qDebug() << "刷新";
		libvlc_media_player_pause(m_player);

		m_instance = libvlc_new(0, nullptr);

		// 创建一个媒体
	//    media = libvlc_media_new_path(instance, localMrl);
		m_media = libvlc_media_new_location(m_instance, m_url.toStdString().c_str());
		libvlc_media_player_set_media(m_player, m_media);
		libvlc_media_parse_with_options(m_media, libvlc_media_parse_network, 1000);
		play();
	}
}

void Player::extractFrames(QString str)
{
	//libvlc_media_new_path文件路径不支持"/" ，支持"\\"
	str = str.replace("/", "\\");
	//const char * localMrl = "D:\\playData\\VlC-QT-SDK\\libVLC-examples\\02da0c4aa0346e92d9feb553f98b4ac83369c3f81c125d5b46470cbe95da3455808732a6bca26362373bfb4af9932096.mp4";

	Context ctx;
	ctx.pixels = new uchar[VIDEO_WIDTH * VIDEO_HEIGHT * 4];
	memset(ctx.pixels, 0, VIDEO_WIDTH*VIDEO_HEIGHT * 4);

	libvlc_instance_t *instance;
	libvlc_media_player_t *player;
	libvlc_media_t *media;

	instance = libvlc_new(0, nullptr);
	media = libvlc_media_new_path(instance, str.toStdString().c_str());
	player = libvlc_media_player_new_from_media(media);

	//设置回调，用于提取帧或者在界面上显示
	libvlc_video_set_callbacks(player, lock, unlock, display, &ctx);
	libvlc_video_set_format(player, "RGBA", VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_WIDTH * 4);

	libvlc_media_player_play(player);

	QThread::sleep(10);

	libvlc_media_release(media);
	libvlc_media_player_release(player);
	libvlc_release(instance);
}

void Player::setVolume(int vol)
{
    libvlc_audio_set_volume(m_player, vol);
}

void Player::seek(int pos)
{
    libvlc_media_t *curMedia = libvlc_media_player_get_media(m_player);
    if (nullptr == curMedia)
        return;

    libvlc_time_t duration = libvlc_media_get_duration(curMedia);
    float ms = float(pos)/100 * float(duration);
    libvlc_media_player_set_time(m_player, libvlc_time_t(ms));
}
