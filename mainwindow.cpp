#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 组件初始化
    ui->pause_botton->setVisible(false); // 暂停按钮初始为不可见
    ui->sideBlock->setVisible(false); // 播放列表初始化为不可见
    this->ui->Frame_img->setVisible(false); // 帧缩略图一开始不可见
    this->ui->Frame_img->setStyleSheet("QLabel{background-color:rgb(128,128,253);}");

    // 连接鼠标离开视频播放条=>不显示缩略图
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
    // 指向进度条的某个位置, 打印当前位置
    connect(ui->video_slider,SIGNAL(sig_moveValueChanged(double)),this,SLOT(test2(double)));

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
void MainWindow::test2(double a){
    // feel free to delete this function
    qDebug()<<"鼠标指向的进度条百分比"<<a<<"\n";
    this->ui->Frame_img->setVisible(true);
    // 当前鼠标的位置
    int x=this->mapFromGlobal(QCursor().pos()).x();
    int y=this->mapFromGlobal(QCursor().pos()).y();
    int img_width = 200;
    int img_height = 100;
    qDebug()<<this->width();
    if(x+img_width>=this->width()){
        x-=img_width;
    }
    this->ui->Frame_img->setGeometry(x,this->ui->widget_controller->pos().y()-img_height-10,img_width,img_height);
    this->ui->Frame_img->setStyleSheet("QLabel{background-color:rgb(128,128,253);}");

    qDebug()<<"x,y="<<x<<" "<<y;
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
