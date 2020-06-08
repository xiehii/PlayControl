#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent) : QWidget(parent)
{

}


void MyWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(this->isFullScreen()){      //全屏状态，切换到非全屏状态
        this->setWindowFlags(Qt::SubWindow);
        this->showNormal();

    }else{                                 //非全屏，切换到全屏
  //      this->setWindowFlags(Qt::Dialog);
        this->setWindowFlags(Qt::Window);
        this->showFullScreen();
    }
}

void MyWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        this->setWindowFlags(Qt::SubWindow);
        this->showNormal();
    }
	if (event->key() == Qt::Key_F5)
	{
		if (this->isFullScreen()) {      //全屏状态，切换到非全屏状态
			this->setWindowFlags(Qt::SubWindow);
			this->showNormal();
		}
		else {                                 //非全屏，切换到全屏
	 //      this->setWindowFlags(Qt::Dialog);
			this->setWindowFlags(Qt::Window);
			this->showFullScreen();
		}
	}
}

void MyWidget::keyReleaseEvent(QKeyEvent *e)
{

}
