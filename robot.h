#ifndef ROBOT_H
#define ROBOT_H
#include<iostream>
#include "robot.h"
#include"serial_recv.h"
#include"serial_send.h"
#include <pthread.h>
#include<stdlib.h>
#include<math.h>
//#include"video_cap.h"
//#include"robot_trj.h"
using namespace std;
struct pose
{
    double x;      //mm
    double y;      //mm
    double ang;    //rad
    pose()
    {
        x = y = ang = 0.0;
    }
};
class ROBOT
{
public:
    ROBOT();
    ~ROBOT();
    void init(const char*dev, int baudrate);
    void runAsync(bool a);
    void setDeltaHeading(double theta); //左转90度，右转为负
    void setVel(double v); //设置速度
    void setVel2(double l_v,double r_v); //分别设置左右轮速度
    double getVel(); //获取机器人速度
    double getLeftVel();
    double getRightVel();//获取两轮的速度
    double getX();     //X;
    double getY();     //Y;
    double getTh();   //获得X Y Theta;
    int isHeadingDone;
    pose getPose();
    void StartRun();
    void stop();
    void disconnect();
    int mod_old;
    float av_old;
    float bv_old;
    pose P;
    SerialAdapter serial;
    SE_RECV recv;
    SE_SEND send;
    bool isSend;
    //video_capture
 //   VideoCap videocap;
    //robot_trj
//    RobotTrj robottrj;

protected:
    pthread_mutex_t Mutex_robot;
    pthread_mutex_t Mutex_recv;
    pthread_mutex_t Mutex_video;
private:
    void StartThread();
    void StopThread();
    int us;

};
#endif // ROBOT_H


