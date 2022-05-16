#ifndef VIDEOINFO_H
#define VIDEOINFO_H

#include <QDataStream>

class videoInfo
{
public:
    videoInfo();
//    videoInfo(const char* fileName, qint64 duration, int width,int height);
//    int getWidth();
//    int getHeight();
//    qint64 getDuration();
//    char* getFileName();
//private:
    QString fileName;
    qint64 duration;
    qint64 durationMs;
    int width, height;

};

#endif // VIDEOINFO_H
