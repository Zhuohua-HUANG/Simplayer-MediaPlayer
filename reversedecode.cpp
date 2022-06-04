#include "reversedecode.h"

ReverseDecode::ReverseDecode(Controller *ctrl)
{
    this->ctrl = ctrl;
}

int ReverseDecode::loadFile(QString fileName){
    qDebug()<<"Init filename:"<<fileName;
    reInitialize(); // 重新初始化
    const char* m_MediaFile = fileName.toStdString().c_str();
    int response = -1;
    format_ctx = avformat_alloc_context();
    response = avformat_open_input(&format_ctx, m_MediaFile, nullptr, nullptr);
    if(response!=0){qDebug()<<"ERROR:"<<"avformat_open_input";return -1;};
    response = avformat_find_stream_info(format_ctx, nullptr);
    if(response<0){qDebug()<<"ERROR:"<<"avformat_open_input";return -1;};
    // 获取解码器, 视频宽高
    for (int i = 0; i < (int)format_ctx->nb_streams; ++i)
    {
        const AVStream* stream = format_ctx->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            //查找解码器
            AVCodec *video_pCodec = avcodec_find_decoder(stream->codecpar->codec_id);
            //打开解码器
            if (avcodec_open2(stream->codec, video_pCodec, nullptr) != 0)
            {
                return -1;
            }
            video_stream_index = i;
            //得到视频帧的宽高
            video_width = stream->codecpar->width;
            video_height = stream->codecpar->height;
        }
    }
    /*设置视频转码器*/
    SRC_VIDEO_pFrame = av_frame_alloc();
    RGB24_pFrame = av_frame_alloc();// 存放解码后YUV数据的缓冲区

    //将解码后的YUV数据转换成RGB24
    img_convert_ctx = sws_getContext(video_width, video_height,
        format_ctx->streams[video_stream_index]->codec->pix_fmt, video_width, video_height,
        AV_PIX_FMT_RGB24, SWS_BICUBIC, nullptr, nullptr, nullptr);

    //计算RGB图像所占字节大小
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, video_width, video_height);

    //申请空间存放RGB图像数据
    out_buffer_rgb = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

    // avpicture_fill函数将ptr指向的数据填充到picture内,但并没有拷贝,只是将picture结构内的data指针指向了ptr的数据
    avpicture_fill((AVPicture *)RGB24_pFrame, out_buffer_rgb, AV_PIX_FMT_RGB24,
        video_width, video_height);


    // 记录视频duration
    duration = format_ctx->streams[video_stream_index]->duration;
    // 初始化一开始寻找的位置
    cur = duration + 5000;

    return 0;
}




void ReverseDecode::reInitialize(){
    format_ctx = nullptr;
    video_stream_index = -1;
    RGB24_pFrame = nullptr;
    SRC_VIDEO_pFrame = nullptr;
    out_buffer_rgb = nullptr;
    img_convert_ctx = nullptr;  //用于解码后的视频格式转换
    m_DifferTime= 40; //两帧相差的时间
    video_width = 0;
    video_height = 0;
    deltaPts=-1; // 两个pts 之间的差值(正数)


}
