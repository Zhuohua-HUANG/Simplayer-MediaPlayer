#include "mainwindow.h"
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
#include <libavutil/imgutils.h>
}


using namespace std;
#include <QApplication>


int main(int argc, char *argv[])
{
    qDebug()<<av_version_info();
    qDebug()<<avcodec_configuration();
    QApplication a(argc, argv);


    MainWindow w;
    w.show();
    return a.exec();
}
