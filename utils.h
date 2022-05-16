#ifndef UTILS_H
#define UTILS_H

#include<QString>
#include<QVector>

//class utils
//{
//public:
//    utils();
//};

bool isValidVideoFile(QString filename);
QString getFileName(QString Qpath);
QVector<QString>* readPlayList(QString defaultPath="./playList.dat");
bool writePlayList(QVector<QString> ,QString defaultPath="./playList.dat");

#endif // UTILS_H
