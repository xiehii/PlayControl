/****************************************
* @file    Player
* @brief   播放器
* @author  Waleon
* @wechat  高效程序员
* @date    2020/03/22
****************************************/

#ifndef PLAYER_H
#define PLAYER_H

//定义输出视频的分辨率
#define VIDEO_WIDTH 500
#define VIDEO_HEIGHT 300

#include <QObject>
#include <QDir>
#include <QMutex>
#include <QDebug>
#include <QThread>
#include <QDateTime>

class libvlc_instance_t;
class libvlc_media_player_t;
class libvlc_media_t;
class libvlc_event_manager_t;

struct Context
{
	QMutex mutex;
	uchar *pixels;
};


class Player : public QObject
{
    Q_OBJECT

public:
    // 播放状态
    typedef enum State {
        Idle,
        Opening,
        Buffering,
        Playing,
        Paused,
        Stopped,
        Ended,
        Error
    } State;

    explicit Player(QObject *parent = nullptr);
    ~Player();
    // 设置视频输出窗口
    void setVideoWindow(QWidget *window);
    // 获取当前状态
    Player::State state();
    //获取媒体信息
    void getMeta(libvlc_media_t *media);
    //获取媒体对象
    libvlc_media_t * getMedia();

	void firstVideo();

Q_SIGNALS:
    // 总时长发生变化
    void durationChanged(qint64 dur);
    // 当前时间发生变化
    void timeChanged(qint64 time);
    // 播放位置发生变化
    void positionChanged(float pos);
    // 状态发生变化
    void stateChanged(Player::State state);

public Q_SLOTS:
    //打开资源的路径
    void openURL(const QString &url);
    // 打开文件
    void openFile(const QString &file);
    // 设置音量
    void setVolume(int vol);
    // 跳播
    void seek(int pos);
    // 播放
    void play();
    // 暂停
    void pause();
    // 停止
    void stop();
    //裁剪
    void screenShot();
    //获取当前的宽高比
    QString GetCurrentAspectRadio();
    // 设置宽高比
    void SetCurrentAspectRadio(QString str);
	// 获取视频轨道的列表
	void GetVideoTracksList();
	//刷新当前网络资源的播放
	void update();
	//提取视频帧
	void extractFrames(QString str);

private:
    // 订阅事件
    void attachEvents();

private:
    libvlc_instance_t *m_instance          {nullptr};
    libvlc_media_player_t *m_player        {nullptr};
    libvlc_media_t *m_media                {nullptr};
    libvlc_event_manager_t *m_eventManager {nullptr};

    QWidget *m_videoWindow           {nullptr};
    unsigned int screenShotNum = 0;           //裁剪的第几个
	QString m_url;
};

Q_DECLARE_METATYPE(Player::State)

#endif // PLAYER_H
