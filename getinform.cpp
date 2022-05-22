#include "getinform.h"
using namespace std;
GetInform::GetInform()
{
}

QString GetInform::get_inform_from_fn(QString fn){
    qDebug()<<"+++++++++++++in get infor:+++++++++++++++++++";
    QString info_return="";
    std::string fn_str = fn.toStdString();
    const char* fileName = fn_str.c_str();
        formatContext = avformat_alloc_context();
    // 读入文件header
    if(avformat_open_input(&formatContext,fileName,NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return NULL;
    }
    //获取音频流信息
    if(avformat_find_stream_info(formatContext,NULL)<0){
        printf("Couldn't find stream information.\n");
        return NULL;
    }
    AVDictionaryEntry *tag = nullptr;
    while (tag = av_dict_get(formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))
    {
        QString keyString = tag->key;
        QString valueString = QString::fromUtf8(tag->value);
        info_return+=keyString+":"+valueString+"\n ";
        qDebug()<<info_return;
    }
    long duration=(formatContext->duration)/ AV_TIME_BASE;
    qDebug()<<formatContext;
    info_return+=" 格式:"+QString(formatContext->iformat->name)+"\n";
    info_return+=" 时长（s）:"+QString::number(duration)+"\n";
    info_return+=" 码率:"+QString::number(formatContext->bit_rate)+"\n";
    info_return+=" 流数:"+QString::number(formatContext->nb_streams)+"\n";
//    info_return+=" 开始时间（ms）:"+QString::number(formatContext->start_time_realtime)+"\n";
    info_return+=" fps探测尺寸:"+QString::number(formatContext->fps_probe_size)+"\n";
    info_return+=" 错误检测系数:"+QString::number(formatContext->error_recognition)+"\n";
    info_return+=" 格式探索分数:"+QString::number(formatContext->probe_score)+"\n";
    AVCodecParameters* pLocalCodecParameters_temp = NULL;
    for (int i = 0; i < formatContext->nb_streams; i++)
    {
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
            int channel=pLocalCodecParameters_temp->channels;//音道数
            int sample_rate=pLocalCodecParameters_temp->sample_rate;//音频采样率
            info_return+=" 音频编码器:"+QString(localCodec->long_name)+"\n";
            info_return+=" 音道数:"+QString::number(channel)+"\n";
            info_return+=" 音频采样率:"+QString::number(sample_rate)+"\n";
            info_return+=" 音频码率:"+QString::number(pLocalCodecParameters_temp->bit_rate)+"\n";
        }
        else if (pLocalCodecParameters_temp->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            //得到视频帧的宽高
            int video_width = formatContext->streams[i]->codecpar->width;
            int video_height = formatContext->streams[i]->codecpar->height;
            int video_pixel_format=formatContext->streams[i]->codecpar->format;
            info_return+=" 视频编码器:"+QString(localCodec->long_name)+"\n";
            info_return+=" 视频宽度:"+QString::number(video_width)+"\n";
            info_return+=" 视频高度:"+QString::number(video_height)+"\n";
            info_return+=" 像素格式:"+QString::number(video_pixel_format)+"\n";
        }
    }
    pLocalCodecParameters_temp = NULL;
    avformat_close_input(&formatContext);
//    history.append(info_return);
//    qDebug()<<info_return;
    qDebug()<<"+++++++++++++++++++end get infor++++++++++++++++++";
    return info_return;
}

GetInform::~GetInform(){
    avformat_free_context(formatContext);
    formatContext = NULL;
//    qDeleteAll(history);
}
