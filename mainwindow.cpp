
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Widget/seekFrame.h"
#include "reversedecode.h"
#include "reversedisplay.h"
#include "audioimage.h"
#include "utils.h"
#include <QDebug>
#include <QMediaPlayer>
#include <QTime>
#include <QMediaPlaylist>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /*
        现存的问题:
        1, 播放大视频的时候, 有时会播不出来 demo2.mp4
    */
    setMouseTracking(true);
    this->setWindowFlags(Qt::FramelessWindowHint);


    // init
//    int w = 100;
//    int heigh
//    ui->openGLWidget->wid
//    QString fn = "C:/Users/24508/Videos/demo4.mp4";
//    std::string fn_str = fn.toStdString();
//    const char* fn_const = fn_str.c_str();
//    currAudioGetFrame=GetAudio(fn_const);
//    pAudioImage =new AudioImage(ui->openGLWidget,ui->openGLWidget->width(),ui->openGLWidget->height());
//    qDebug()<<ui->openGLWidget->width();
//    qDebug()<<ui->openGLWidget->height();
////    setCentralWidget(pAudioImage);

//    //set window size
//    pAudioImage->setMinimumSize(ui->openGLWidget->width(),ui->openGLWidget->height());

//    //get data
//    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
//        for(int i = 1;i<60;i++){
//            long data_time=currAudioGetFrame.loadAudio(i);
//            //send data to AudioImage
//            pAudioImage->set_startdata(data_time);
//            pAudioImage->paintGL();
//            QThread::msleep(1000);
//        }
//    });



    //end



    initSystem();
    ui->normal_widget->setStyleSheet("background-color:black;");
    mediaPlayer = new QMediaPlayer;
    mediaPlayer->setVideoOutput(ui->normal_widget);

    playList = new QMediaPlaylist();
    mediaPlayer->setPlaylist(playList);
    initWdigets();
    connect2Player();
    createExtraWidget();



    // volume slider 音量改变信号的demo
    qDebug()<<ui->volume_slider->maximumSize();

    // Screencap演示demo
//    connect(ui->video_slider,SIGNAL(sig_moveValueChanged(double)),this,SLOT(showOscillograph(double)));
    connect(ui->video_slider,SIGNAL(sig_moveValueChanged(double)),this,SLOT(showScreenCap(double)));


}
void MainWindow::showOscillograph(double ratio){
//    if(loadedVideo){
//        // 根据当前的比例来显示screencap
//        // feel free to alter or delete this function
//        int time = int(durationForSeekFrame_S*ratio);
//        qDebug()<<"showOscillograph: time"<<time;
//        long data_time=currAudioGetFrame.loadAudio(time);
//        //send data to AudioImage
//        pAudioImage->set_startdata(data_time);
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::test(){
    qDebug()<<"test here00\n";
}

//void MainWindow::test1(int a){
//    // feel free to delete this function
//    qDebug()<<"鼠标点击的进度条百分比"<<a<<"\n";
//}


void MainWindow::showScreenCap(double ratio){
    if(loadedVideo){
        // 根据当前的比例来显示screencap
        // feel free to alter or delete this function
        int time = int(durationForSeekFrame_S*ratio);
        qDebug()<<"鼠标指向的进度条百分比"<<ratio<<"指向的秒数(int):"<<time;
        // 设置screencap可见

        frameLabel->setVisible(true);
        frameLabel->raise(); // 保证它在最前

        // 设置波形图可见
        int audioImageHeight = 50;

        pAudioImage->setVisible(true);
        pAudioImage->raise();



        // 当前鼠标的位置
        int x=this->mapFromGlobal(QCursor().pos()).x();
        // screencap固定高度, 宽度按照比例缩放
        int img_height = 100;
        int img_width = img_height*currVideoSeekFrame->codecContext->width/
                currVideoSeekFrame->codecContext->height; // 按照比例得到宽度
        // 处理缩略图位置越界的情况
        if(x+img_width>=this->width()){
            x-=img_width;
        }


        pAudioImage->resizeGL(img_width,audioImageHeight);
        long data_time=currAudioGetFrame->loadAudio(time);
        pAudioImage->set_startdata(data_time);

        frameLabel->setGeometry(x,this->ui->widget_controller->pos().y()-img_height-10,img_width,img_height);
        pAudioImage->setGeometry(x,this->ui->widget_controller->pos().y()-img_height-10-audioImageHeight,img_width,audioImageHeight);  // 波形图的显示



        // GL打印，缩略图重加载
//        this->ui->openGLWidget->setVisible(false);
//        this->ui->openGLWidget->paintGL();
//        this->ui->openGLWidget->setVisible(true);
        pAudioImage->paintGL();


        // 得到当前时间的frame
        if(currMediaType==2){
            // 是视频才有缩略图
            AVFrame* pFrameRGB = currVideoSeekFrame->getFrame(time);
            // 将frame转化为 QImage对象
            QImage image = QImage(pFrameRGB->data[0], pFrameRGB->width, pFrameRGB->height,
                                  pFrameRGB->linesize[0], QImage::Format_RGB888)
                                 .copy();//可选加参数,Qt::IgnoreAspectRatio,Qt::SmoothTransformation
            // .scaled(img_width,img_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            // QImage渲染
            frameLabel->setPixmap(QPixmap::fromImage(image.scaled(img_width,img_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
            // 释放掉该frame
            av_frame_free(&pFrameRGB);
        }
        else{
            frameLabel->setVisible(false);
            pAudioImage->setGeometry(x,this->ui->widget_controller->pos().y()-10-audioImageHeight,img_width,audioImageHeight);
            pAudioImage->paintGL();
        }


    }
    else{
        qDebug()<<"还没有初始化视频!";
    }




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

void MainWindow::playClicked(){
    if(!loadedVideo&&!isReverse){
        qDebug()<<"尚未加载视频";
        return;
    }
    // 处理点击了播放-暂停按钮的处理
    qDebug()<<"'current satate:"<<m_playerState;
    switch (m_playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        if(isReverse){
            this->reversePause();
        }else{play();}
        break;
    case QMediaPlayer::PlayingState:
        if(isReverse){
            this->reversePause();
        }else{pause();}
        break;
    }
    qDebug()<<"'after satate:"<<m_playerState;
}

void MainWindow::connect2Player(){
    //关闭, 最大化, 最小化
    connect(ui->close,&QPushButton::clicked,this,[=](){ this->close();});
    connect(ui->minimize,&QPushButton::clicked,this,[=](){ this->setWindowState(Qt::WindowMinimized);});
    connect(ui->maximize,&QPushButton::clicked,this,[=](){ this->setWindowState(Qt::WindowMaximized);});
    // ui:设置打开和关闭侧边栏
    connect(ui->openFileList,&QPushButton::clicked,this,[=](){

        bool playListVisable = ui->sideBlock->isVisible();
        if(playListVisable){
            ui->sideBlock->setVisible(false);
            this->resize(this->width()-ui->sideBlock->width(),this->height());
            ui->openFileList->setStyleSheet(
                        "image: url(:new/image/off1.png);"
                        "border-radius:0px; "
            );
        }
        else{
            ui->sideBlock->setVisible(true);
            this->resize(this->width()+ui->sideBlock->width(),this->height());
            ui->openFileList->setStyleSheet(
                        "image: url(:new/image/on1.png);"
                        "border-radius:0px; "
            );
        }
    });
    // 连接鼠标离开视频播放条不显示screencap
    connect(ui->video_slider,&VideoSlider::sig_mouseLeave,this,[=](){
        frameLabel->setVisible(false);
        pAudioImage->setVisible(false);
    });
    connect(ui->play_button,SIGNAL(clicked()),this,SLOT(playClicked()));
    connect(ui->pause_botton,SIGNAL(clicked()),this,SLOT(playClicked()));
    connect(mediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
//    connect(ui->video_slider,SIGNAL(sig_valueChanged(double)),this,SLOT(seek(double)));
    connect(ui->video_slider,SIGNAL(sig_valueChanged_v(qint64)),this,SLOT(seek(qint64)));
    connect(ui->volume_slider,SIGNAL(valueChanged(int)),SLOT(changeVolume(int)));
    connect(ui->volume_button,SIGNAL(clicked()),this,SLOT(changeMute()));
    connect(ui->addFile,SIGNAL(clicked()),this,SLOT(openFileButtonClicked()));
    connect(playList, &QMediaPlaylist::currentIndexChanged, this, &MainWindow::playlistPositionChanged);
    // demo:
    connect(ui->playList,&QListWidget::itemDoubleClicked, this, [=](){
        /*双击正向播放视频*/
        qDebug()<<"双击啦!";
        /*获取当前双击的item*/
        QListWidgetItem * item = ui->playList->item(ui->playList->currentIndex().row());

        mediaPlayer->pause();
//        initVideo(item->data(Qt::UserRole).toString(),true,false);
        initializeVideo(item->data(Qt::UserRole).toString());
        normalPlay();
    });
    connect(ui->playList, &QListWidget::itemClicked, this, [=](){
        qDebug()<<"当前点击的index"<<ui->playList->currentIndex().row();
        QListWidgetItem * item = ui->playList->item(ui->playList->currentIndex().row());
        qDebug()<<"当前点击的index的text"<<item->data(Qt::UserRole).toString();

    });
    connect(ui->forward_seconds,&QPushButton::clicked,this,[=](){ skipForwardOrBackward(true);});
    connect(ui->backward_seconds,&QPushButton::clicked,this,[=](){ skipForwardOrBackward(false);});
    connect(ui->next_video,&QPushButton::clicked,this,[=](){ changeVideo(true); });
    connect(ui->previous_video,&QPushButton::clicked,this,[=](){ changeVideo(false); });
    connect(ui->video_slider,&VideoSlider::sig_pressed,this,[=](){
        if(isReverse){
            reverseSeek(qint64(ui->video_slider->value()));
        }
    });
    connect(this,SIGNAL(sig_reverseProgress(qint64)),this,SLOT(reverseUpdateDurationInfo(qint64)));
    // 接收倒放的信号, 进行倒放
    connect(this,SIGNAL(sig_reversePlay(QString)),this,SLOT(reversePlay(QString)));

    //快捷键
    QShortcut *shortCutForPasueAndPlay = new QShortcut(Qt::Key_Space, this );
    QShortcut *shortCutForSkipForward = new QShortcut(Qt::Key_Right, this );
    QShortcut *shortCutForSkipBackward = new QShortcut(Qt::Key_Left, this );
    QShortcut *shortCutForMute = new QShortcut(Qt::CTRL+Qt::Key_M, this );
    connect(shortCutForMute,&QShortcut::activated,this,[=](){ changeMute();});
    connect(shortCutForSkipForward,&QShortcut::activated,this,[=](){ skipForwardOrBackward(true);});
    connect(shortCutForSkipBackward,&QShortcut::activated,this,[=](){ skipForwardOrBackward(false);});
    connect(shortCutForPasueAndPlay,&QShortcut::activated,this,[=](){playClicked();});
}

void MainWindow::createExtraWidget(){
    // 创建显示帧的QLabel 组件
    frameLabel = new QLabel(this);
    frameLabel->setGeometry(100,100,100,100);
    frameLabel->setVisible(false);


    // 创建用于显示波形图的组件
    pAudioImage = new AudioImage(this);
    pAudioImage->setObjectName(QString::fromUtf8("openGLWidget"));
    pAudioImage->setGeometry(QRect(10, 350, 200, 100));
    pAudioImage->setVisible(false);


//    pAudioImage =new AudioImage(ui->openGLWidget);
//    list->show();
//    ui->playList->addItem();

}


void MainWindow::positionChange(qint64 progress) // * progress 是当前的毫秒数
{
    qDebug()<<"MainWindow::positionChange:  progress"<<progress;
    qDebug()<<"MainWindow::positionChange:  total duration"<<mediaPlayer->duration();
    // 计算当前进度
    // 计算当前的秒数
    int curr = int(double(progress)/double(currVideoSeekFrame->formatContext->duration/1000)*100) ;
    qDebug()<<"curr"<<curr;
    if (!ui->video_slider->isSliderDown())
        qDebug()<<"max:"<<ui->video_slider->maximum();
        ui->video_slider->setValue(progress);

    currentPosition = progress;
    updateDurationInfo(progress/1000);  //progress/1000 = 秒数
}
void MainWindow::reverseUpdateDurationInfo(qint64 currentInfo){

    if(isReverse){
        qDebug()<<"MainWindow::reverseUpdateDurationInfo"<<currentInfo<<" reverseDurationSecond: "<<reverseDurationSecond;
        QString tStr;
        if (currentInfo ||reverseDurationSecond) {
            QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
                currentInfo % 60, (currentInfo * 1000) % 1000);
            QTime totalTime((reverseDurationSecond / 3600) % 60, (reverseDurationSecond / 60) % 60,
                reverseDurationSecond % 60, (reverseDurationSecond * 1000) % 1000);
            QString format = "mm:ss";
            if (reverseDurationSecond > 3600)
                format = "hh:mm:ss";
            tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
        }
        qDebug()<<"m_labelDuration"<<tStr;
        ui->current_time->setText(tStr);
    }
}

void MainWindow::updateDurationInfo(qint64 currentInfo){
    /*用于更新当显示的秒数*/
    qDebug()<<"currentInfo"<<currentInfo;
    qDebug()<<"second_duration"<<video->duration;

    QString tStr;
    if (currentInfo || video->duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((video->duration / 3600) % 60, (video->duration / 60) % 60,
            video->duration % 60, (video->duration * 1000) % 1000);
        QString format = "mm:ss";
        if (video->duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    qDebug()<<"m_labelDuration"<<tStr;
    ui->current_time->setText(tStr);
    // 若放完了, 则跳转到开头, 并暂停
    if(currentInfo==video->duration){
        mediaPlayer->setPosition(10);
        pause();
    }
}

void MainWindow::initVideoInfo(QString fileName){

    video->fileName = fileName;
    video->width = currVideoSeekFrame->video_width; /*这里有问题*/
    video->height = currVideoSeekFrame->video_width;
    video->durationMs = (qint64)(currVideoSeekFrame->formatContext->duration/1000);
    video->duration = (qint64)(currVideoSeekFrame->formatContext->duration/1000000);
}




void MainWindow::seek(qint64 time){
    /*正放的时候跳转到某个位置*/
    mediaPlayer->setPosition(time);
}
void MainWindow::changeVolume(int volume){
    mediaPlayer->setVolume(volume);
}

void MainWindow::changeMute(){
    mediaPlayer->setMuted(m_playerMuted);
    m_playerMuted = !m_playerMuted;
}

void MainWindow::jump(int second){
    if(!loadedVideo){
        qDebug()<<"尚未加载视频";
        return;
    }
    mediaPlayer->setPosition(currentPosition+second*1000);
}

void MainWindow::addVideoItem(QString fileName){
    int mediaTypeTemp = mediaType(fileName);

    QString shortFilename = getFileName(fileName);
    qDebug()<<"加入!";
//    QString fileName="xxxx.mp4";



    QWidget *widget_3 = new QWidget(ui->playList);
    widget_3->setObjectName(QString::fromUtf8("widget_3"));
    widget_3->setMaximumSize(QSize(16777215, 1651461));
    QHBoxLayout *horizontalLayout_4 = new QHBoxLayout(widget_3);
    horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
    QLabel *label = new QLabel(widget_3);
    label->setText(shortFilename);
    label->setStyleSheet("color: rgb(246,246,246);");
    label->setObjectName(QString::fromUtf8("label"));
    QFont font;
    font.setFamily(QString::fromUtf8("Arial"));
    font.setPointSize(12);
    label->setFont(font);

    QPushButton *pushButton_3 = new QPushButton(widget_3);
    pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
    pushButton_3->setMinimumSize(QSize(30, 30));
    pushButton_3->setMaximumSize(QSize(30, 30));
    pushButton_3->setStyleSheet(QString::fromUtf8("QPushButton{ \n"
            "image: url(:new/image/info_1.png);\n"
            "border-radius:0px; \n"
            "padding:0px;\n"
            "}  \n"
            "QPushButton:hover{ \n"
            "image: url(:new/image/info_2.png);\n"
            "border-radius:0px; \n"
            "padding:0px;\n"
            "} \n"
            "\n"
            "QPushButton:checked{ \n"
            "image: url(:new/image/info_1.png);\n"
            "} \n"
            "\n"
            ""));

    horizontalLayout_4->addWidget(pushButton_3);

    horizontalLayout_4->addWidget(label);

    QSpacerItem * horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_4->addItem(horizontalSpacer_4);

    QPushButton *pushButton_2 = new QPushButton(widget_3);
    pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
    pushButton_2->setMinimumSize(QSize(30, 30));
    pushButton_2->setMaximumSize(QSize(30, 30));
    pushButton_2->setStyleSheet(QString::fromUtf8("QPushButton{ \n"
        "image: url(:new/image/delete.png);\n"
        "border-radius:0px; \n"
        "padding:0px;\n"
        "}  \n"
        "QPushButton:hover{ \n"
        "image: url(:new/image/delete_hover.png);\n"
        "border-radius:0px; \n"
        "padding:0px;\n"
        "} \n"
        "\n"
        "QPushButton:checked{ \n"
        "image: url(:new/image/delete.png);\n"
        "} \n"
        "\n"
        ""));

    horizontalLayout_4->addWidget(pushButton_2);


    /*倒放按钮*/
    QPushButton *pushButton = NULL;
    if(mediaTypeTemp==2){
        // 说明是视频
        pushButton = new QPushButton(widget_3);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(30, 30));
        pushButton->setMaximumSize(QSize(30, 30));
        pushButton->setStyleSheet(QString::fromUtf8("QPushButton{ \n"
            "image: url(:new/image/playback.png);\n"
            "border-radius:0px; \n"
            "padding:0px;\n"
            "}  \n"
            "QPushButton:hover{ \n"
            "image: url(:new/image/playback_hover.png);\n"
            "border-radius:0px; \n"
            "padding:0px;\n"
            "} \n"
            "QPushButton:checked{ \n"
            "image: url(:new/image/playback.png);\n"
            "} \n"
            "\n"
            ""));

        horizontalLayout_4->addWidget(pushButton);
    }




    QListWidgetItem *Qitem=new QListWidgetItem(ui->playList,0);
    Qitem->setSizeHint(QSize(120,40));
    ui->playList->setItemWidget(Qitem,widget_3);
    Qitem->setData(Qt::UserRole,fileName);
    // 在创建的时间就连接好这个删除键
    connect(pushButton_2,&QPushButton::clicked,this,[=](){
        ui->playList->removeItemWidget(Qitem);
        int removeIndex = -1;
        for (int i = 0;i<playListLocal->size();i++) {
            if((*playListLocal)[i]==Qitem->data(Qt::UserRole).toString()){
                removeIndex = i;
            }
        }
        if(removeIndex!=-1){
            playListLocal->removeAt(removeIndex);
            writePlayList(*playListLocal);
        }

        delete Qitem;
    });
    // 读取视频的信息
    QString info = getVideoInfo(fileName);
    // 连接信息键和信息弹窗
    connect(pushButton_3,&QPushButton::clicked,[=](){
        QMessageBox msgBox;
        msgBox.setStyleSheet(
                    "QMessageBox"
        "{"
           " border:none;"
            "background-color: rgb(255,255,255);"
            "border-radius: 15px;"
        "}"
                  "  QPushButton{ "
                "    image: url(C:/Users/24508/Downloads/ok.png);"
               "     border-radius:0px; "
              "      padding:1px;"
              "      }  "
        );
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.button(QMessageBox::Ok)->setText("");
        msgBox.button(QMessageBox::Ok)->setMinimumSize(QSize(25, 25));
        msgBox.setIcon(QMessageBox::NoIcon);
        QIcon * windowIcon = new QIcon("C:/Users/24508/Downloads/info.png");
        msgBox.setWindowIcon(*windowIcon);
        msgBox.setWindowTitle("视媒体信息");
        msgBox.setText(info);
        msgBox.exec();
    });

    // 连接倒放按钮和信号
    if(pushButton!=NULL){
        // 说明是视频
        connect(pushButton,&QPushButton::clicked, this, [=](){
            emit sig_reversePlay(Qitem->data(Qt::UserRole).toString());
        });
    }

}


void MainWindow::playlistPositionChanged(int currentItem){
    qDebug()<<"现在的currentItem"<<currentItem;
//    ui->playList->setCurrentIndex()
}

void MainWindow::openFileButtonClicked(){
    QString filename;
    QT_TRY{
        filename = QFileDialog::getOpenFileName();
        if (filename == ""){
            return;
        }
        if(!isValidVideoFile(filename)){
            QMessageBox::warning(this, tr("Error"),
                                           tr("打开的文件不是音视频格式")
                                           );
            return;
        }



        if(!isReverse)mediaPlayer->pause();
        addVideoItem(filename);
        playListLocal->append(filename); // 把文件名写入到本地
//        initVideo(filename,true,false);
        qDebug()<<"openFileButtonClicked1"<<isReverse;
        initializeVideo(filename);
        qDebug()<<"openFileButtonClicked2"<<isReverse;
        normalPlay();
        if(writePlayList(*playListLocal)){
            qDebug()<<"成功写入";
        }
        else {
            qDebug()<<"发生异常";
        }
    }
    QT_CATCH(QMediaPlayer::Error e)
    {
        qDebug()<<"出错了";
        return;
    }
    // 添加到文件列表



}

void MainWindow::initializeVideo(QString fileName){
    /*用于初始化视频, 无论是正放还是倒放都要用到*/
    currMediaType = mediaType(fileName);

    // 记录当前播放的路径
    currentVideoPath = fileName;
    // 初始化SeekFrame

    deleteSeekFrame();
    initSeekFrame(fileName);
    // 初始化波形图
    deleteWaveForm();
    initWaveForm(fileName);

    // 初始化文件信息
    video = new videoInfo();
    initVideoInfo(fileName);




}

void MainWindow::highlightInFileList(){
    // 在播放列表中高亮currentVideoPath
    int numVideos = ui->playList->count();
    if(numVideos!=0){
        int currentVideoIndex = -1;
        for(int i = 0;i<numVideos;i++){
            QString i_address = ui->playList->item(i)->data(Qt::UserRole).toString();
            if(currentVideoPath==i_address){
                currentVideoIndex = i;
                break;
            }
        }
        if(currentVideoIndex!=-1){
            ui->playList->item(currentVideoIndex)->setSelected(true);
        }
    }
}

void MainWindow::showNormalWidget(){
    ui->normal_widget->setVisible(true);
//    ui->reverse_widget->setVisible(false);
    ui->reverse_widget->setVisible(false);
}
void MainWindow::showReverseWidget(){
//    ui->normal_widget->setVisible(false);
    ui->normal_widget->setVisible(false);
    ui->reverse_widget->setVisible(true);
}
void MainWindow::normalPlay(){
    /*正放视频*/
    /*需要先使用initializeVideo 初始化*/

    // multimedia设置当前播放的媒体
    mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(currentVideoPath)));
    // 在媒体库中高亮
    highlightInFileList();
    playHistory->append(currentVideoPath);
    // 若正在倒放, 则清掉
    if(isReverse){
        qDebug()<<"normalPlay被调用";
        if(m_playerState==QMediaPlayer::StoppedState){
            qDebug()<<"当前在暂停";
            /*此时倒放在暂停,要先解锁*/
            reverseDisplayer->resumeThread();
        }
        quitCurrentReversePlay();
    }
    isReverse=false;
    showNormalWidget();
    play();

    // 设置播放进度条最大值(秒数)
    qDebug()<<"MainWindow::normalPlay  "<<mediaPlayer->duration();
    qDebug()<<"MainWindow::normalPlay s "<<video->duration;
    qDebug()<<"MainWindow::normalPlay ms "<<video->durationMs;
    ui->video_slider->setMaximum((int)video->durationMs);

    // 设置标记位
    if(!loadedVideo){
        loadedVideo = true;
    }

}

void MainWindow::initVideo(QString filename, bool isPlay, bool reverse){

    // 初始化当前文件路径, 视频信息, SeekFrame
    currentVideoPath = filename;
    std::string str = filename.toStdString();
    const char* fileName = str.c_str();
    video = new videoInfo();
    // 初始化SeekFrame
    deleteSeekFrame();
    initSeekFrame(filename);

    video = new videoInfo();
    initVideoInfo(fileName);
    mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(fileName)));

    // 在播放列表中高亮(输入:currentVideoPath)
    int numVideos = ui->playList->count();
    if(numVideos!=0){
        int currentVideoIndex = -1;
        for(int i = 0;i<numVideos;i++){
            QString i_address = ui->playList->item(i)->data(Qt::UserRole).toString();
            if(currentVideoPath==i_address){
                currentVideoIndex = i;
                break;
            }
        }
        if(currentVideoIndex!=-1){
            ui->playList->item(currentVideoIndex)->setSelected(true);
        }
    }
    /*下面判断到底要正放还是倒放*/
    if(reverse){
        //倒放
        if(isPlay){
            //开始播放
            // 记录播放过的视频
            playHistory->append(filename);

        }
        else{
            // 暂时不播放
        }
    }
    else{
        //正放

        if(isPlay){
            //开始正放
            quitCurrentReversePlay(); // 有问题???
            isReverse=false;
            showNormalWidget();

            mediaPlayer->play();
            play(); // 播放按钮和状态更新
            // 记录播放过的视频
            playHistory->append(filename);
        }
        else{
            // 暂时不播放
        }
    }





    // 设置标记位
    if(!loadedVideo){
        loadedVideo = true;
    }


}





void MainWindow::initSystem(){
    loadedVideo = false; // 标记目前还没有加载视频
    isReverse = false; // 初始化倒放标志
    playHistory = new QVector<QString>;
    playListLocal = readPlayList();
    if(playListLocal->size()!=0){
        for (int i=0;i<playListLocal->size();i++) {
            addVideoItem((*playListLocal)[i]);
        }
    }

    /*初始化倒放组件*/
    ctrl = new Controller;
    reverseDisplayer = new ReverseDisplay(ctrl,ui->reverse_widget);
    reverseDecoder = new ReverseDecode(ctrl);

    /*初始化seekFrame*/
    currVideoSeekFrame = NULL;

    /*初始化波形图*/
    pAudioImage = NULL;
    currAudioGetFrame = NULL;

}

void MainWindow::contextMenuEvent(QContextMenuEvent *e)
{
    if(!isReverse&&loadedVideo){
        QMenu *menu = new QMenu();
        menu->setFixedWidth(160);
        menu->addAction(rtText);
        menu->addSeparator();
        menu->addAction(rt0_5);
        menu->addAction(rt0_75);
        menu->addAction(rt1_0);
        menu->addAction(rt1_25);
        menu->addAction(rt1_5);
        menu->addAction(rt2_0);
        menu->addAction(rt3_0);
        menu->exec(e->globalPos());
        delete menu;
    }

}


void MainWindow::changePlayingRatio(float rt){
    qDebug()<<"改变速率"<<rt;
    if(!isReverse && loadedVideo){
        // 只有在正放的时候, 才能设置播放速率
        mediaPlayer->setPlaybackRate(qreal(rt));
    }
}

void MainWindow::initWdigets(){
    /*此函数用于初始化组件*/

    // 初始化播放按钮
    ui->pause_botton->setVisible(false); // 暂停按钮初始为不可见
    ui->sideBlock->setVisible(false); // 播放列表初始化为不可见

    rtText = new QAction("调整倍速",ui->normal_widget); // parent=this is the same with that parent=ui->widget? why?
    rt0_5 = new QAction("x0.5",ui->normal_widget);
    rt0_75 = new QAction("x0.75",ui->normal_widget);
    rt1_0 = new QAction("x1.0",ui->normal_widget);
    rt1_25 = new QAction("x1.25",ui->normal_widget);
    rt1_5 = new QAction("x1.5",ui->normal_widget);
    rt2_0 = new QAction("x2.0",ui->normal_widget);
    rt3_0 = new QAction("x3.0",ui->normal_widget);

    connect(rt0_5,&QAction::triggered,[=](){changePlayingRatio(0.5);});
    connect(rt0_75,&QAction::triggered,[=](){changePlayingRatio(0.75);});
    connect(rt1_0,&QAction::triggered,[=](){changePlayingRatio(1.0);});
    connect(rt1_25,&QAction::triggered,[=](){changePlayingRatio(1.25);});
    connect(rt1_5,&QAction::triggered,[=](){changePlayingRatio(1.5);});
    connect(rt2_0,&QAction::triggered,[=](){changePlayingRatio(2.0);});
    connect(rt3_0,&QAction::triggered,[=](){changePlayingRatio(3.0);});




}

void MainWindow::skipForwardOrBackward(bool mode)
{
    if(!loadedVideo){
        qDebug()<<"skipForwardOrBackward:尚未加载视频";
        return;
    }
    if(mode){
        // 前进
        int jumpSecond = 5;
        if(currentPosition+jumpSecond*1000>=video->durationMs){
            // 减10是为了不要跳到最后
            mediaPlayer->setPosition(video->durationMs-10);
            qDebug()<<"jump end";
        }
        else{
            jump(jumpSecond);
        }
    }
    else{
        // 后退
        int jumpSecond = -5;
        if(currentPosition+jumpSecond*1000<=0){
            mediaPlayer->setPosition(10);
            qDebug()<<"jump start";
        }
        else{
            jump(jumpSecond);
        }

    }
//    int jumpSecond = 10;
//    if(currentPosition+jumpSecond*1000>=video->durationMs){
//        mediaPlayer->setPosition(video->durationMs);
//        qDebug()<<"jump end";
//    }
//    else{
//        qDebug()<<"jump 10";
//        jump(10);
//    }
}
void MainWindow::test1(bool input){
    qDebug()<<input;
}

void MainWindow::pause(){
    m_playerState = QMediaPlayer::StoppedState;
    ui->play_button->setVisible(true);
    ui->pause_botton->setVisible(false);
    mediaPlayer->pause();
}
void MainWindow::play(){
    m_playerState = QMediaPlayer::PlayingState;
    ui->play_button->setVisible(false);
    ui->pause_botton->setVisible(true);
    qDebug()<<"开始播放";
    mediaPlayer->play();
}

void MainWindow::changeVideo(bool nextOrPrevious){
    /*
     * 可能的情况
     * 1. 播放列表什么也没有
     *  1. 没有加载视频
     *  2. 有加载视频=> 跳转到开始
     * 2. 播放列表有东西
     *  1. 没有加载视频
     *  2. 正在播的东西不在播放列表中
     *  3. 正在播的东西在列表中
    */
    if(!loadedVideo){ // solution for 1.1 & 2.1
        qDebug()<<"changeVideo:尚未加载视频";
        return;
    }
    // 若当前播放列表没有视频, 但有加载 (solution for 1.2)
    int numVideos = ui->playList->count();
    if(numVideos==0){
        // 此时下一个视频和上一个视频都是自己, 直接跳到开头
        mediaPlayer->setPosition(10);
        if(m_playerState==QMediaPlayer::PausedState)play();
        return;
    }
    int currentVideoIndex = -1;
    QString nextPath = "";
    QString previousPath = "";
    for(int i = 0;i<numVideos;i++){
        QString i_address = ui->playList->item(i)->data(Qt::UserRole).toString();
        if(currentVideoPath==i_address){
            currentVideoIndex = i;
            break;
        }
    }
    if(currentVideoIndex==-1){
        // 说明视频不在播放列表,
        // previousPath取播放记录的上一个视频
        // nextPath取播放列表的第一个视频
        nextPath = ui->playList->item(0)->data(Qt::UserRole).toString();
        if(playHistory->count()-2<0){
            // 在此之前没有播放过视频, prev此时不知道选什么, 那就播放列表第一个了吧
            previousPath = nextPath;
        }
        else{
            previousPath = (*playHistory)[playHistory->count()-2];
        }
    }
    else{
        // 说明视频在播放列表
        int previousIndex = currentVideoIndex-1;
        int nextIndex = currentVideoIndex+1;
        if(previousIndex<0){
            // 处于第一个, previous 是最后一个
            previousIndex = numVideos-1;
        }
        if (nextIndex>=numVideos){
            // 说明是最后一个, next 是最第一个
            nextIndex = 0;
        }
        previousPath = ui->playList->item(previousIndex)->data(Qt::UserRole).toString();
        nextPath = ui->playList->item(nextIndex)->data(Qt::UserRole).toString();
    }

    qDebug()<<"next"<<nextPath;
    qDebug()<<"previous"<<previousPath;
    if(nextOrPrevious){
        // 下一个视频(默认点击 上/下_一个视频的时候, 选择正放)
//        initVideo(nextPath,true,false);
        initializeVideo(nextPath);
        normalPlay();
    }
    else{
        //前一个视频
//        initVideo(previousPath,true,false);
        initializeVideo(previousPath);
        normalPlay();
    }
}

//void MainWindow::mousePressEvent(QMouseEvent *event) {
//    m_nMouseClick_X_Coordinate = event->x();
//    m_nMouseClick_Y_Coordinate = event->y();
//}

//void MainWindow::mouseMoveEvent(QMouseEvent *event) {
//    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
//}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->m_drag = true;
        this->dragPos = event->pos();
        this->resizeDownPos = event->globalPos();
        this->mouseDownRect = this->rect();
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    if (resizeRegion != Default)
    {
        handleResize();
        return;
    }
    if(m_move) {
        move(event->globalPos() - dragPos);
        return;
    }
    QPoint clientCursorPos = event->pos();
    QRect r = this->rect();
    QRect resizeInnerRect(resizeBorderWidth, resizeBorderWidth, r.width() - 2*resizeBorderWidth, r.height() - 2*resizeBorderWidth);
    if(r.contains(clientCursorPos) && !resizeInnerRect.contains(clientCursorPos)) { //调整窗体大小
        ResizeRegion resizeReg = getResizeRegion(clientCursorPos);
        setResizeCursor(resizeReg);
        if (m_drag && (event->buttons() & Qt::LeftButton)) {
            resizeRegion = resizeReg;
            handleResize();
        }
    }
    else { //移动窗体
        setCursor(Qt::ArrowCursor);
        if (m_drag && (event->buttons() & Qt::LeftButton)) {
            m_move = true;
            move(event->globalPos() - dragPos);
        }
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_drag = false;
    if(m_move) {
        m_move = false;
//        handleMove(event->globalPos()); //鼠标放开后若超出屏幕区域自动吸附于屏幕顶部/左侧/右侧
    }
    resizeRegion = Default;
    setCursor(Qt::ArrowCursor);
}
void MainWindow::setResizeCursor(ResizeRegion region)
{
    switch (region)
    {
    case North:
    case South:
        setCursor(Qt::SizeVerCursor);
        break;
    case East:
    case West:
        setCursor(Qt::SizeHorCursor);
        break;
    case NorthWest:
    case SouthEast:
        setCursor(Qt::SizeFDiagCursor);
        break;
    default:
        setCursor(Qt::SizeBDiagCursor);
        break;
    }
}
ResizeRegion MainWindow::getResizeRegion(QPoint clientPos)
{
    if (clientPos.y() <= resizeBorderWidth) {
        if (clientPos.x() <= resizeBorderWidth)
            return NorthWest;
        else if (clientPos.x() >= this->width() - resizeBorderWidth)
            return NorthEast;
        else
            return North;
    }
    else if (clientPos.y() >= this->height() - resizeBorderWidth) {
        if (clientPos.x() <= resizeBorderWidth)
            return SouthWest;
        else if (clientPos.x() >= this->width() - resizeBorderWidth)
            return SouthEast;
        else
            return South;
    }
    else {
        if (clientPos.x() <= resizeBorderWidth)
            return West;
        else
            return East;
    }
}
//void MainWindow::handleMove(QPoint pt)
//{
//    QPoint currentPos = pt - dragPos;
//    if(currentPos.x()<desktop->x()) { //吸附于屏幕左侧
//        currentPos.setX(desktop->x());
//    }
//    else if (currentPos.x()+this->width()>desktop->width()) { //吸附于屏幕右侧
//        currentPos.setX(desktop->width()-this->width());
//    }
//    if(currentPos.y()<desktop->y()) { //吸附于屏幕顶部
//        currentPos.setY(desktop->y());
//    }
//    move(currentPos);
//}
void MainWindow::handleResize()
{
    int xdiff = QCursor::pos().x() - resizeDownPos.x();
    int ydiff = QCursor::pos().y() - resizeDownPos.y();
    qDebug()<<"xdiff"<<xdiff;
    qDebug()<<"ydiff"<<ydiff;
    switch (resizeRegion)
    {
    case East:
    {
        resize(mouseDownRect.width()+xdiff, this->height());
        break;
    }
    case West:
    {
        resize(mouseDownRect.width()-xdiff, this->height());
        move(resizeDownPos.x()+xdiff, this->y());
        break;
    }
    case South:
    {
        resize(this->width(),mouseDownRect.height()+ydiff);
        break;
    }
    case North:
    {
        resize(this->width(),mouseDownRect.height()-ydiff);
        move(this->x(), resizeDownPos.y()+ydiff);
        break;
    }
    case SouthEast:
    {
        resize(mouseDownRect.width() + xdiff, mouseDownRect.height() + ydiff);
        break;
    }
    case NorthEast:
    {
        resize(mouseDownRect.width()+xdiff, mouseDownRect.height()-ydiff);
        move(this->x(), resizeDownPos.y()+ydiff);
        break;
    }
    case NorthWest:
    {
        resize(mouseDownRect.width()-xdiff, mouseDownRect.height()-ydiff);
        move(resizeDownPos.x()+xdiff, resizeDownPos.y()+ydiff);
        break;
    }
    case SouthWest:
    {
        resize(mouseDownRect.width()-xdiff, mouseDownRect.height()+ydiff);
        move(resizeDownPos.x()+xdiff, this->y());
        break;
    }
    }
}


//void MainWindow::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//    painter.setBrush(QBrush(Qt::red));
//    painter.setPen(Qt::transparent);
//    QRect rect = this->rect();
//    rect.setWidth(rect.width() - 1);
//    rect.setHeight(rect.height() - 1);
//    painter.drawRoundedRect(rect, 15, 15);
//    //也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);
//    {
//        QPainterPath painterPath;
//        painterPath.addRoundedRect(rect, 15, 15);
//        painter.drawPath(painterPath);
//    }
//    QWidget::paintEvent(event);
//}

void MainWindow::reversePlay(QString fileName){
    /*切换显示的组件从正方的widget到reverse_widget*/
//    initVideo(fileName,false,true);
    if(!isReverse)mediaPlayer->stop();
    currentVideoPath = fileName;
    highlightInFileList();// 在播放列表中高亮
    initializeVideo(fileName); // 初始化

    qDebug()<<"reversePlay1";
    if(isReverse){
        qDebug()<<"reversePlay2";
        if(m_playerState==QMediaPlayer::StoppedState){
            qDebug()<<"当前在暂停";
            /*此时倒放在暂停,要先解锁*/
            reverseDisplayer->resumeThread();
        }
        // 如果当前正在倒放, 现在要转到下一个视频倒放, 则先清空线程
        qDebug()<<"reversePlay3";
        quitCurrentReversePlay();
    }
    qDebug()<<"reversePlay4";
    // 处理倒放视频切换
    int res;
    if(reverseDecoder == NULL && reverseDisplayer == NULL){
        qDebug()<<"reverseDecoder 和 reverseDisplayer 为空";
        reverseDisplayer = new ReverseDisplay(ctrl,ui->reverse_widget);
        reverseDecoder = new ReverseDecode(ctrl);
    }
    // 连接发送帧的信号和渲染帧的槽函数
    connect(reverseDisplayer, SIGNAL(SendOneFrame(QImage)), ui->reverse_widget, SLOT(slotSetOneFrame(QImage)));
    // 连接更新进度条的槽函数
    connect(reverseDisplayer, SIGNAL(SendTime(qint64)), this, SLOT(reverseShowRatio(qint64)));
    // 连接 更新当前秒数的槽函数
    connect(reverseDisplayer, SIGNAL(SendSecond(qint64)), this, SLOT(recieveReverseSecond(qint64)));
    res = reverseDecoder->loadFile(fileName);
    if(res==0){
        qDebug()<<"初始化成功";
    }else{qDebug()<<"初始化失败";}

    duration = reverseDecoder->duration; // 这个其实是最大的pts
    reverseDurationSecond = (qint64)duration*av_q2d(reverseDecoder->format_ctx->streams[reverseDecoder->video_stream_index]->time_base);
    lastSecond = reverseDurationSecond;
    ui->video_slider->setMaximum(duration);  // 设置进度条的最大值
    mediaPlayer->stop(); // 停止正放
    reverseDecoder->start();
    reverseDisplayer->start();
    isReverse = true; // 设置倒放标志
    loadedVideo = true; // 设置加载视频标志
    m_playerState = QMediaPlayer::PlayingState; // 设置播放中的标志
    // 设置播放按钮样式
    ui->play_button->setVisible(false);
    ui->pause_botton->setVisible(true);
    // 设置渲染窗口
    showReverseWidget();

}

void MainWindow::quitCurrentReversePlay(){
    // 首先使用seek信号诱导渲染线程退出
    ctrl->seekMutex.lock();
    ctrl->isDisplayerSeek=true;
    ctrl->seekMutex.unlock();
    // 设置isQuit标志, 让 decoder 退出
    ctrl->quitMutex.lock();
    ctrl->isQuit=true;
    ctrl->quitMutex.unlock();
    // Qthread设置退出
    reverseDecoder->exit(0);
    reverseDisplayer->exit(0);
    reverseDecoder->deleteLater();
    reverseDisplayer->deleteLater();
    reverseDecoder->wait();
    reverseDisplayer->wait();
    // 断开与当前变量的所有连接
    disconnect(reverseDisplayer, nullptr, ui->reverse_widget, nullptr);
    disconnect(reverseDisplayer, nullptr, this, nullptr);
    // 清除堆栈空间
    delete reverseDecoder;
    delete reverseDisplayer;
    // 设置2个线程对象为空
    reverseDecoder = NULL;
    reverseDisplayer = NULL;
    // 倒放控制变量重设
    ctrl->init();
    ctrl->print();
    // 设置倒放标志位
    isReverse = false;
    // 设置倒放的显示组件不可见, 正放的可见
    showNormalWidget();
}

void MainWindow::reverseShowRatio(qint64 pts){
    /*用于更新进度条*/
    ui->video_slider->setValue(int(pts));

//    qDebug()<<"MainWindow::reverseShowRatio: reverseDurationSecond=>"<<reverseDurationSecond;
//    qDebug()<<"MainWindow::reverseShowRatio: time=>"<<time;
//    emit sig_reverseProgress(time);
}

void MainWindow::reversePause(){
    if(isReverse){

        if(reverseDisplayer->pause_==false){
            // 暂停
            ui->play_button->setVisible(true);
            ui->pause_botton->setVisible(false);
            reverseDisplayer->pauseThread();
            m_playerState = QMediaPlayer::StoppedState;
        }
        else{
            // 恢复
            ui->play_button->setVisible(false);
            ui->pause_botton->setVisible(true);
            reverseDisplayer->resumeThread();
            m_playerState = QMediaPlayer::PlayingState;
        }
    }
}

void MainWindow::reverseSeek(qint64 seekPos){
    if(isReverse){
        lastSecond = reverseDurationSecond;
        ctrl->seekMutex.lock();
        ctrl->isDecoderSeek=true;
        ctrl->isDisplayerSeek=true;
        ctrl->seekPos = seekPos;
        ctrl->seekMutex.unlock();
    }
}


void MainWindow::recieveReverseSecond(qint64 second){
    if(second<lastSecond){
        lastSecond = second;
        emit sig_reverseProgress(second);
    }
//    qDebug()<<"MainWindow::recieveReverseSecond second="<<second;
//    emit sig_reverseProgress(second);
}


void MainWindow::initSeekFrame(QString fileName){
    // 初始化seekFrame 类
    currVideoSeekFrame = new SeekFrame(fileName,1000,9999);
    durationForSeekFrame_S = currVideoSeekFrame->formatContext->duration/1000000; // 秒(double)
}

void MainWindow::deleteSeekFrame(){
    if(this->currVideoSeekFrame!=NULL){
        // 释放内存
        delete currVideoSeekFrame;
        currVideoSeekFrame = NULL;
    }
}

void MainWindow::initWaveForm(QString fileName){
    // 初始化 currAudioFrame
    currAudioGetFrame = new GetAudio(fileName);
}

void MainWindow::deleteWaveForm(){
    if(this->currAudioGetFrame!=NULL){
        delete  currAudioGetFrame;
        currAudioGetFrame = NULL;
    }
}

