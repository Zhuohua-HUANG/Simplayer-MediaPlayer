
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Widget/seekFrame.h"
#include "Widget/getAudio.h"
#include "utils.h"
#include <QDebug>
#include <QMediaPlayer>
#include <QTime>
#include <QMediaPlaylist>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>

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
//    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);
    initSystem();
    ui->widget->setStyleSheet("background-color:black;");
    mediaPlayer = new QMediaPlayer;
    mediaPlayer->setVideoOutput(ui->widget);

    playList = new QMediaPlaylist();
    mediaPlayer->setPlaylist(playList);
//    mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(fileName)));
    initWdigets();
    connect2Player();
    createExtraWidget();








    // ui:设置打开和关闭侧边栏
    connect(ui->openFileList,&QPushButton::clicked,this,[=](){

        bool playListVisable = ui->sideBlock->isVisible();
        if(playListVisable){
            ui->sideBlock->setVisible(false);
            this->resize(this->width()-ui->sideBlock->width(),this->height());
            ui->openFileList->setStyleSheet(
                        "image: url(:new/image/off.png);"
                        "border-radius:0px; "
            );
        }
        else{
            ui->sideBlock->setVisible(true);
            this->resize(this->width()+ui->sideBlock->width(),this->height());
            ui->openFileList->setStyleSheet(
                        "image: url(:new/image/on.png);"
                        "border-radius:0px; "
            );
        }
    });


    // volume slider 音量改变信号的demo
    qDebug()<<ui->volume_slider->maximumSize();

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

//void MainWindow::test1(int a){
//    // feel free to delete this function
//    qDebug()<<"鼠标点击的进度条百分比"<<a<<"\n";
//}


void MainWindow::showScreenCap(double ratio){
    if(loadedVideo){
        // 根据当前的比例来显示screencap
        // feel free to alter or delete this function
        int time = int(currVideoDuration*ratio);
        qDebug()<<"鼠标指向的进度条百分比"<<ratio<<"指向的秒数(int):"<<time;
        // 设置screencap可见
        frameLabel->setVisible(true);
        frameLabel->raise(); // 保证它在最前
        // 当前鼠标的位置
        int x=this->mapFromGlobal(QCursor().pos()).x();
        // screencap固定高度, 宽度按照比例缩放
        int img_height = 100;
        int img_width = img_height*currVideoSeekFrame.codecContext->width/currVideoSeekFrame.codecContext->height;
        // 处理screencap位置越界的情况
        if(x+img_width>=this->width()){
            x-=img_width;
        }
        frameLabel->setGeometry(x,this->ui->widget_controller->pos().y()-img_height-10,img_width,img_height);
        // 得到当前时间的frame
        AVFrame* pFrameRGB = currVideoSeekFrame.getFrame(time);
    //    AVFrame* pFrameRGB = currGetAudioFrame.loadAudio(time);
        // 将frame转化为 QImage对象
        QImage image = QImage(pFrameRGB->data[0], pFrameRGB->width, pFrameRGB->height,
                              pFrameRGB->linesize[0], QImage::Format_RGB888)
                             .copy();//可选加参数,Qt::IgnoreAspectRatio,Qt::SmoothTransformation
        // .scaled(img_width,img_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        // QImage渲染
        frameLabel->setPixmap(QPixmap::fromImage(image.scaled(img_width,img_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        av_frame_free(&pFrameRGB);
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

void MainWindow::playClicked(){
    if(!loadedVideo){
        qDebug()<<"尚未加载视频";
        return;
    }
    // 处理点击了播放-暂停按钮的处理
    qDebug()<<"'current satate:"<<m_playerState;
    switch (m_playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        play();
        break;
    case QMediaPlayer::PlayingState:
        pause();
        break;
    }
    qDebug()<<"'after satate:"<<m_playerState;
}

void MainWindow::connect2Player(){
    //关闭, 最大化, 最小化
    connect(ui->close,&QPushButton::clicked,this,[=](){ this->close();});
    connect(ui->minimize,&QPushButton::clicked,this,[=](){ this->setWindowState(Qt::WindowMinimized);});
    connect(ui->maximize,&QPushButton::clicked,this,[=](){ this->setWindowState(Qt::WindowMaximized);});
    // 连接鼠标离开视频播放条不显示screencap
    connect(ui->video_slider,&VideoSlider::sig_mouseLeave,this,[=](){
        frameLabel->setVisible(false);
    });
    connect(ui->play_button,SIGNAL(clicked()),this,SLOT(playClicked()));
    connect(ui->pause_botton,SIGNAL(clicked()),this,SLOT(playClicked()));
    connect(mediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
    connect(ui->video_slider,SIGNAL(sig_valueChanged(double)),this,SLOT(seek(double)));
    connect(ui->volume_slider,SIGNAL(valueChanged(int)),SLOT(changeVolume(int)));
    connect(ui->volume_button,SIGNAL(clicked()),this,SLOT(changeMute()));
    connect(ui->addFile,SIGNAL(clicked()),this,SLOT(openFileButtonClicked()));
    connect(playList, &QMediaPlaylist::currentIndexChanged, this, &MainWindow::playlistPositionChanged);
    // demo:
//    connect(ui->playList,SIGNAL(itemDoubleClicked()),this,SLOT(doubleClickItem()));
    connect(ui->playList,&QListWidget::itemDoubleClicked, this, [=](){
        qDebug()<<"双击啦!";
        QListWidgetItem * item = ui->playList->item(ui->playList->currentIndex().row());
        mediaPlayer->pause();
        initVideo(item->data(Qt::UserRole).toString(),true);
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
}

void MainWindow::createExtraWidget(){
    // 创建显示帧的QLabel 组件
    frameLabel = new QLabel(this);
    frameLabel->setGeometry(100,100,100,100);
    frameLabel->setVisible(false);

//    list->show();
//    ui->playList->addItem();

}

void MainWindow::positionChange(qint64 progress)
{
    qDebug()<<"current progress"<<progress;
    // 计算当前进度
    int curr = int(double(progress)/double(sf.formatContext->duration/1000)*100) ;
    qDebug()<<"curr"<<curr;
    if (!ui->video_slider->isSliderDown())
        ui->video_slider->setValue(curr);
    currentPosition = progress;
    updateDurationInfo(progress/1000);
}

void MainWindow::updateDurationInfo(qint64 currentInfo){

    qDebug()<<"currentInfo"<<currentInfo;
    qDebug()<<"m_duration"<<video->duration;

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

void MainWindow::initVideoInfo(const char* fileName){

    video->fileName = QString(fileName);
//    video->fileName = temp;
    video->width = sf.video_width;
    video->height = sf.video_width;
    video->durationMs = (qint64)(sf.formatContext->duration/1000);
    video->duration = (qint64)(sf.formatContext->duration/1000000);
}


void MainWindow::seek(double ratio){
    if(!loadedVideo){
        qDebug()<<"seek:尚未加载视频";
        return;
    }
    qint64 time = qint64((double)video->durationMs*ratio);
    qDebug()<<"seek time"<<time;
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
    // QList初始化
    QString shortFilename = getFileName(fileName);
    qDebug()<<"加入!";
//    QString fileName="xxxx.mp4";
    QWidget *w = new QWidget(ui->playList);
    QHBoxLayout *layout=new QHBoxLayout(w);
    QPushButton *pushButton=new QPushButton(w);
    pushButton->setStyleSheet(
                "QPushButton{ "
                "image: url(:new/image/delete.png);"
                "border-radius:0px; "
                "}  "
                "QPushButton:hover{ "
                "image: url(:new/image/delete_hover.png);"
                "border-radius:0px; "
                "} "
                "QPushButton:pressed{ "
                "image: url(:new/image/delete.png);"
                "border-radius:0px; "
                "}"
                );
    pushButton->setMinimumSize(30,30);
    QListWidgetItem *Qitem=new QListWidgetItem(ui->playList,0);
    Qitem->setSizeHint(QSize(120,40));
    QLabel *label = new QLabel(shortFilename);
//    label->setVisible(false);
    QFont font ( "Microsoft YaHei", 12, 50);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    label->setPalette(pa);
    label->setFont(font);
    label->setMinimumSize(80,30);
    layout->addStretch();
    layout->addWidget(label);
    layout->addWidget(pushButton);
    layout->addStretch();
    w->setLayout(layout);
    w->show();

    ui->playList->setItemWidget(Qitem,w);
    Qitem->setData(Qt::UserRole,fileName);
    // 在创建的时间就连接好这个删除键
    connect(pushButton,&QPushButton::clicked,this,[=](){
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
                                           tr("打开的文件不是音频格式")
                                           );
            return;
        }

        mediaPlayer->pause();
        addVideoItem(filename);
        playListLocal->append(filename); // 把文件名写入到本地
        initVideo(filename,true);
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

void MainWindow::initVideo(QString filename, bool isPlay){

    currentVideoPath = filename;
    std::string str = filename.toStdString();
    const char* fileName = str.c_str();
    video = new videoInfo();
    sf = SeekFrame(fileName,1000,9999); // 这里要考虑清除空间, 要再写一下


    // 初始化视频信息
    currVideoSeekFrame = sf;
    currVideoDuration = sf.formatContext->duration/1000000; // 初始化当前视频的时长
    video = new videoInfo();
    initVideoInfo(fileName);
    mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(fileName)));

    // 在播放列表中高亮
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


    if(isPlay){
        mediaPlayer->play();
        play(); // 播放按钮和状态更新
    }
    // 记录播放过的视频
    playHistory->append(filename);

    // 设置标记位
    if(!loadedVideo){
        loadedVideo = true;

    }


}

int MainWindow::currentClickedIndex(QListWidgetItem* item)
{

}

void MainWindow::doubleClickItem()
{
    qDebug()<<"正在双击!";

}

void MainWindow::initSystem(){
    loadedVideo = false;
    playHistory = new QVector<QString>;
    playListLocal = readPlayList();
    if(playListLocal->size()!=0){
        for (int i=0;i<playListLocal->size();i++) {
            addVideoItem((*playListLocal)[i]);
        }
    }

}

void MainWindow::initWdigets(){
    // 初始化播放按钮
    ui->pause_botton->setVisible(false); // 暂停按钮初始为不可见
    ui->sideBlock->setVisible(false); // 播放列表初始化为不可见





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
        // 下一个视频
        initVideo(nextPath,true);
    }
    else{
        //前一个视频
        initVideo(previousPath,true);
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
