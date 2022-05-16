#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPropertyAnimation>
#include <QVideoWidget>
#include "Widget/seekFrame.h"
#include "Widget/getAudio.h"
#include <QLabel>
#include <QMediaPlayer>
#include "videoinfo.h"
#include <QListWidgetItem>

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

public:
    double currVideoDuration;
    SeekFrame currVideoSeekFrame;
    GetAudio currGetAudioFrame;
    QMediaPlayer * mediaPlayer; // 播放器
    QMediaPlaylist *playList;
    videoInfo* video = NULL;
    // 初始化当前的 video
    void initVideo(QString filename, bool isPlay); // 初始化一个播放视频
    void initSystem();
    int volume() const;
    qint64 currentPosition=0;
//    bool isMuted() const; // 静音
    bool m_playerMuted = true;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
    void handleResize();
    void handleMove(QPoint pt);
    ResizeRegion getResizeRegion(QPoint clientPos);
    void setResizeCursor(ResizeRegion region);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent *event);

public slots:

    void connect2Player();
    void showScreenCap(double);
//    void setState(QMediaPlayer::State state);
    void playClicked();
    void seek(double);
    void changeVolume(int);
    void changeMute();
    void jump(int);
    void pause();
    void play();
    void skipForwardOrBackward(bool);
    void positionChange(qint64 progress);
    void playlistPositionChanged(int);
    void openFileButtonClicked();
    void addVideoItem(QString);
    int currentClickedIndex(QListWidgetItem* );
    void doubleClickItem();
    void changeVideo(bool);
    // 测试使用
    void test();
    void test1(bool);
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
    SeekFrame sf;

public:
    // 初始化组件
    void initWdigets();
public:
    // 使用QT初始化视频
    void initVideoInfo(const char*);
    void createExtraWidget();
public:
    // 标志位
    bool loadedVideo;
    // 当前正在播放的视频的地址
    QString currentVideoPath;

public:
    // 其它
    QVector<QString> *playHistory;
    QVector<QString> *playListLocal;
//    void paintEvent(QPaintEvent *event);


private:
    /*************无边框需要用到的属性*************/
    bool m_drag, m_move;
    QPoint dragPos, resizeDownPos;
    const int resizeBorderWidth = 10;
    ResizeRegion resizeRegion;
    QRect mouseDownRect;
    /************************************/

};
#endif // MAINWINDOW_H
