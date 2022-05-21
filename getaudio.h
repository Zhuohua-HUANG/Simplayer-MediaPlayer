#ifndef GETAUDIO_H
#define GETAUDIO_H



#include<iostream>
#include<fstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
//#include <GLFW/glfw3.h>
}

class GetAudio {
    /*
    做法:
        - 在初始化的时候读取文件的音频，然后保存为temp.pcm文件方便读取。
        - 当loadAudio得到时间的时候，返回文件索引的start_data作为文件seek的位置。
        - start_data可以被AudioImage读取然后完成绘制操作。
    */
public:
    double avd; // 音频流的timebase 对应的转换成了double类型的值
    int audioStreamIndex;
    AVFormatContext* formatContext = NULL; // 媒体上下文
    AVCodecContext* codecContext = NULL; // 音频流的codev
    long total_len=0;
    long duration=0;

    // 构造函数, 用文件名filename, 最大寻找次数maxFindTimes, 以及目标帧和真实帧所在时间的距离
    GetAudio(const char* filename );
    GetAudio(){};
    // 初始化formatContext,codecContext,sws_ctx,等等
    void init(const char* filename);
    // 根据需要的时间: wanted_time 来获取该时间的帧
    long loadAudio(int want_t);

    ~GetAudio();
};

#endif // GETAUDIO_H
