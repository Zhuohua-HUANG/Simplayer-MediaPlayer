/**
 * 叶海辉
 * QQ群121376426
 * http://blog.yundiantech.com/
 */

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

    isSliderMoving = false; // 初始默认没在播放

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

    emit sig_valueChanged(ratio);

}

void VideoSlider::mouseMoveEvent(QMouseEvent *event)
{

    m_posX = event->pos().x();

    double curr_x = double(m_posX);
    double curr_total = double(width());
    double ratio = curr_x/curr_total;
    emit  sig_moveValueChanged(ratio);
    QSlider::mouseMoveEvent(event);

}


void VideoSlider::setValue(int value)
{
    if (!isSliderMoving)
    QSlider::setValue(value);
}



