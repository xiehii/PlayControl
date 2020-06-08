#include "widget.h"
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QDir>
#include <QTime>

Widget::Widget(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("libVLC 播放控制"));

    initConfigure();
    initUI();
    createConnections();

    m_player.setVideoWindow(m_videoWindow);
    m_player.setVolume(m_volSlider->value());
	m_player.firstVideo();
}

Widget::~Widget()
{

}

void Widget::openURL()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr(""),
                                         tr(u8"请给保存的地图起个名："), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
    {
        m_player.openURL(text);
    }
}

void Widget::initUI()
{
    m_mediaMenu = new QMenu(this);
    m_openFileAction = new QAction(this);
    m_openURLAction = new QAction(this);
    m_mediaMenu->addAction(m_openFileAction);
    m_mediaMenu->addAction(m_openURLAction);

    m_playMenu = new QMenu(this);

    m_videoMenu = new QMenu(this);
    m_videoTrackAction = new QAction(this);
    m_mediaMsgAction = new QAction(this);
    m_zoomAction = new QAction(this);
    m_aspectRatioMenu = new QMenu(this);
    m_tailoringAction = new QAction(this);
	m_extractFrames = new QAction(this);
    m_screenshotAction = new QAction(this);

    m_videoMenu->addAction(m_videoTrackAction);
    m_videoMenu->addSeparator();
    m_videoMenu->addAction(m_mediaMsgAction);
    m_videoMenu->addAction(m_zoomAction);
    m_videoMenu->addMenu(m_aspectRatioMenu);
    m_videoMenu->addAction(m_tailoringAction);
	m_videoMenu->addAction(m_extractFrames);
    m_videoMenu->addSeparator();
    m_videoMenu->addAction(m_screenshotAction);

    m_videoMenu->setTitle(QStringLiteral("视频"));
    m_videoTrackAction->setText(QStringLiteral("视频轨道"));
    m_mediaMsgAction->setText(QStringLiteral("媒体信息"));
    m_zoomAction->setText(QStringLiteral("缩放"));
    m_aspectRatioMenu->setTitle(QStringLiteral("宽高比"));
    foreach(QString str,m_aspectRatioList)
    {
        QAction *action = new QAction(this);
        action->setText(str);
        action->setCheckable(true);
        m_aspectRatioMenu->addAction(action);
        connect(action, &QAction::triggered, this, &Widget::menuChildClicked);
    }
    m_aspectRatioMenu->actions()[0]->setChecked(true);

    m_tailoringAction->setText(QStringLiteral("裁剪"));
	m_extractFrames->setText(QStringLiteral("提取视频帧"));
    m_screenshotAction->setText(QStringLiteral("截图"));

    m_subtitleAction = new QMenu(this);
    m_subtitleAction->setTitle(QStringLiteral("字幕"));

    m_toolAction = new QMenu(this);
    m_toolAction->setTitle(QStringLiteral("工具"));

    m_helpAction = new QMenu(this);
    m_helpAction->setTitle(QStringLiteral("帮助"));

	m_updateAction = new QMenu(this);
	m_updateAction->setTitle(QStringLiteral("刷新"));

    QWidget *widget = new QWidget(this);
    m_videoWindow = new MyWidget(this);
    m_posSlider = new QSlider(this);
    m_volSlider = new QSlider(this);

    m_playButton = new QPushButton(this);
    m_stopButton = new QPushButton(this);
    m_timeLabel = new QLabel(this);

    menuBar()->addMenu(m_mediaMenu);
    menuBar()->addMenu(m_playMenu);
    menuBar()->addMenu(m_videoMenu);
    menuBar()->addMenu(m_subtitleAction);
    menuBar()->addMenu(m_toolAction);
    menuBar()->addMenu(m_helpAction);
	menuBar()->addMenu(m_updateAction);
    setCentralWidget(widget);

    m_mediaMenu->setTitle(QStringLiteral("媒体"));
    m_openFileAction->setText(QStringLiteral("打开文件"));
    m_openURLAction->setText(QStringLiteral("输入视频路径"));
    m_playButton->setText(QStringLiteral("播放"));
    m_stopButton->setText(QStringLiteral("停止"));

    m_playMenu->setTitle(QStringLiteral("播放"));

    QString time = timeFormat(0);
    m_timeLabel->setText(QString("%1 / %2").arg(time).arg(time));

    // 设置背景为黑色
    QPalette plt = m_videoWindow->palette();
    plt.setColor(QPalette::Window, Qt::black);
    m_videoWindow->setMinimumSize(500,300);
    m_videoWindow->setPalette(plt);
    m_videoWindow->setAutoFillBackground(true);
    m_videoWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 设置为水平滑块
    m_posSlider->setOrientation(Qt::Horizontal);
    m_volSlider->setOrientation(Qt::Horizontal);

    m_posSlider->setRange(0, 100);
    m_volSlider->setRange(0, 100);
    m_volSlider->setFixedWidth(100);
    m_volSlider->setValue(50);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(m_playButton);
    hLayout->addWidget(m_stopButton);
    hLayout->addWidget(m_timeLabel);
    hLayout->addStretch();
    hLayout->addWidget(m_volSlider);
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_videoWindow);
    layout->addWidget(m_posSlider);
    layout->addLayout(hLayout);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);
    widget->setLayout(layout);
}

void Widget::createConnections()
{
    connect(m_openFileAction, &QAction::triggered, this, &Widget::openFile);
    connect(m_openURLAction, &QAction::triggered, this, &Widget::openURL);
    connect(m_playMenu, &QMenu::triggered, this, &Widget::play);
	connect(m_extractFrames, &QAction::triggered, this, &Widget::extractFrames);
	connect(m_updateAction, &QMenu::triggered, &m_player, &Player::update);
    connect(m_screenshotAction, &QAction::triggered, &m_player, &Player::screenShot);
    connect(m_posSlider, &QSlider::valueChanged, &m_player, &Player::seek);
    connect(m_volSlider, &QSlider::valueChanged, &m_player, &Player::setVolume);
    connect(m_videoTrackAction, &QAction::triggered, &m_player, &Player::GetVideoTracksList);


    connect(m_playButton, &QPushButton::clicked, [=](){
        if (m_player.state() == Player::Playing) {
            m_player.pause();
        } else {
             m_player.play();
        }
    });

    connect(m_stopButton, &QPushButton::clicked, &m_player, &Player::stop);
    connect(&m_player, &Player::durationChanged, [=](qint64 dur){
        QStringList list = m_timeLabel->text().split("/");
        QString time;
        if (list.count() == 2)
            time = list.at(0).trimmed();
        QString duration = timeFormat(dur);
        m_timeLabel->setText(QString("%1 / %2").arg(time).arg(duration));
    });
    connect(&m_player, &Player::timeChanged, [=](qint64 time){
        QStringList list = m_timeLabel->text().split("/");
        QString duration;
        if (list.count() == 2)
            duration = list.at(1).trimmed();
        QString t = timeFormat(time);
        m_timeLabel->setText(QString("%1 / %2").arg(t).arg(duration));
    });

    connect(&m_player, &Player::positionChanged, [=](float pos){
        m_posSlider->blockSignals(true);
        int realPos = qRound(pos * 100);
        m_posSlider->setValue(realPos);
        m_posSlider->blockSignals(false);
    });

    connect(&m_player, &Player::stateChanged, this, &Widget::stateChanged);
}

QString Widget::timeFormat(qint64 ms)
{
    QTime n(0, 0, 0);
    QTime t = n.addMSecs(ms);
    QString time = t.toString("hh:mm:ss");

    return time;
}

void Widget::openFile()
{
    QString file = QFileDialog::getOpenFileName(this,
                                                QStringLiteral("打开文件"),
                                                QDir::homePath(),
                                                QStringLiteral("多媒体文件(*)"));
    if (!file.isEmpty()) {
        file = QDir::toNativeSeparators(file);
        m_player.openFile(file);
    }
}

void Widget::play()
{
    if (m_player.state() == Player::Playing) {
        m_player.pause();
        m_playMenu->setTitle(QStringLiteral("播放"));
    } else {
         m_player.play();
         m_playMenu->setTitle(QStringLiteral("暂停"));
    }
}

void Widget::extractFrames()
{
	deleteDirectory(u8"./提取的视频帧");
	QDir file;
	file.mkdir(u8"./提取的视频帧");
	QString mediaPath = QFileDialog::getOpenFileName(this, u8"选择您需要提取的视频文件", "./", u8"视频文件(*.*)::All file (*.*)");
	if (!mediaPath.isEmpty())
		m_player.extractFrames(mediaPath);
	else
		QMessageBox::warning(this, u8"警告", u8"文件名为空");
}

bool Widget::deleteDirectory(const QString &path)
{
	if (path.isEmpty())
		return false;

	QDir dir(path);
	if (!dir.exists())
		return true;

	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
	QFileInfoList fileList = dir.entryInfoList();
	foreach(QFileInfo fi, fileList)
	{
		if (fi.isFile())
			fi.dir().remove(fi.fileName());
		else
			deleteDirectory(fi.absoluteFilePath());
	}
	return dir.rmpath(dir.absolutePath());
}

void Widget::stateChanged(Player::State state)
{
    switch (state) {
    case Player::Playing: {
        m_playButton->setText(QStringLiteral("暂停"));
        break;
    }
    case Player::Paused: {
        m_playButton->setText(QStringLiteral("播放"));
        break;
    }
    case Player::Stopped: {
        m_playButton->setText(QStringLiteral("播放"));
        m_posSlider->blockSignals(true);
        m_posSlider->setValue(0);
        m_posSlider->blockSignals(false);

        QString time = timeFormat(0);
        m_timeLabel->setText(QString("%1 / %2").arg(time).arg(time));
        break;
    }
    default:
        break;
    }
}

void Widget::menuChildClicked()
{
    m_player.SetCurrentAspectRadio(dynamic_cast<QAction*>(sender())->text());
    foreach(auto item,m_aspectRatioMenu->actions())
    {
        if(item->text() != dynamic_cast<QAction*>(sender())->text())
        {
            item->setChecked(false);
        }
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{

    if (event->key() == Qt::Key_F5)
    {
        m_videoWindow->setWindowFlags(Qt::Window);
        m_videoWindow->showFullScreen();
    }
    QWidget::keyPressEvent(event);
}

void Widget::keyReleaseEvent(QKeyEvent * e)
{
    QWidget::keyReleaseEvent(e);
}

void Widget::initConfigure()
{
    QString file_name = QDir::currentPath()+"/configture.xml";
    qDebug()<<file_name;
    if(file_name.isEmpty())
        return;

    QFile file(file_name);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::information(NULL, QString("title"), QString("open error!"));

        return;
    }

    QDomDocument document;
    QString error;
    int row = 0, column = 0;
    if(!document.setContent(&file, false, &error, &row, &column))
    {
        QMessageBox::information(NULL, QString("title"), QString("parse file failed at line row and column") +
                                 QString::number(row, 10) + QString(",") + QString::number(column, 10));

        return;
    }

    if(document.isNull())
    {
        QMessageBox::information(NULL, QString("title"), QString("document is null!"));

        return;
    }

    QDomElement root = document.documentElement();

    //root_tag_name为Menu
    QString root_tag_name = root.tagName();

    //menuItem
    QDomElement menuItem = root.firstChildElement();
    if(menuItem.isNull())
        return;

    QString person_tag_name = menuItem.tagName();

    QString name = menuItem.attributeNode("name").value();

    //获取子节点，数目为2
    QDomNodeList list = root.childNodes();
    int count = list.count();
    for(int i=0; i<count;++i)
    {
        QDomNode dom_node = list.item(i);
        QDomElement element = dom_node.toElement();

        //获取name值，等价
        QString name_1 = element.attributeNode("name").value();
        QString name_2 = element.attribute("name");

        //获取子节点，数目为n，包括：n个name
        QDomNodeList child_list = element.childNodes();
        int child_count = child_list.count();
        for(int j=0; j<child_count; ++j)
        {
            QDomNode child_dom_node = child_list.item(j);
            QDomElement child_element = child_dom_node.toElement();
            QString child_tag_name = child_element.tagName();
            QString child__tag_value = child_element.attribute("name");
            if(name_2 == "AspectRatio")
                m_aspectRatioList.append(child__tag_value);
        }
    }

//    //按照name、age、email、website的顺序获取值
//    QDomElement element = person.firstChildElement();
//    while(!element.isNull())
//    {
//        QString tag_name = element.tagName();
//        QString tag_value = element.text();
//        element = element.nextSiblingElement();
//    }

}

//void Widget::mouseDoubleClickEvent(QMouseEvent *event)
//{
//	Q_UNUSED(event);
//	if (m_videoWindow->isFullScreen()) {      //全屏状态，切换到非全屏状态
//		m_videoWindow->setWindowFlags(Qt::SubWindow);
//		m_videoWindow->showNormal();
//
//	}
//	else {                                 //非全屏，切换到全屏
//		m_videoWindow->setWindowFlags(Qt::Dialog);
//		m_videoWindow->setWindowFlags(Qt::Window);
//		m_videoWindow->showFullScreen();
//	}
//}
