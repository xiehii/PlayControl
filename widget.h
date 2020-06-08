/****************************************
* @file    Widget
* @brief   播放器
* @author  Waleon
* @wechat  高效程序员
* @date    2020/03/22
****************************************/

#ifndef WIDGET_H
#define WIDGET_H
#include <QKeyEvent>
#include <QMainWindow>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QDomDocument>
#include "player.h"
#include "mywidget.h"

class QLabel;
class QSlider;
class QPushButton;

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private Q_SLOTS:
    void openURL();
    void openFile();
    void play();
	void extractFrames();
    void stateChanged(Player::State state);
    void menuChildClicked();

	//void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *e);

private:
    void initConfigure();
    void initUI();
    void createConnections();
    QString timeFormat(qint64 ms);
	bool deleteDirectory(const QString &path);



private:
    QMenu *m_mediaMenu         {nullptr};
    QAction *m_openFileAction  {nullptr};
    QAction *m_openURLAction   {nullptr};

    QMenu *m_playMenu          {nullptr};

    QMenu *m_videoMenu         {nullptr};
    QAction *m_videoTrackAction {nullptr};//视频轨道
    QAction *m_mediaMsgAction   {nullptr};//媒体信息
    QAction *m_zoomAction       {nullptr};//缩放
    QMenu *m_aspectRatioMenu    {nullptr};//宽高比
    QAction *m_tailoringAction      {nullptr};//裁剪
	QAction *m_extractFrames{ nullptr };//提取视频帧
    QAction *m_screenshotAction {nullptr};//截图

    QMenu *m_subtitleAction     {nullptr};

    QMenu *m_toolAction         {nullptr};

    QMenu *m_helpAction         {nullptr};

	QMenu *m_updateAction             {nullptr };//刷新

    MyWidget *m_videoWindow     {nullptr};
    QSlider *m_posSlider       {nullptr};
    QSlider *m_volSlider       {nullptr};
    QPushButton *m_playButton  {nullptr};
    QPushButton *m_stopButton  {nullptr};
    QLabel *m_timeLabel        {nullptr};

    Player m_player;
    QStringList     m_aspectRatioList;
};

#endif // WIDGET_H
