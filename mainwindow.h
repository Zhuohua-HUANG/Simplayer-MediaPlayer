#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPropertyAnimation>
#include <QVideoWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void test();
    void test1(double);
    void test2(double);
private:
    Ui::MainWindow *ui;
    QPropertyAnimation *mAnimation_ControlWidget;   //控制底部控制控件的出现和隐藏
    void showOutControlWidget(); //显示底部控制控件
    void hideControlWidget();
};
#endif // MAINWINDOW_H
