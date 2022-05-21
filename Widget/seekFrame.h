#ifndef SEEKFRAME_H
#define SEEKFRAME_H

#include<iostream>
#include<QImage>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class SeekFrame {
    /*
    做法:
        - 首先通过av_seek_frame API来找到离目标时间最近的前面的关键帧, 然后从
          该关键帧开始往后找, 直至遍历到的帧的时间和目标时间相差小于 minDistance.
          也就是说, 找到的帧和目标帧会有小于minDistance秒的误差.
          小视频找帧,耗时<0.5s. 但是视频越大延迟会越高.
        - 当minDistance取一个特别大的数字的时候, 此时就退化成了:找到离目标最近的关键帧,
          大视频关键帧很多,可考虑设minDistance为一个很大的数, 此时会非常流畅

    */
public:
    int maxFindTimes;
    double minDistance; // 超参数, 值越大, 找screencap的速度越快.
    double avd; // 视频流的timebase 对应的转换成了double类型的值
    int videoStreamIndex;
    int video_width = 0;
    int video_height = 0;
    AVFormatContext* formatContext = NULL; // 媒体上下文
    AVCodecContext* codecContext = NULL; // 视频流的codev
    struct SwsContext* sws_ctx = NULL; // RGB格式转换器

    // 构造函数, 用文件名filename, 最大寻找次数maxFindTimes, 以及目标帧和真实帧所在时间的距离
    SeekFrame(QString filename, int maxFindTimes, double minDistance);
    SeekFrame(){};
    // 初始化formatContext,codecContext,sws_ctx,等等
    void init(const char* filename);
    // 根据需要的时间: wanted_time 来获取该时间的帧
    AVFrame* getFrame(int wanted_time);
    static void save_rgb_frame(unsigned char* buf, int wrap, int xsize, int ysize, const char* filename)
    /*把AVFrame对象写入磁盘(debug 使用, 不会用到) */
    {
        FILE* f;
        int i;
        f = fopen(filename, "wb");
        fprintf(f, "P6\n%d %d\n255\n", xsize, ysize);

        for (i = 0; i < ysize; i++)
        {
            unsigned char* ch = (buf + i * wrap);
            fwrite(ch, 1, xsize * 3, f);  //Write 3 bytes per pixel.
        }

        fclose(f);
    }
};

#endif // SEEKFRAME_H
