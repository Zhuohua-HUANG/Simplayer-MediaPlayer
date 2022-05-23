#ifndef UTILS_H
#define UTILS_H

#include<QString>
#include<QVector>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <thread>
#include <ctime>
#include <mutex>
#include <condition_variable>
#include <QLabel>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
};
using namespace std;
//class utils
//{
//public:
//    utils();
//};

struct IMAGE_FRAME
{
    QImage image;
    double video_clock;
    int64_t pts;
    AVRational frameRate;

};
static QVector<QString> audioType = {"mp3","wav"};
static QVector<QString> videoType = {"mp4","wmv","mkv","avi"};
class Controller
{
public:
    std::queue<vector<IMAGE_FRAME>> Q; // 存储倒放帧的队列
    mutex mutex_; // 保护条件变量
    bool isDecoderSeek = false;
    bool isDisplayerSeek = false;
    bool isQuit = false;
    qint64 seekPos;
    mutex seekMutex; // 用于保护isDecoderSeek,isDisplayerSeek 以及seekPos
    mutex Q_mutex;  // 用于保护队列
    mutex quitMutex; // 用于保护isQuit
    // 满和空的条件变量
    condition_variable cvfull;
    condition_variable cvempt;

    void clear();
    void init(); // 重新初始化这个controller的部件内容
    void print();
};

bool isValidVideoFile(QString filename);
QString getFileName(QString Qpath);
QVector<QString>* readPlayList(QString defaultPath="./playList.dat");
bool writePlayList(QVector<QString> ,QString defaultPath="./playList.dat");
QString getVideoInfo(QString);
QString getSuffix(QString fileName);

int mediaType(QString fileName);
#endif // UTILS_H
