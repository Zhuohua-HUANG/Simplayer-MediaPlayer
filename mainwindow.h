#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPropertyAnimation>
#include <QVideoWidget>
#include "Widget/seekFrame.h"
#include "reversedecode.h"
#include "reversedisplay.h"
#include <QLabel>
#include <QMediaPlayer>
#include "videoinfo.h"
#include <QListWidgetItem>
#include <QContextMenuEvent>  //用于产生右键事件
#include <QMenu>    //用于生成右键菜单
#include <QAction>  //用于添加菜单栏动作
#include <QCursor>  //用于获取当前光标位置
#include <QProcess> //用于启动记事本
#include "audioimage.h"
#include "getaudio.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
enum ResizeRegion
{
    Default,
    North,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest
};
class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void sig_reversePlay(QString);
    void sig_reverseProgress(qint64);

public:
    double durationForSeekFrame_S; // 秒
    SeekFrame* currVideoSeekFrame;
    GetAudio* currAudioGetFrame;
    QMediaPlayer * mediaPlayer; // 播放器
    QMediaPlaylist *playList;
    videoInfo* video = NULL;
    // 初始化当前的 video
    void initVideo(QString filename, bool isPlay, bool reverse); // 初始化一个播放视频
    // 正放视频
    void initializeVideo(QString);
    void normalPlay();
    void initSystem(); // 用于初始化一些类
    int volume() const;
    qint64 currentPosition=0; // 记录当前的播放位置
    bool m_playerMuted = true;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

/*无边框相关*/
public:
    void handleResize();
    void handleMove(QPoint pt);
    ResizeRegion getResizeRegion(QPoint clientPos);
    void setResizeCursor(ResizeRegion region);
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent * event)override;
    void mousePressEvent(QMouseEvent *event)override;
private:
    AudioImage * pAudioImage;
public slots:

    /*其它*/
    void connect2Player();  // 用于进行所有的链接
    void highlightInFileList(); // 高亮播放列表中的某个视频
    void showNormalWidget();
    void showReverseWidget();

    /*波形图相关*/
    void showOscillograph(double);
    void initWaveForm(QString); //初始化currAudioFrame类
    void deleteWaveForm(); // 释放当前currAudioFrame对象的空间

    /*缩略图相关*/
    void initSeekFrame(QString); //初始化seekFrame类
    void deleteSeekFrame(); // 释放当前SeekFrame对象的空间
    void showScreenCap(double); // 用于显示缩略图

    /*seek相关*/
    void seek(qint64);   // 正放的时候, seek到某个位置

    /*音量相关*/
    void changeVolume(int);  //改变音量
    void changeMute();  // 静音

    /*以下两个后续可以考虑合并在一起*/
    void pause(); // 暂停
    void play();  // 继续播放
    void playClicked();  // 播放按钮的点击事件


    /*视频向前或向后跳10s 相关*/
    void skipForwardOrBackward(bool); // 判断是往前skip还是往后skip
    void jump(int); // 进行视频的前后10s的跳转

    /*时间显示相关*/
    void positionChange(qint64 progress);

    /*播放列表相关*/
    void playlistPositionChanged(int); //* 暂时未用到此函数
    void openFileButtonClicked(); // 处理打开一个新文件的按钮
    void addVideoItem(QString); // 添加某个url到播放列表
    void changeVideo(bool); // 上一首,下一首


    /*倒放相关*/
    void reversePlay(QString);
    void reversePause();
    void reverseSeek(qint64);
    void recieveReverseSecond(qint64);
    void reverseUpdateDurationInfo(qint64);

    // 测试使用
    void test();
    void test1(bool);
    void reverseShowRatio(qint64);
public:
    void quitCurrentReversePlay();
private:
    Ui::MainWindow *ui;
    // 当前的播放状态
    QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
    QLabel* frameLabel=NULL;
    QPropertyAnimation *mAnimation_ControlWidget;
    //显示底部控制控件(现在没有用,后期可能有用,先不要管)
    void showOutControlWidget();
    void hideControlWidget();
    void updateDurationInfo(qint64 currentInfo);

public:
    // 初始化组件
    void initWdigets();
public:
    // 使用QT初始化视频
    void initVideoInfo(QString);
    void createExtraWidget();
public:
    // 标志位
    bool loadedVideo;
    bool isReverse; // 标志是否在倒放
    int currMediaType;
    // 当前正在播放的视频的地址
    QString currentVideoPath;

public:
    // 其它
    QVector<QString> *playHistory;
    QVector<QString> *playListLocal;
//    void paintEvent(QPaintEvent *event);
    void changePlayingRatio(float);

private:
    /*************无边框需要用到的属性*************/
    bool m_drag, m_move;
    QPoint dragPos, resizeDownPos;
    const int resizeBorderWidth = 10;
    ResizeRegion resizeRegion;
    QRect mouseDownRect;
    /************************************/

/*倒放相关*/
public:
    qint64 duration;
    Controller *ctrl = NULL;
    ReverseDisplay *reverseDisplayer = NULL;
    ReverseDecode *reverseDecoder = NULL;
    qint64 reverseDurationSecond;
    volatile qint64 lastSecond = 1e10;
/*右键菜单栏(进行调整倍速)*/
public:
    QAction *rtText;
    QAction *rt0_5;
    QAction *rt0_75;
    QAction *rt1_0;
    QAction *rt1_25;
    QAction *rt1_5;
    QAction *rt2_0;
    QAction *rt3_0;
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

};
#endif // MAINWINDOW_H
