#ifndef VideoFrameDisplay_H
#define VideoFrameDisplay_H

#include <QWidget>
#include <QPaintEvent>
#include <QDebug>
#include <QLabel>
#include <QTime>

///显示视频用的widget
///这个仅仅是显示视频画面的控件
class VideoFrameDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit VideoFrameDisplay(QWidget *parent = 0);
    ~VideoFrameDisplay();

    //画面旋转度数
    int m_nRotateDegree;
    void Set_Rotate(int Rotate);
    //获取当前图像
    QImage GetImage();

   // QLabel *time_lable;
    bool m_pause_state=false;
    qint64 m_play_time=0;
    qint64 m_start_time=0; //开始的时间
    qint64 m_pos_time=0;
signals:
    void s_VideoWidgetEvent(int type);
protected:
    void paintEvent(QPaintEvent *event);
    QImage src_mImage;
    QImage mImage; //记录当前的图像
    void mouseDoubleClickEvent(QMouseEvent *e);
public slots:
    ///播放器相关的槽函数
    void slotSetOneFrame(QImage img);
};

#endif // VideoFrameDisplay_H
