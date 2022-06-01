#ifndef REVERSEDECODE_H
#define REVERSEDECODE_H
#include <QThread>
#include "utils.h"
#include <QDebug>
#include <QMutex>
extern "C" {
#include <libavutil/opt.h>
#include <libavutil/mem.h>
#include <libavutil/fifo.h>
#include <libavutil/pixfmt.h>
#include <libavutil/log.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}

class ReverseDecode : public QThread
{
    Q_OBJECT
public slots:
    void seek(qint64 pos);
public:
    bool endProcessed;
    AVFormatContext *format_ctx = nullptr;
    int video_stream_index = -1;
    AVFrame *RGB24_pFrame = nullptr;
    AVFrame *SRC_VIDEO_pFrame = nullptr;
    uint8_t *out_buffer_rgb = nullptr;
    struct SwsContext *img_convert_ctx = nullptr;  //用于解码后的视频格式转换
    qint32 m_DifferTime= 40; //两帧相差的时间
    Controller *ctrl;
    qint64 cur;
    qint64 duration;
    int video_width = 0;
    int video_height = 0;
    qint64 deltaPts=-1; // 两个pts 之间的差值(正数)
    ReverseDecode(Controller *ctrl);

    int loadFile(QString);
    void run() override;
    void reInitialize();
};

#endif // REVERSEDECODE_H
