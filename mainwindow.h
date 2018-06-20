#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<opencv2/opencv.hpp>
#include<opencv/cv.h>
#include<stdio.h>
#include <iostream>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
//for stick socket
#include <QtNetwork>//use the socket
#include <QMessageBox>//for return mistake
//#include <math.h>
#include "dirent.h"
#include <sys/termios.h>
#include<string.h>
//#include <QTime>
#include "robot.h"

using namespace cv;
using namespace std;

//define struct for stick
struct Actual_JoyStick_CrlData//this data is used for controling
{
    Actual_JoyStick_CrlData() {}
    float Wheel;//wheel
    int Acc;//acc
    int brake;//brake
    int clutch;//seperation and reunion
    bool buttom_up;
    bool buttom_down;
    bool buttom_left;
    bool buttom_right;
    int shiftgears;//forward:1  Null: 0    backward:-1
};
//for distribute the frame
#define UDP_MAX_SIZE 1200
struct PackageHeader
{
    //包头大小(sizeof(PackageHeader))
    unsigned int uTransPackageHdrSize;
    //当前包头的大小(sizeof(PackageHeader)+当前数据包长度)
    unsigned int uTransPackageSize;
    //数据的总大小
    unsigned int uDataSize;
    //数据被分成包的个数
    unsigned int uDataPackageNum;
    //数据包当前的帧号
    unsigned int uDataPackageCurrIndex;
    //数据包在整个数据中的偏移
    unsigned int uDataPackageOffset;
};
//difine end


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    VideoCapture capture;
    Actual_JoyStick_CrlData *CtlDara;
    QTimer *timer;//the main function to send Image back to PC
    QUdpSocket *ImgSender;
    QUdpSocket *CtlRecever;
    Mat frame;
    ROBOT tiger;

private slots:
    void on_Start_clicked(void);
    void OntimeRun(void);
    void CtlAction(void);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
