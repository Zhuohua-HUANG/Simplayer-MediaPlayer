#ifndef AUDIOIMAGE_H
#define AUDIOIMAGE_H



#include <QWidget>
#include<iostream>
#include<fstream>
#include <windows.h>
# include <GL/gl.h>
# include <GL/glu.h>
# include<QOpenGLWidget>
# include<QOpenGLFunctions>

class AudioImage : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    //初始化
    explicit AudioImage(QWidget *parent = nullptr,int width=300,int height=200);
    void initializeGL() Q_DECL_OVERRIDE;
    //paintGL为设置好startdata自动绘制
    void paintGL() Q_DECL_OVERRIDE;
    //改变窗口size
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    //start data 设置
    void set_startdata(long data);
private:
    int w,h;
    // startdata为索引位置
    long startdata;
signals:

};


#endif // AUDIOIMAGE_H
