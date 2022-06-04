#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPropertyAnimation>
#include <QVideoWidget>
#include "seekFrame.h"
#include "reversedecode.h"
#include "reversedisplay.h"
#include <QLabel>
#include <QMediaPlayer>
#include <QListWidgetItem>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QProcess>
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


/*初始化相关*/
public:
    // 初始化视频
    void initializeVideo(QString);
    // 正放视频
    void normalPlay();
    // 用于初始化一些类 (缩略图, 波形图..)
    void initSystem();
    // 使用QT初始化视频
    void initVideoInfo(QString);

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

/*视频信息相关成员*/
public:
    // 视频长度
    int durationS; //(秒)
    qint64 duration; //(微秒)
    qint64 durationMs; //(毫秒)
    // 倒放组件读出来的视频的秒数
    qint64 reverseDurationSecond;
    // 当前正在播放的视频的地址
    QString currentVideoPath;
    volatile qint64 lastSecond = 1e10;
    qint64 currentPosition=0; // 记录当前的播放位置
    // 用于显示缩略图的QLabel
    QLabel* frameLabel=NULL;
    // 用于显示波形图
    AudioImage * pAudioImage;

    int volume() const;

/*播放功能组件相关*/
public:
    SeekFrame* currVideoSeekFrame; // 缩略图
    GetAudio* currAudioGetFrame; // 波形图
    QMediaPlayer * mediaPlayer; // 播放器
    QMediaPlaylist *playList; // 播放列表
    Controller *ctrl = NULL; // 倒放的控制器(控制解码和渲染2个线程)
    ReverseDisplay *reverseDisplayer = NULL; // 倒放渲染线程
    ReverseDecode *reverseDecoder = NULL; // 倒放解码线程
    void quitCurrentReversePlay();// 退出当前的倒放

/*标志位*/
public:
    // 标志是否静音
    bool m_playerMuted = true;
    // 标志是否已经加载了视频
    bool loadedVideo;
    // 标志是否在倒放
    bool isReverse;
    // 当前视频的类型
    int currMediaType;
    // 当前的播放状态
    QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;

/*无边框相关*/
public:
    void handleResize();
    void handleMove(QPoint pt);
    ResizeRegion getResizeRegion(QPoint clientPos);
    void setResizeCursor(ResizeRegion region);
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent * event)override;
    void mousePressEvent(QMouseEvent *event)override;
/*槽函数*/
public slots:

    /*其它*/
    void connect2Player();  // 用于进行所有的链接
    void highlightInFileList(); // 高亮播放列表中的某个视频
    void showNormalWidget();
    void showReverseWidget();

    /*波形图相关*/
    void initWaveForm(QString); //初始化currAudioFrame类
    void deleteWaveForm(); // 释放当前currAudioFrame对象的空间

    /*缩略图相关*/
    void initSeekFrame(QString); //初始化seekFrame类
    void deleteSeekFrame(); // 释放当前SeekFrame对象的空间
    void showScreenCap(double); // 用于显示缩略图

    /*正放跳转相关*/
    void seek(qint64);   // 正放的时候, seek到某个位置

    /*音量相关*/
    void changeVolume(int);  //改变音量
    void changeMute();  // 静音

    /*暂停和继续播放*/
    void pause(); // 暂停
    void play();  // 继续播放
    void playClicked();  // 播放按钮的点击事件


    /*视频向前或向后跳10s 相关*/
    void reverseSkipForwardOrBackward(bool); // 倒放的跳秒
    void skipForwardOrBackward(bool); // 判断是往前skip还是往后skip
    void jump(int); // 进行视频的前后10s的跳转

    /*时间显示相关*/
    void positionChange(qint64 progress);
    void reverseShowRatio(qint64); //设置进度条比例

    /*播放列表相关*/
    void playlistPositionChanged(int); //* 暂时未用到此函数
    void openFileButtonClicked(); // 处理打开一个新文件的按钮
    void addVideoItem(QString); // 添加某个url到播放列表
    void changeVideo(bool); // 上一首,下一首


    /*倒放相关*/
    void reversePlay(QString);
    void reversePause();
    void reverseSeek(qint64);
    void recieveReverseSecond(double);
    void reverseUpdateDurationInfo(qint64);

    // 测试使用
    void test();
    void test1(bool);

private:
    Ui::MainWindow *ui;

    QPropertyAnimation *mAnimation_ControlWidget;
    //显示底部控制控件(现在没有用,后期可能有用,先不要管)
    void showOutControlWidget();
    void hideControlWidget();
    void updateDurationInfo(qint64 currentInfo);

/*Qt Widget相关*/
public:
    // 初始化组件
    void initWdigets();
    void createExtraWidget();


/*其它内容*/
public:
    QVector<QString> *playHistory;
    QVector<QString> *playListLocal;
    void changePlayingRatio(float);

private:
    /**********无边框需要用到的属性********/
    bool m_drag, m_move;
    QPoint dragPos, resizeDownPos;
    const int resizeBorderWidth = 10;
    ResizeRegion resizeRegion;
    QRect mouseDownRect;
    /***********************************/



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
