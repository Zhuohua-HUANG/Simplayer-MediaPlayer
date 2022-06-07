#include <QtTest>
#include <QCoreApplication>
#include "audioimage.h"
#include "getaudio.h"
#include "seekFrame.h"
#include "utils.h"
#include "reversedecode.h"
using namespace std;
// add necessary includes here

class SimplayerTest : public QObject
{
    Q_OBJECT

public:
    SimplayerTest();
    ~SimplayerTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
    void test_case2();
    void test_case3();
    void test_case4();
    void test_case5();

private:
    GetAudio* currAudioGetFrame;
    SeekFrame* currVideoSeekFrame;
    QString fileName;
    ReverseDecode *reverseDecoder;
};

SimplayerTest::SimplayerTest()
{
    currAudioGetFrame =NULL;
    currVideoSeekFrame=NULL;
    reverseDecoder = NULL;
    fileName="D:/Video/up.mp4";
}

SimplayerTest::~SimplayerTest()
{
    if(this->currAudioGetFrame!=NULL){
        delete  currAudioGetFrame;
        currAudioGetFrame = NULL;
    }
    if(this->currVideoSeekFrame!=NULL){
        // 释放内存
        delete currVideoSeekFrame;
        currVideoSeekFrame = NULL;
    }
    reverseDecoder->exit(0);
    reverseDecoder->deleteLater();
    reverseDecoder->wait();
    delete reverseDecoder;
    reverseDecoder = NULL;
}

void SimplayerTest::initTestCase()
{
    currAudioGetFrame = new GetAudio(fileName);
    currVideoSeekFrame=new SeekFrame(fileName,1000,9999);
    reverseDecoder = new ReverseDecode(NULL);
}

void SimplayerTest::cleanupTestCase()
{

}

void SimplayerTest::test_case1()
{
    // test audio info
    QVERIFY((currAudioGetFrame->total_len)==56963072);
    QVERIFY((currAudioGetFrame->duration)==322);

    // test loadAudio
    QVERIFY((currAudioGetFrame->loadAudio(200))==35380400);
    QVERIFY((currAudioGetFrame->loadAudio(0))==0);
    for( int i=0;i<currAudioGetFrame->duration;i+=10){
       QVERIFY((currAudioGetFrame->loadAudio(i))<=currAudioGetFrame->total_len
               && (currAudioGetFrame->loadAudio(i))>=0 );
    }
}

void SimplayerTest::test_case2()
{
    // test video info
    QVERIFY((currVideoSeekFrame->video_width)==640);
    QVERIFY((currVideoSeekFrame->video_height)==360);
    QVERIFY((currVideoSeekFrame->formatContext)!=NULL);
    QVERIFY((currVideoSeekFrame->codecContext)!=NULL);

    // test getFrame
    QVERIFY((currVideoSeekFrame->getFrame(200))!=NULL);
    QVERIFY((currVideoSeekFrame->getFrame(0))!=NULL);
    for( int i=0;i<currAudioGetFrame->duration;i+=10){
           QVERIFY((currVideoSeekFrame->getFrame(i))!=NULL);
    }

}

void SimplayerTest::test_case3()
{
    // test get info
    QVERIFY((getVideoInfo(fileName))==" major_brand: mp42\n minor_version: 0\n"
                                      " compatible_brands: isommp42\n encoder: Google\n"
                                      " 格式: mov,mp4,m4a,3gp,3g2,mj2\n 时长（s）: 322\n"
                                      " 码率: 675017\n 流数: 2\n fps探测尺寸: -1\n"
                                      " 错误检测系数: 1\n 格式探索分数: 100\n "
                                      "视频编码器: H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10\n"
                                      " 视频宽度: 640\n 视频高度: 360\n 像素格式: 0\n"
                                      " 音频编码器: AAC (Advanced Audio Coding)\n 音道数: 2\n"
                                      " 音频采样率: 44100\n 音频码率: 95999\n");
}

void SimplayerTest::test_case4()
{
    // test get info
    QVERIFY((reverseDecoder->loadFile(fileName))==0);
}

void SimplayerTest::test_case5(){
    //utils test

    //valid file name test
    QVERIFY((isValidVideoFile("3d.mp3"))==true);
    QVERIFY((isValidVideoFile("3d.avi"))==true);
    QVERIFY((isValidVideoFile("3d.wmv"))==true);
    QVERIFY((isValidVideoFile("3d.mpg"))==true);
    QVERIFY((isValidVideoFile("3d.mpeg"))==true);
    QVERIFY((isValidVideoFile("3d.mov"))==true);
    QVERIFY((isValidVideoFile("3d.rm"))==true);
    QVERIFY((isValidVideoFile("avi.txt.avi"))==true);

    QVERIFY((isValidVideoFile("3d.md"))==false);
    QVERIFY((isValidVideoFile("3d.txt"))==false);
    QVERIFY((isValidVideoFile("3d.avi.md"))==false);

    //filename
    QVERIFY((getFileName("C:/abusrd/sdf/abc.mp3"))=="abc.mp3");
    QVERIFY((getFileName("C:\\abusrd\\sdf\\abc.mp3"))=="abc.mp3");

    // playlist
    QVector<QString>* playList=
            readPlayList("C:/Users/hzh30/Desktop/my_new/master/Simplayer/SimplayerTest/playList.dat");
    QVERIFY((playList[0][0])=="C:/Users/24508/Videos/Serrini - 油尖旺金毛玲.mp3");
    QVERIFY((playList[0][3])=="D:/Video/up.mp4");

    //attached pic
    QVERIFY(getAttachedPic("D:/Video/MONEY-LISA.mp3").size().width()==1620);
    QVERIFY(getAttachedPic("D:/Video/MONEY-LISA.mp3").size().height()==1028);

    // get suffix
    QVERIFY(getSuffix("MONEY-LISA.mp3")=="mp3");
    QVERIFY(getSuffix("2341.txt")=="txt");

    // mediaType test
    QVERIFY(mediaType("MONEY-LISA.mp3")==1);
    QVERIFY(mediaType("MONEY-LISA.wav")==1);
    QVERIFY((mediaType("3d.wmv"))==2);
    QVERIFY((mediaType("avi.txt.avi"))==2);
    QVERIFY((mediaType("3d.mp4"))==2);
    QVERIFY((mediaType("avi.txt.mkv"))==2);
    QVERIFY(mediaType("2341.txt")==0);
    QVERIFY(mediaType("2341.md")==0);
}

QTEST_MAIN(SimplayerTest)

#include "tst_simplayertest.moc"
