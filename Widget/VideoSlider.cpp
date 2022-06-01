
#include "VideoSlider.h"

#include <QDebug>
#include <QTimer>
#include <QResizeEvent>
#include <QStyle>

VideoSlider::VideoSlider(QWidget *parent) :
    QSlider(parent)
{
    setMouseTracking(true);

    this->setOrientation(Qt::Horizontal);

    isSliderMoving = false; // 标记视频有无在播放

}

VideoSlider::~VideoSlider()
{

}


void VideoSlider::mousePressEvent(QMouseEvent *event)
{

    m_posX = event->pos().x();

    double curr_x = double(m_posX);
    double curr_total = double(width());
    double ratio = curr_x/curr_total;
    int value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->pos().x(), width());
    setValue(value);
    emit sig_valueChanged(ratio);
    emit sig_valueChanged_v((qint64)this->value());
    emit sig_pressed();

}

void VideoSlider::mouseMoveEvent(QMouseEvent *event)
{

    m_posX = event->pos().x();

    double curr_x = double(m_posX);
    double curr_total = double(width());
    double ratio = curr_x/curr_total;
    pointingRatio = ratio; // 更新当前鼠标指向的值
    emit  sig_moveValueChanged(ratio);

    QSlider::mouseMoveEvent(event);

}
void VideoSlider::leaveEvent(QEvent *)
{
    qDebug()<<"leave";
    emit  sig_mouseLeave();
}

void VideoSlider::setValue(int value)
{

    QSlider::setValue(value);
}



