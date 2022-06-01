#include "seekFrame.h"
SeekFrame::SeekFrame(QString fileName, int maxFindTimes, double minDistance)
{
    std::string fileName_str = fileName.toStdString();
    const char* fileName_const = fileName_str.c_str();
    init(fileName_const);
    this->maxFindTimes = maxFindTimes;
    this->minDistance = minDistance;
}
void SeekFrame::init(const char* fileName) {
    formatContext = avformat_alloc_context();
    AVCodec* codec = NULL;  // 视频流的decoder
    AVCodecParameters* codecParameters = NULL;
    videoStreamIndex = -1; // 视频流index

    // 读入文件header
    avformat_open_input(&formatContext, fileName, NULL, NULL);
    //获取视频流信息
    avformat_find_stream_info(formatContext, NULL);
    std::cout << "Format: " << formatContext->iformat->name
        << "duration" << formatContext->duration
        << "bitrate" << formatContext->bit_rate;
    // 寻找视频流index, 以及视频流的decoder
    for (int i = 0; i < (int)formatContext->nb_streams; i++)
    {
        AVCodecParameters* localCodecParameters = NULL;
        localCodecParameters = formatContext->streams[i]->codecpar;

        AVCodec* localCodec = NULL;
        localCodec = avcodec_find_decoder(localCodecParameters->codec_id);
        if (localCodec == NULL)
        {
            printf("Error: Unsupported codec");
            continue;
        }
        if (localCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            if (videoStreamIndex == -1)
            {
                videoStreamIndex = i; // 找到了视频流的index
                codec = localCodec; // 找到了视频流的decoder
                codecParameters = localCodecParameters;
                // 记录视频的高和宽
                video_width = localCodecParameters->width;
                video_height = localCodecParameters->height;
            }
        }
    }
    //利用已有decoder的信息;来分配一个AVCodecContext
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) { std::cout << "Failed to allocate memory for AVCodecContext"; return ;}
    if (avcodec_parameters_to_context(codecContext, codecParameters) < 0)
    {
        std::cout << "Failed to copy codec paramters to codec context";
        return ;
    }
    // 初始化刚刚分配完空间的AVCodecContext, 仍然是用pCodec来初始化
    if (avcodec_open2(codecContext, codec, NULL) < 0)
    {
        std::cout<< "failed to open codec throught avcodec_open2";
    }


    // 图片格式转换器(转换成RGB)
    sws_ctx = sws_getContext
    (
        codecContext->width,
        codecContext->height,
        codecContext->pix_fmt,
        codecContext->width,
        codecContext->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );
    // 初始化avd
    avd = av_q2d(formatContext->streams[videoStreamIndex]->time_base);



}

AVFrame* SeekFrame::getFrame(int wanted_time) {
    int response = 0;
    int howManypacketsToProcess = this->maxFindTimes;
    AVFrame* frame = av_frame_alloc(); // 用于接收Frame
    AVPacket* packet = av_packet_alloc(); //用于接收packet
    AVFrame* pFrameRGB = av_frame_alloc(); // 用于存储把frame转换成了RGB之后的帧
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
    unsigned char* frame_buffer = (uint8_t*)av_malloc(num_bytes);
    response = av_image_fill_arrays(pFrameRGB->data,       //uint8_t *dst_data[4],
        pFrameRGB->linesize,   //int dst_linesize[4],
        frame_buffer,          //const uint8_t * src,
        AV_PIX_FMT_RGB24,      //enum AVPixelFormat pix_fmt,
        codecContext->width,   //int width,
        codecContext->height,  //int height,
        1);                    //int align);
    if (response < 0)
    {
        printf("av_image_fill_arrays Failed, response = %d\n", response);
    }
    pFrameRGB->width = codecContext->width;
    pFrameRGB->height = codecContext->height;


    int64_t want_ts2 = int64_t((wanted_time) / avd);
    std::cout << "\n\nwant time " << wanted_time << std::endl;
    std::cout << "want sc " << want_ts2 << std::endl;
    av_seek_frame(formatContext, videoStreamIndex, want_ts2, AVSEEK_FLAG_BACKWARD);
    bool flag = false; // 标志有没有找到frame
    while (av_read_frame(formatContext, packet) >= 0 && flag == false)
    {
        if (packet->stream_index == videoStreamIndex)
        {
            response = avcodec_send_packet(codecContext, packet);
            if (response < 0)
            {
                std::cout << "Error sending packet to decoder";
                break;
            }
            while (response >= 0)
            {
                response = avcodec_receive_frame(codecContext, frame);
                if (response == AVERROR(EAGAIN))
                {   //Not data memory for frame, have to free and get more data
                    av_frame_unref(frame);
                    av_freep(frame);
                    break; // 去消耗process次数
                }
                if (response == AVERROR_EOF)
                {
                    printf("Error AVERROR_EOF");
                    break;// 去消耗process次数
                }
                double frame_time = (double)avd * frame->best_effort_timestamp;
                if (response >= 0 && (abs(frame_time - (double)wanted_time) < minDistance))
                {
                    // 把frame转换格式
                    response = sws_scale(sws_ctx, (unsigned char const* const*)(frame->data), (frame->linesize),
                        0, codecContext->height, pFrameRGB->data, pFrameRGB->linesize);
                    std::cout << "\nbest_time step:" << frame_time << std::endl;  // 这个时间非常准
                    std::cout << "frame timestep:" << frame->best_effort_timestamp << std::endl;
                    return pFrameRGB;
                }
            }
            if (--howManypacketsToProcess <= 0) // 找了howManypacketsToProcess次还找不到就别找了
            {
                break;
            }

        }
    }
    return NULL;
}


