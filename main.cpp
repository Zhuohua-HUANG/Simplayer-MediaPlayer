#include "mainwindow.h"
#include "Widget/seekFrame.h"
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include <QString>
#include <inttypes.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>  // <-- Requiered for av_image_get_buffer_size
}
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"avcodec.lib")
//#pragma comment(lib,"swscale.lib")
//#pragma comment(lib,"swresample.lib")

using namespace std;
#include <QApplication>


int main(int argc, char *argv[])
{
    // 要记得在build下手动加入 ffmpeg的bin下的东西... ? 怎么才能不需要手动做这个呢?
    qDebug()<<av_version_info();
    qDebug()<<avcodec_configuration();
    QApplication a(argc, argv);

//    QVector<QString> *vec = new QVector<QString>;
//    vec->append("a");
//    vec->append("b");
//    vec->append("c");
//    bool w = writePlayList(*vec,"./data1.dat");
//    QVector<QString> *vec1 = readPlayList("./data1.dat");
//    for(int i = 0;i<vec1->size();i++){
//        qDebug()<<(*vec1)[i];
//    }
//    qDebug()<<w;
    MainWindow w;
    w.show();
    return a.exec();
}
