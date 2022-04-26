#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPropertyAnimation>
#include <QVideoWidget>
#include "Widget/seekFrame.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    double currVideoDuration;
    SeekFrame currVideoSeekFrame;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void test();
    void test1(double);
    void showScreenCap(double);
private:
    Ui::MainWindow *ui;
    //控制底部控制控件的出现和隐藏(现在没有用,后期可能有用,先不要管)
    QPropertyAnimation *mAnimation_ControlWidget;
    //显示底部控制控件(现在没有用,后期可能有用,先不要管)
    void showOutControlWidget();
    void hideControlWidget();
};
#endif // MAINWINDOW_H
