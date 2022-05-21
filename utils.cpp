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
