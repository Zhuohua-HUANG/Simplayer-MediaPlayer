#ifndef GETINFORM_H
#define GETINFORM_H
# include <QString>
#include<iostream>
#include<fstream>
//#include <QList>
#include <QDebug>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
//#include <GLFW/glfw3.h>
}

class GetInform
{
public:
    GetInform();
    ~GetInform();
    QString get_inform_from_fn(QString fn);
//    QList<QString> get_history();
private:
    AVFormatContext* formatContext = NULL; // 媒体上下文
//    QList<QString> history={};
};

#endif // GETINFORM_H
