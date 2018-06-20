#include"robot.h"
ROBOT::ROBOT()
{
}
ROBOT::~ROBOT()
{
}
void * stopMessage(void *arg)
{
    ROBOT *stop_= (ROBOT *)arg;
    stop_->StartRun();
}
void ROBOT::init(const char*dev, int baudrate) //初始化
{
    serial.Initialize(dev, baudrate);
    pthread_mutex_init(&Mutex_robot,NULL);
    pthread_mutex_init(&Mutex_recv,NULL);
    pthread_mutex_init(&Mutex_video,NULL);
    send.init(serial);
    recv.init(serial);
   // videocap.init();//初始化
    mod_old=0;
    av_old=0;
    bv_old=0;
    us=0;
    isHeadingDone=1;
    isSend=true;
}
void ROBOT::runAsync(bool a) //启动机器人线程
{
    if (a==true)
    {
        recv.StartThread();
        send.StartThread();
     //   videocap.StartThread();
        StartThread();
    }
    else
    {
        recv.StopThread();
        send.StopThread();
     //   videocap.StopThread();
    }
}

void ROBOT::setDeltaHeading(double theta) //设置转多少度
{
    pthread_mutex_lock(&Mutex_robot);
    send.av_s=theta;
    send.sendmod_s=2;
    send.bv_s=0;
    pthread_mutex_unlock(&Mutex_robot);
    if(isSend==true)
    {
        isHeadingDone=0;
        send.setMessage();
        isSend=false;
    }
}

void ROBOT::setVel(double v) //设置速度
{
    pthread_mutex_lock(&Mutex_robot);
    send.sendmod_s=1;
    send.av_s=(float)v;
    send.bv_s=(float)v;
    mod_old=1;
    av_old=(float)v;
    bv_old=(float)v;
    pthread_mutex_unlock(&Mutex_robot);
    if(isSend==true)
        send.setMessage();
}
void ROBOT::setVel2(double l_v,double r_v) //分别设置左右轮速度
{
    pthread_mutex_lock(&Mutex_robot);
    send.sendmod_s=1;
    send.av_s=(float)l_v;
    send.bv_s=(float)r_v;
    mod_old=1;
    av_old=(float)l_v;
    bv_old=(float)r_v;
    pthread_mutex_unlock(&Mutex_robot);
   if(isSend==true)
    send.setMessage();
}
double ROBOT::getVel()//获取机器人速度
{
    recv.getMessage();
    return (double)((recv.aspeed_s+recv.bspeed_s)/2.0);
}
double ROBOT::getLeftVel()//获取左轮速度
{
    recv.getMessage();
    return (double)recv.aspeed_s;
}
double ROBOT::getRightVel()//获取右轮的速度
{
    recv.getMessage();
    return (double)recv.bspeed_s;
}
void ROBOT::disconnect() //断开连接
{
    send.sendmod_s=0;
    send.av_s=0;
    send.bv_s=0;
    recv.StopThread();
    send.StopThread();
}
void ROBOT::stop()
{
    pthread_mutex_lock(&Mutex_robot);
    send.sendmod_s=0;
    send.av_s=0;
    send.bv_s=0;
    mod_old=0;
    av_old=0;
    bv_old=0;
    pthread_mutex_unlock(&Mutex_robot);
    send.setMessage();
}
double ROBOT::getTh()
{
    recv.getMessage();
    return (double)recv.theta_s;
}
double ROBOT::getX()
{
    recv.getMessage();
    return (double)recv.x_s;
}
double ROBOT::getY()
{
    recv.getMessage();
    return (double)recv.y_s;
}
pose ROBOT::getPose()
{
    recv.getMessage();
    P.x=recv.x_s;
    P.y=recv.y_s;
    P.ang=recv.theta_s;
    return P;
}
void ROBOT::StartThread()
{
    pthread_t a_thread;
    int res =pthread_create(&a_thread,NULL,stopMessage,this);
    if(res !=0)
    {
        printf("receiving thread create failed!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }
}
void ROBOT::StopThread()
{

}
void ROBOT::StartRun()
{
    while(1)
    {
        if(send.sendmod_s==2)
        {
            usleep(200000);
            if(recv.isD==1)
            {
                isHeadingDone=1;
                isSend=true;
                send.sendmod_s=mod_old;
                send.av_s=av_old;
                send.bv_s=bv_old;
                pthread_mutex_unlock(&Mutex_robot);
                send.setMessage();
                cout<<"isH:"<<isHeadingDone<<endl;
            }
            if(recv.isD==0)
            {
                isHeadingDone=0;
                isSend=false;
            }
        }
        usleep(50000);
    }
}
