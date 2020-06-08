QT += core gui
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlayControl
TEMPLATE = app

CONFIG += c++11

# 包含 pri
include($$PWD/../libVLC.pri)

HEADERS += \
        player.h \
        widget.h \
    mywidget.h

SOURCES += \
        main.cpp \
        player.cpp \
        widget.cpp \
    mywidget.cpp
