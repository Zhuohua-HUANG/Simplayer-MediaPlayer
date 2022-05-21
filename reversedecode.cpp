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


void ReverseDecode::run(){
    int i = 1;
    while (true) {
        qDebug()<<"here1, cur="<<cur<<" i="<<i<<ctrl->isQuit;
        i+=1;
        if(cur <= 0)cur = 1*deltaPts; // 尚存的问题: seek到开头 (把倒放一直持续下去)

        //判断是否线程需要退出
        ctrl->quitMutex.lock();
        if(ctrl->isQuit){
            ctrl->quitMutex.unlock();qDebug()<<"DECODER: break while";
            break;
        }
        ctrl->quitMutex.unlock();

        // 首先查看是否需要seek
        ctrl->seekMutex.lock();
        if(ctrl->isDecoderSeek)
        {
            cur=ctrl->seekPos;
            ctrl->isDecoderSeek = false;
        }
        ctrl->seekMutex.unlock();

        AVPacket pkt;
        if (av_seek_frame(format_ctx, video_stream_index, cur, AVSEEK_FLAG_BACKWARD) < 0) { qDebug()<<"here seekframe break"; break; }
        avcodec_flush_buffers(format_ctx->streams[video_stream_index]->codec);
        std::vector<IMAGE_FRAME> V; // 用于装此段解码得到的QImage
        while (av_read_frame(format_ctx, &pkt) >= 0) {
            if (pkt.stream_index == video_stream_index) {
                // 原始帧解码
                if (avcodec_send_packet(format_ctx->streams[video_stream_index]->codec, &pkt) != 0)
                {
                    av_packet_unref(&pkt);//不成功就释放这个pkt
                    continue;
                }
                //接受后对视频帧进行解码  // 帧解码完了就放到这里面 SRC_VIDEO_pFrame
                if (avcodec_receive_frame(format_ctx->streams[video_stream_index]->codec, SRC_VIDEO_pFrame) != 0)
                {
                    av_packet_unref(&pkt);//不成功就释放这个pkt
                    continue;
                }

                //转格式, RGB24_pFrame为目标格式的帧
                sws_scale(img_convert_ctx,
                    (uint8_t const **)SRC_VIDEO_pFrame->data,
                    SRC_VIDEO_pFrame->linesize, 0, video_height, RGB24_pFrame->data,
                    RGB24_pFrame->linesize);
                if (SRC_VIDEO_pFrame->pts > cur) {
                    // qDebug()<<"RGB24_pFrame->pts 超界";
                    break;
                }



                //加载图片数据
                QImage img(out_buffer_rgb, video_width, video_height, QImage::Format_RGB888);
                struct IMAGE_FRAME image_frame;
                image_frame.image = img.copy();
                image_frame.video_clock = av_q2d(format_ctx->streams[video_stream_index]->time_base) * pkt.pts;
                image_frame.pts = SRC_VIDEO_pFrame->pts;
                image_frame.frameRate = format_ctx->streams[video_stream_index]->avg_frame_rate;

                V.push_back(image_frame);
                ctrl->Q_mutex.lock();
                qDebug()<<"pused pts:"<<SRC_VIDEO_pFrame->pts<<" Qsize:"<<ctrl->Q.size();
                ctrl->Q_mutex.unlock();

                //释放包
                av_packet_unref(&pkt);

                //检测 seek
                ctrl->seekMutex.lock();
                if(ctrl->isDecoderSeek)
                {
                    cur=ctrl->seekPos;
                    ctrl->isDecoderSeek = false;
                    // 丢掉V中的所有内容
                    V.clear();
                    ctrl->seekMutex.unlock();
                    break;
                }
                ctrl->seekMutex.unlock();

                //判断是否线程需要退出
                ctrl->quitMutex.lock();
                if(ctrl->isQuit){ctrl->quitMutex.unlock();break;}
                ctrl->quitMutex.unlock();
            }

        }

        //判断是否线程需要退出
        ctrl->quitMutex.lock();
        if(ctrl->isQuit){ctrl->quitMutex.unlock();break;}
        ctrl->quitMutex.unlock();

        while (V.size() != 0) { // 实际上这是一个if(V.size!=0){}else{}的结构, 但是为了方便随时退出if块, 而改用的while
            qDebug()<<"DECODER: 阻塞等待, current Q size="<<ctrl->Q.size();  // 解码线程大部分时间是阻塞在这里
            unique_lock<mutex> lk(ctrl->mutex_);
            ctrl->cvempt.wait(lk, [&]() { return ctrl->Q.size() < 2; });//避免解码过快占用大量内存

            //判断是否线程需要退出
            ctrl->quitMutex.lock();
            if(ctrl->isQuit){ctrl->quitMutex.unlock();break;}
            ctrl->quitMutex.unlock();

            // 判断是否处于seek状态
            ctrl->seekMutex.lock();
            if(ctrl->isDecoderSeek)
            {
                V.clear();
                ctrl->seekMutex.unlock();
                break;
            }
            ctrl->seekMutex.unlock();

            // 加入队列
            ctrl->Q_mutex.lock(); // seek 使用.
            cur = V[0].pts -1;//更新cur的位置
            if(deltaPts==-1)deltaPts = V[1].pts-V[0].pts;
            qDebug()<<"DECODER: 完成的解码的最后一帧的pts:"<<V.back().pts;
            ctrl->Q.push(V);
            ctrl->Q_mutex.unlock();
            qDebug()<<"DECODER: 完成一段解码!  Qsize: "<<ctrl->Q.size();
            ctrl->cvfull.notify_one();
            break;
        }
        ctrl->seekMutex.lock();
        if(!ctrl->isDecoderSeek&&V.size() == 0)cur -= deltaPts;
        ctrl->seekMutex.unlock();

    }
    {
        unique_lock<mutex> lk(ctrl->mutex_); // wait之前先锁住 mutex
        ctrl->cvempt.wait(lk, [&]() { return ctrl->Q.size() < 2; }); // 匿名函数, 等待直到队列中的帧数<2, 然后上锁
        ctrl->Q.push(vector<IMAGE_FRAME>());//最后压入一个空vector 通知渲染线程已经结束
        ctrl->cvfull.notify_one();
    }
    av_frame_free(&SRC_VIDEO_pFrame);
    av_frame_free(&RGB24_pFrame);
    qDebug()<<"DECODER: 成功退出decoder的while循环";
}


void ReverseDecode::seek(qint64 pos){

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
