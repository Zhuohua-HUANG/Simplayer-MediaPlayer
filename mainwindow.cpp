#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Widget/seekFrame.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Screencap 相关
    // 1, 初始化当前的视频
    const char* fileName = "C:/Users/24508/demo2.mp4";
    SeekFrame sf = SeekFrame(fileName,1000,0.5);// 小视频第三个参数设小些, 大视频第三个参数可设无穷大int型整数
    currVideoSeekFrame = sf; // 初始化seekFrame对象
    currVideoDuration = sf.formatContext->duration/1000000; // 初始化当前视频的时长



    // 组件初始化
    ui->pause_botton->setVisible(false); // 暂停按钮初始为不可见
    ui->sideBlock->setVisible(false); // 播放列表初始化为不可见
    this->ui->Frame_img->setVisible(false); // 帧缩略图一开始不可见
    this->ui->Frame_img->setStyleSheet("QLabel{background-color:rgb(128,128,253);}");

    // 连接鼠标离开视频播放条不显示screencap
    connect(ui->video_slider,&VideoSlider::sig_mouseLeave,this,[=](){
        this->ui->Frame_img->setVisible(false);
    });



    // ui:实现播放和暂停按钮的点击交替: 点击播放时, 显示暂停按钮. 点击暂停按钮, 再显示播放按钮
    connect(ui->play_button,&QPushButton::clicked,this,[=](){
        ui->pause_botton->setVisible(true);
        ui->play_button->setVisible(false);
    });
    connect(ui->pause_botton,&QPushButton::clicked,this,[=](){
        ui->play_button->setVisible(true);
        ui->pause_botton->setVisible(false);
    });

    // ui:设置打开和关闭侧边栏
    connect(ui->openFileList,&QPushButton::clicked,this,[=](){

        bool playListVisable = ui->sideBlock->isVisible();
        if(playListVisable){
            ui->sideBlock->setVisible(false);
            this->resize(this->width()-ui->sideBlock->width(),this->height());
        }
        else{
            ui->sideBlock->setVisible(true);
            this->resize(this->width()+ui->sideBlock->width(),this->height());
        }
    });


    // volume slider 音量改变信号的demo
    qDebug()<<ui->volume_slider->maximumSize();
    connect(ui->volume_slider,&QSlider::valueChanged,this,[=](){
        qDebug()<<ui->volume_slider->value();
        ui->volume_value->setNum(ui->volume_slider->value());
    });

    //视频的slider的信号的演示
    // 点击进度条的某个位置, 打印当前位置
    connect(ui->video_slider,SIGNAL(sig_valueChanged(double)),this,SLOT(test1(double)));
    // Screencap演示demo
    connect(ui->video_slider,SIGNAL(sig_moveValueChanged(double)),this,SLOT(showScreenCap(double)));




}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::test(){
    qDebug()<<"test here00\n";
}

void MainWindow::test1(double a){
    // feel free to delete this function
    qDebug()<<"鼠标点击的进度条百分比"<<a<<"\n";
}


void MainWindow::showScreenCap(double ratio){
    // 根据当前的比例来显示screencap
    // feel free to alter or delete this function
    int time = int(currVideoDuration*ratio);
    qDebug()<<"鼠标指向的进度条百分比"<<ratio<<"指向的秒数(int):"<<time;
    // 设置screencap可见
    this->ui->Frame_img->setVisible(true);
    // 当前鼠标的位置
    int x=this->mapFromGlobal(QCursor().pos()).x();
    // screencap固定高度, 宽度按照比例缩放
    int img_height = 100;
    int img_width = img_height*currVideoSeekFrame.codecContext->width/currVideoSeekFrame.codecContext->height;
    // 处理screencap位置越界的情况
    if(x+img_width>=this->width()){
        x-=img_width;
    }
    this->ui->Frame_img->setGeometry(x,this->ui->widget_controller->pos().y()-img_height-10,img_width,img_height);
    // 得到当前时间的frame
    AVFrame* pFrameRGB = currVideoSeekFrame.getFrame(time);
    // 将frame转化为 QImage对象
    QImage image = QImage(pFrameRGB->data[0], pFrameRGB->width, pFrameRGB->height,
                          pFrameRGB->linesize[0], QImage::Format_RGB888)
                         .copy().scaled(img_width,img_height);//可选加参数,Qt::IgnoreAspectRatio,Qt::SmoothTransformation
    // QImage渲染
    ui->Frame_img->setPixmap(QPixmap::fromImage(image));
    av_frame_free(&pFrameRGB);

}


void MainWindow::showOutControlWidget()
{

    mAnimation_ControlWidget->setDuration(800);

    int w = ui->widget_controller->width();
    int h = ui->widget_controller->height();
    int x = 0;
    int y = ui->centralwidget->height() - ui->widget_controller->height();

    if (ui->widget_controller->isHidden())
    {
        ui->widget_controller->show();
        mAnimation_ControlWidget->setStartValue(ui->widget_controller->geometry());
    }
    else
    {
        mAnimation_ControlWidget->setStartValue(ui->widget_controller->geometry());
    }

//    mAnimation_ControlWidget->setKeyValueAt(0, QRect(0, 0, 00, 00));
//    mAnimation_ControlWidget->setKeyValueAt(0.4, QRect(20, 250, 20, 30));
//    mAnimation_ControlWidget->setKeyValueAt(0.8, QRect(100, 250, 20, 30));
//    mAnimation_ControlWidget->setKeyValueAt(1, QRect(250, 250, 100, 30));
    mAnimation_ControlWidget->setEndValue(QRect(x, y, w, h));
    mAnimation_ControlWidget->setEasingCurve(QEasingCurve::Linear); //设置动画效果

    mAnimation_ControlWidget->start();

}

void MainWindow::hideControlWidget()
{
    mAnimation_ControlWidget->setTargetObject(ui->widget_controller);

    mAnimation_ControlWidget->setDuration(300);

    int w = ui->widget_controller->width();
    int h = ui->widget_controller->height();
    int x = 0;
    int y = ui->centralwidget->height() + h;

    mAnimation_ControlWidget->setStartValue(ui->widget_controller->geometry());
    mAnimation_ControlWidget->setEndValue(QRect(x, y, w, h));
    mAnimation_ControlWidget->setEasingCurve(QEasingCurve::Linear); //设置动画效果

    mAnimation_ControlWidget->start();
}
