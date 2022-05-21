#ifndef REVERSEDISPLAY_H
#define REVERSEDISPLAY_H

#include "Widget/VideoFrameDisplay.h"
#include "utils.h"
#include <QThread>
#include <QMutex>

class ReverseDisplay : public QThread
{
    Q_OBJECT
signals:
    void SendOneFrame(QImage); //输出信号
    void SendTime(qint64);
    void SendSecond(qint64);
public:
    QMutex pauseLock;
    volatile bool stop_;
    bool pause_;
    Controller *ctrl;
    VideoFrameDisplay *videoDisplayer;
    ReverseDisplay(Controller *ctrl,VideoFrameDisplay *videoDisplayer);
    void run() override;
    void pauseThread();
    void resumeThread();
};

#endif // REVERSEDISPLAY_H
