#include "getaudio.h"

using namespace std;
GetAudio::GetAudio(QString fileName)
{
    init(fileName);
}
void GetAudio::init(QString fileName) {
    std::string fn_str = fileName.toStdString();
    const char* fn = fn_str.c_str();
    const char* output = "temp.pcm";
    formatContext = avformat_alloc_context();
    AVCodec* codec = NULL;  // 音频流的decoder
    AVCodecParameters* pLocalCodecParameters = NULL;
    audioStreamIndex = -1; // 音频流index
    // 读入文件header
    if(avformat_open_input(&formatContext,fn,NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return ;
    }
    //获取音频流信息
    if(avformat_find_stream_info(formatContext,NULL)<0){
        printf("Couldn't find stream information.\n");
        return ;
    }
    duration=(formatContext->duration)/ AV_TIME_BASE;
    // 寻找音频流index, 以及音频流的decoder
    for (int i = 0; i < formatContext->nb_streams; i++)
    {
        AVCodecParameters* pLocalCodecParameters_temp = NULL;
        pLocalCodecParameters_temp = formatContext->streams[i]->codecpar;

        AVCodec* localCodec = NULL;
        localCodec = avcodec_find_decoder(pLocalCodecParameters_temp->codec_id);
        if (localCodec == NULL)
        {
            printf("Error: Unsupported codec");
            continue;
        }
        if (pLocalCodecParameters_temp->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            if (audioStreamIndex == -1)
            {
                audioStreamIndex = i; // 找到了音频流的index
                codec = localCodec; // 找到了音频流的decoder
                pLocalCodecParameters = pLocalCodecParameters_temp;
            }

        }
    }
    //利用已有decoder的信息;来分配一个AVCodecContext
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) { std::cout << "Failed to allocate memory for AVCodecContext"; return ;}
    if (avcodec_parameters_to_context(codecContext, pLocalCodecParameters) < 0)
    {
        std::cout << "Failed to copy codec paramters to codec context"<<endl;
        return ;
    }

    // 初始化刚刚分配完空间的AVCodecContext, 仍然是用pCodec来初始化
    if (avcodec_open2(codecContext, codec, NULL) < 0)
    {
        std::cout<< "failed to open codec throught avcodec_open2"<<endl;
                    exit(0);
        return ;
    }
    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    enum AVSampleFormat out_sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
    int out_sample_rate = 44100;
    int64_t in_ch_layout = codecContext->channel_layout;
    enum AVSampleFormat in_sample_fmt = codecContext->sample_fmt;
    int in_sample_rate = codecContext->sample_rate;

    //拿到转换上下文 设置输入输出参数
    SwrContext* swrContext = swr_alloc_set_opts(NULL,
       out_ch_layout, out_sample_fmt, out_sample_rate,
       in_ch_layout, in_sample_fmt, in_sample_rate,
       0, NULL);
    if (swr_init(swrContext) < 0) {
       std::cout << "swr_init error";
       return;
    }
    AVPacket* avPacket = av_packet_alloc();
    AVFrame* avFrame = av_frame_alloc();
    uint8_t* out_buffer = (uint8_t*)(av_malloc(2 * 2 * 44100));
    //PCM数据输出的文件
    FILE* file = fopen(output, "wb");
    int frame_nb=0;
    int ret = 0;
    int datasize_=0;
    while (av_read_frame(formatContext, avPacket) >= 0) {
        if (avPacket->stream_index != audioStreamIndex) {
            continue;
        }
        ret = avcodec_send_packet(codecContext, avPacket);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        }
        else if ((ret == AVERROR(EINVAL)) || (ret == AVERROR_EOF) || (ret == AVERROR(ENOMEM))) {
            break;
        }
        else if (ret < 0) {
            std::cout << "legitimate decoding 1 errors ret="<< ret;
            exit(1);
            break;
        }
        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, avFrame);

        if (ret == AVERROR(EAGAIN)) {
            continue;
        }
        else if ((ret == AVERROR(EINVAL)) || (ret == AVERROR_EOF)) {
            break;
        }
        else if (ret < 0) {
            std::cout << "legitimate decoding 2 errors";
            exit(1);
            break;
        }
        //重采样 将 frame 数据写到输出缓冲区 out_buffer 里面
        ret = swr_convert(swrContext, &out_buffer, avFrame->nb_samples,
            (const uint8_t**)avFrame->data, avFrame->nb_samples);

        if (ret < 0) {
            char buf[1024];
            av_strerror(ret, buf, 1024);
            std::cout << "swr_convert error "<<ret<< buf<<endl;//Invalid argument
            exit(0);
            continue;
        }
        //计算 out_buffer 数量
        int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
        int data_size = ret * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
        frame_nb+=1;
        datasize_=data_size;
        fwrite(out_buffer, 1, data_size, file);
    }
    cout<<"total frame nb:"<<frame_nb<<endl;
    cout << "data_size="<< datasize_<<endl;
    total_len=datasize_*frame_nb;
    cout<<"total len:"<<total_len<<endl;

    //释放空间
    av_packet_free(&avPacket);
    av_free(avPacket);
    avPacket = NULL;
    fclose(file);
    av_free(out_buffer);
    out_buffer = NULL;
    swr_free(&swrContext);
    swrContext = NULL;
}

long GetAudio::loadAudio(int want_t) {
    long  base_data = long((total_len/duration)*want_t);
    long start_data=0;
//    cout<<base_data<<endl;
    //设置初始索引位置 为show_rate
    if (base_data<200){
        start_data=0;
    }else {
        start_data=base_data-200;
    }
    cout<<"start_data: "<<start_data<<endl;
    return start_data;
}



GetAudio::~GetAudio(){
    avcodec_close(codecContext); // close decoder
    avformat_close_input(&formatContext); //close input file
    avcodec_free_context(&codecContext);
    codecContext = NULL;
    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);
    formatContext = NULL;

}

