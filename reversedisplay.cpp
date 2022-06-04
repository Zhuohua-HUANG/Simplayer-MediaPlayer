#include "reversedisplay.h"

ReverseDisplay::ReverseDisplay(Controller *ctrl,VideoFrameDisplay *videoDisplayer)
{
    this->ctrl = ctrl;
    this->videoDisplayer = videoDisplayer;
    stop_=false;
    pause_=false;
}

void ReverseDisplay::run(){
    while (1) {
        ctrl->quitMutex.lock();
        if(ctrl->isQuit){ctrl->quitMutex.unlock();break;}
        ctrl->quitMutex.unlock();
        vector<IMAGE_FRAME> V;
        {
            unique_lock<mutex> lk(ctrl->mutex_);
            qDebug()<<"DISPLAYER:  正在等待QImage队列非空";
            ctrl->cvfull.wait(lk, [&]() { return ctrl->Q.size() > 0; });
            qDebug()<<"DISPLAYER:  等到了QImage队列, the size of Q="<< ctrl->Q.size();
            V = ctrl->Q.front();
            ctrl->Q.pop();
            ctrl->cvempt.notify_one();
        }
        if (V.size() == 0) {
            qDebug()<<"V size=0, quit";
            break;  // 我们要一直keep住这个渲染线程
        }
        for (auto i = V.rbegin(); i != V.rend(); i++) { // 反向迭代


            // 发出信号
            pauseLock.lock(); // 暂停锁

            // 查看是否seek
            ctrl->seekMutex.lock();
            if(ctrl->isDisplayerSeek){
                qDebug()<<"DISPLAYER: 检测到了seek";
                // 发现目前正在seek, 立刻退出渲染的循环

                //清空队列Q
                ctrl->Q_mutex.lock();
                ctrl->clear();
                qDebug()<<"DISPLAYER: 清理完毕,  Qsize:"<<ctrl->Q.size();
                ctrl->Q_mutex.unlock();

                //发出empty的信号(因为Q以及被清空)
                ctrl->cvempt.notify_one(); // 此时Q已经被清空, 需要唤醒正在阻塞(大部分情况下)的decoder线程

                // seek标记返回常态
                ctrl->isDisplayerSeek=false;

                ctrl->seekMutex.unlock();
                pauseLock.unlock();
                break;

            }
            ctrl->seekMutex.unlock();

            SendOneFrame((*i).image);  // 发送帧信号
            SendTime(qint64((*i).pts)); // 发送时间信号
            SendSecond((*i).video_clock); // 发送以秒为单位的时间信号

            pauseLock.unlock();
            int delta = int(1000* 1.0 / (double)av_q2d((*i).frameRate));
            //qDebug()<<"DISPLAYER: current pts:"<<(*i).pts<<" time:"<<(*i).video_clock<<"  sleep:"<<delta;

            //QThread::msleep(delta);  // 性能原因, 视频帧间距短的时候, sleep时间较少会导致卡顿

            QThread::msleep(40);  // 暂时固定住渲染的帧率
        }
    }
    qDebug()<<"DISPLAYER:  退出大循环";
}
void ReverseDisplay::pauseThread(){
    pauseLock.lock();
    pause_=true;
}

void ReverseDisplay::resumeThread(){
    pauseLock.unlock();
    pause_=false;
}


