# include "getAudio.h"

using namespace std;
GetAudio::GetAudio(const char* fileName)
{
    init(fileName);
}
void GetAudio::init(const char* fileName) {
    const char* output = "C:/Users/24508/source/repos/Media-Player/temp/temp.pcm";
    formatContext = avformat_alloc_context();
    AVCodec* codec = NULL;  // 音频流的decoder
    AVCodecParameters* pLocalCodecParameters = NULL;
    audioStreamIndex = -1; // 音频流index
    // 读入文件header
    if(avformat_open_input(&formatContext,fileName,NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return ;
    }
    //获取音频流信息
    if(avformat_find_stream_info(formatContext,NULL)<0){
        printf("Couldn't find stream information.\n");
        return ;
    }
    duration=formatContext->duration/ AV_TIME_BASE;
    std::cout <<"++++++++++++++++++++++++++++++++++++++++++++"<<endl;
    std::cout << "Format: " << formatContext->iformat->name
        << " duration:" << duration
        << " bitrate:" << formatContext->bit_rate
        <<" nbstreams:"<<formatContext->nb_streams
        <<endl;

    //print
//    AVStream **stream = (formatContext->streams);
//    int den = (*stream)->avg_frame_rate.den;
//    int num = (*stream)->avg_frame_rate.num;
//    float rate = (float)num/(float)den;
//    int msnum = (formatContext->duration % 1000000) / 1000;
//    int yuzhen =  (int)(msnum * 1/rate);
//    cout << "total frame :"
//         <<  (formatContext->duration / AV_TIME_BASE) * (int)rate + yuzhen
//         << endl;
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
    cout<<"Audio Codec: "<<pLocalCodecParameters->channels<< " channels, sample rate "<< pLocalCodecParameters->sample_rate<<endl;
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
    //    SwrContext *swrContext = swr_alloc_set_opts(NULL,
    //                                                AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, avCodecContext->sample_rate,
    //                                                avCodecContext->channel_layout, avCodecContext->sample_fmt, avCodecContext->sample_rate,
    //                                                NULL, NULL);
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
//        cout<<endl;
//        cout<<endl;
        //这里要先判断 之前在拿到 frame 之后判断 老是报 -22 错误
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
//        cout<<avFrame->nb_samples<<endl<<"data:"<<avFrame->data<<endl;
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
//        cout<<"frame nb:"<<frame_nb<<endl;
        frame_nb+=1;
        datasize_=data_size;
        fwrite(out_buffer, 1, data_size, file);

        //        //拿的不对 再看看？
        //        ret = av_samples_get_buffer_size(NULL, channels, avFrame->format,
        //                                         AV_SAMPLE_FMT_S16, 1);
        //        fwrite(out_buffer, 1, ret, file);
    }
    cout<<"total frame nb:"<<frame_nb<<endl;
    cout << "data_size="<< datasize_<<endl;
    total_len=datasize_*frame_nb;
    cout<<"total len:"<<total_len<<endl;
    av_packet_free(&avPacket);
    av_free(avPacket);
    avPacket = NULL;
    fclose(file);
    av_free(out_buffer);
    out_buffer = NULL;
    swr_free(&swrContext);
    swrContext = NULL;
}

AVFrame* GetAudio::loadAudio(int want_t) {
    int WindowSizeX=1600;
    int WindowSizeY=900;
    int show_rate=200;
    float min_step=2.0/show_rate;
    long  base_data = long((total_len/duration)*want_t);
    long start_data=0;
    AVFrame* pFrameRGB = av_frame_alloc(); // 用于存储把frame转换成了RGB之后的帧
    cout<<base_data<<endl;


    if (base_data<20){
        start_data=0;
    }else {
        start_data=base_data-20;
    }
    cout<<"ss: "<<start_data<<endl;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        exit(1);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WindowSizeX, WindowSizeY, "让我看看 ", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
         exit(1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
        /*your draw*/
        // drawPoint();
        glClearColor (1.0, 1.0, 1.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT);

        glLineWidth(2);//设置线段宽度
        glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);

        short pcm_In;
        float xstart=-1.0;
        float ystart=0.0;
        float xend=0.0;
        float yend=0.0;
        float temp = 0.0;

        fstream f;
        f.open("C:/Users/24508/source/repos/Media-Player/temp/temp.pcm", ios::in|ios::binary);
        f.seekg(start_data,ios::beg);
        long times=0;
        while(!f.eof())
        {
            f.seekg(times*2,ios::cur);
            f.read((char*)&pcm_In, 2);     //pcm中每个数据大小为2字节，每次读取1个数据
//            cout<<pcm_In<<endl;
            times+=1;
            //确定绘制波形的折线两点坐标
            xstart=xstart+min_step;
            ystart=temp;
            xend=xstart+min_step;

            //-------------------------------------------------------------------------------------------------------------------------
            yend=(float)pcm_In/100000;       //short类型除以一个数要么是0要么是大于一的整数，所以坐标变换要注意先转为float

            //绘制折线
            if (xend>=1.0){
                break;
            }
            glVertex2f(xstart,ystart);
            glVertex2f(xend,yend);

//            cout<<xstart<<"   "<<xend<<endl;

            temp=yend;    //终点作为下一次的起点

            if (times==show_rate){
                break;
            }
        }
        f.close();
//        cout<<times<<endl;
//        xstart=0.0;
//        ystart=0.0;
//        xend=0.5;
//        yend=0.5;
//        glVertex2f(xstart,ystart);
//        glVertex2f(xend,yend);

        glEnd();
        //申请颜色数据内存
        int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, WindowSizeX, WindowSizeY, 1);
        unsigned char* frame_buffer = (uint8_t*)av_malloc(num_bytes);
        int response;
        response = av_image_fill_arrays(pFrameRGB->data,       //uint8_t *dst_data[4],
            pFrameRGB->linesize,   //int dst_linesize[4],
            frame_buffer,          //const uint8_t * src,
            AV_PIX_FMT_RGB24,      //enum AVPixelFormat pix_fmt,
            WindowSizeX,   //int width,
            WindowSizeY,  //int height,
            1);                    //int align);
        if (response < 0)
        {
            printf("av_image_fill_arrays Failed, response = %d\n", response);
        }
        pFrameRGB->width = WindowSizeX;
        pFrameRGB->height = WindowSizeY;
        //读取像素
        glReadPixels(0, 0, WindowSizeX, WindowSizeY, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);
        pFrameRGB->linesize[0]=WindowSizeX;
        //将数据写入文件

        //清理颜色数据内存
    glfwTerminate();
    return pFrameRGB;
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
