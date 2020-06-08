/****************************************
* @file    main
* @brief   播放器
* @author  Waleon
* @wechat  高效程序员
* @date    2020/03/22
****************************************/

#include <QApplication>
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<Player::State>("Player::State");

    Widget w;
    w.resize(640, 480);
    w.show();

    return app.exec();
}
