#include "audioimage.h"
# include"cmath"
using namespace std;
AudioImage::AudioImage(QWidget *parent,int width, int height)
    : QOpenGLWidget{parent}
{
    cout<<"in Initialize:"<<endl;
    cout<<"w:"<<width<<endl;
    cout<<"h:"<<height<<endl;
    pstartdata=new long;
    *pstartdata=200;
    w=width;
    h=height;
};
void AudioImage::initializeGL()
{
    cout<<"in Initialize GL:"<<endl;
    cout<<"w:"<<w<<endl;
    cout<<"h:"<<h<<endl;
    initializeOpenGLFunctions();
    glClearColor(1,1,1,0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
   // cout<<glGetString(GL_VERSION)<<endl;
}
void AudioImage::set_startdata(long data){
    *pstartdata=data;
    cout<<"in startdata"<<endl;
    cout<<*pstartdata<<endl;
}

void AudioImage::paintGL()
{

    cout<<"------------"<<endl;
    cout<<"paintGL start:"<<endl;
    if (h==30){
     cout<<"h:"<<30<<endl;
     cout<<"ime"<<endl;
    }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLineWidth(2);//设置线段宽度
        glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);
        short pcm_In;
        float xstart=float(-w/2);
        float ystart=0.0;
        float xend=float(-w/2);
        float yend=0.0;
        float temp = 0.0;
        float show_rate=200.0;
        float min_step=float(w/show_rate);
        cout<<"in my absurdsize"<<endl;
        cout<<w<<endl;
        cout<<h<<endl;


         //         cout<<xstart<<endl;
         //         cout<<xend<<endl;
         //         cout<<min_step<<endl;
         cout<<*pstartdata<<endl;

         fstream f;
         f.open("temp.pcm", ios::in|ios::binary);
         f.seekg(*pstartdata,ios::beg);
         long times=0;

         //        int show=30;
         while(!f.eof())
         {
             f.seekg(times*2,ios::cur);
             f.read((char*)&pcm_In, 2);     //pcm中每个数据大小为2字节，每次读取1个数据
//                             cout<<pcm_In<<endl;

             times+=1;
             // acustom to max_height change
             if (abs(pcm_In)>=max_height){
                 float rigin=10000.0;
                 max_height=abs(pcm_In)+rigin;
                 cout<<"max_height"<<max_height<<endl;
             }
             //确定绘制波形的折线两点坐标
             xstart=xstart+min_step;
             ystart=temp;
             xend=xstart+min_step;

             //-------------------------------------------------------------------------------------------------------------------------
             yend=(float)(pcm_In/max_height)*h/2;       //short类型除以一个数要么是0要么是大于一的整数，所以坐标变换要注意先转为float
 //            cout<<yend<<endl;

             //绘制折线
             if (xend>=w/2){
                 break;
             }

             glVertex2f(xstart,ystart);
             glVertex2f(xend,yend);
 //            glVertex2f(xstart,-show);
 //            glVertex2f(xend,show);
//                             cout<<xstart<<"   "<<xend<<endl;
//                             cout<< ystart<<"   "<<yend<<endl;

             temp=yend;    //终点作为下一次的起点

 //            if (times==show_rate/2){
 //                show=-30;
 //            }
             if (times==show_rate){
                 break;
             }
         }
         cout<<"time"<<times<<endl;
         f.close();

        glEnd();
        cout<<"paintGL end."<<endl;
}

void AudioImage::resizeGL(int width, int height)
{
    w=width;
    h=height;
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/2,width/2,-height/2,height/2,-1,1);
    glMatrixMode(GL_MODELVIEW);
}

AudioImage::~AudioImage(){
   delete pstartdata;

}
