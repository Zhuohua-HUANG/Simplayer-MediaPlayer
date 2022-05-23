#include "utils.h"
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QDebug>

void Controller::clear(){
    while (!Q.empty()) Q.pop();
}

void Controller::init(){
    clear();
    isDecoderSeek = false;
    isDisplayerSeek = false;
    isQuit = false;
}

void Controller::print(){
    qDebug()<<"----------------";
    qDebug()<<"isDecoderSeek"<< isDecoderSeek;
    qDebug()<<"isDisplayerSeek"<< isDisplayerSeek;
    qDebug()<<"isQuit"<< isQuit;
    qDebug()<<"Q.size"<< Q.size();
    qDebug()<<"----------------";
}

bool isValidVideoFile(QString filename){
    filename = filename.toLower();
    int lastindex = filename.lastIndexOf(".");
    QString type = "";
    for (int i = lastindex+1;i<filename.length();i++){
        type += filename[i];
    }
    if (type == "avi" || type == "wmv" || type == "mpg" || type == "mpeg" || type == "mov" || type == "rm"
            || type == "ram" || type == "swf" || type == "flv" || type == "mp4" || type == "mp3" || type == "wma"
            || type == "rmvb" || type == "mkv"){
        return true;
    }
    return false;
}

//获取文件的名称
QString getFileName(QString Qpath)
{
    std::string path = Qpath.toStdString();
    std::string name;
    for (int i = path.size() - 1; i > 0; i--)
    {
        if (path[i] == '\\' || path[i] == '/')
        {
            name = path.substr(i + 1);
            /*return "";*/
            break;
        }
    }
    QString res = QString::fromStdString(name);

    return res;
}


QVector<QString>* readPlayList(QString defaultPath){
    QFile file(defaultPath);
    QVector<QString> *playList = new QVector<QString>;
    file.open(QIODevice::ReadOnly);
    QDataStream input(&file);
    QString name;
    QString str;
    while (!input.atEnd()){
        QString buf;
        input>>buf;
        playList->append(buf);
     }

    file.close();
    return playList;


}


bool writePlayList(QVector<QString> playList,QString defaultPath){
    QFile file(defaultPath);
    if(file.open(QIODevice::WriteOnly| QIODevice::Truncate))
    {
        QDataStream out(&file);
        for(int i=0;i<playList.size();i++){
            out<<playList[i];
        }
        file.close();
        return true;
    }
    else{
        return false;
    }

}



QString getVideoInfo(QString fn){
    QString info_return="";
    std::string fn_str = fn.toStdString();
    const char* fileName = fn_str.c_str();
    AVFormatContext* formatContext = avformat_alloc_context();
    AVDictionaryEntry *tag = NULL;
    if(avformat_open_input(&formatContext,fileName,NULL,NULL)!=0){
        qDebug()<<"Couldn't open input stream.\n";
        return "-1";
    }
    //获取音频流信息
    if(avformat_find_stream_info(formatContext,NULL)<0){
        qDebug()<<"Could not find stream information\n";
        return "-1";
    }
    while (tag = av_dict_get(formatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))
    {
        QString keyString = tag->key;
        QString valueString = QString::fromUtf8(tag->value);
        info_return+=" "+keyString+": "+valueString+"\n";
        qDebug()<<info_return;
    }
    long duration=(formatContext->duration)/ AV_TIME_BASE;
    qDebug()<<formatContext;
    info_return+=" 格式: "+QString(formatContext->iformat->name)+"\n";
    info_return+=" 时长（s）: "+QString::number(duration)+"\n";
    info_return+=" 码率: "+QString::number(formatContext->bit_rate)+"\n";
    info_return+=" 流数: "+QString::number(formatContext->nb_streams)+"\n";
//    info_return+=" 开始时间（ms）:"+QString::number(formatContext->start_time_realtime)+"\n";
    info_return+=" fps探测尺寸: "+QString::number(formatContext->fps_probe_size)+"\n";
    info_return+=" 错误检测系数: "+QString::number(formatContext->error_recognition)+"\n";
    info_return+=" 格式探索分数: "+QString::number(formatContext->probe_score)+"\n";
    AVCodecParameters* pLocalCodecParameters_temp = NULL;
    for (int i = 0; i < (int)formatContext->nb_streams; i++)
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
            info_return+=" 音频编码器: "+QString(localCodec->long_name)+"\n";
            info_return+=" 音道数: "+QString::number(channel)+"\n";
            info_return+=" 音频采样率: "+QString::number(sample_rate)+"\n";
            info_return+=" 音频码率: "+QString::number(pLocalCodecParameters_temp->bit_rate)+"\n";
        }
        else if (pLocalCodecParameters_temp->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            //得到视频帧的宽高
            int video_width = formatContext->streams[i]->codecpar->width;
            int video_height = formatContext->streams[i]->codecpar->height;
            int video_pixel_format=formatContext->streams[i]->codecpar->format;
            info_return+=" 视频编码器: "+QString(localCodec->long_name)+"\n";
            info_return+=" 视频宽度: "+QString::number(video_width)+"\n";
            info_return+=" 视频高度: "+QString::number(video_height)+"\n";
            info_return+=" 像素格式: "+QString::number(video_pixel_format)+"\n";
        }
    }
    pLocalCodecParameters_temp = NULL;
    avformat_free_context(formatContext);
    delete tag;
    return info_return;

}

QString getSuffix(QString fileName){
    std::string filename_str = fileName.toStdString();
    std::string res=filename_str.substr(filename_str.find_last_of('.') + 1);
    QString suffix = QString::fromStdString(res);
    return suffix;
}

int mediaType(QString fileName){
    QString suffix = getSuffix(fileName);

    if(audioType.indexOf(suffix)>=0){
        return 1;
    }
    else if(videoType.indexOf(suffix)>=0){
        return 2;
    }
    else{
        return 0;
    }
}
