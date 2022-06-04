#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include <QWidget>
#include <QSlider>

namespace Ui {
class VideoSlider;
}

class VideoSlider : public QSlider
{
    Q_OBJECT

public:
    explicit VideoSlider(QWidget *parent = 0);
    ~VideoSlider();

    void setValue(int value);
    double pointingRatio;
signals:

    void sig_valueChanged(double);
    void sig_valueChanged_v(qint64);
    void sig_moveValueChanged(double);
    void sig_mouseLeave();
    void sig_pressed();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);

private:
    int m_posX; // 当前进度条的位置
    bool isSliderMoving; // 标志位, 标记是否在播放

};

#endif // VIDEOSLIDERVIEW_H
