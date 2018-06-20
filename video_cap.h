// coded by Luo Junren
// 2018.05.24
#ifndef VIDEO_CAP
#define VIDEO_CAP

#include<opencv2/opencv.hpp>
#include<opencv/cv.h>
#include<stdio.h>
#include <iostream>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include"robot.h"
#include <pthread.h>
using namespace cv;
using namespace std;

class VideoCap
{
public:
    VideoCap();
    ~VideoCap();

    void init();//ROBOT rb);
    void StartThread();
    void StopThread();
    void StartRun();
//    void VIDEOPROCESS();
    //ROBOT robot_v;
protected:
    pthread_mutex_t m_Mutexvideocap;
};
#endif // VIDEO_CAP

