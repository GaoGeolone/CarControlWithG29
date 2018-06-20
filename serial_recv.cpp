#include"serial_recv.h"

void * ReceivingserMessage(void *arg);
SE_RECV::SE_RECV()
{
}
SE_RECV::~SE_RECV()
{
}
unsigned short usRECVMBCRC16(u_char *pucFrame, unsigned short usLen)
{
    u_char ucCRCHi = 0xFF;
    u_char ucCRCLo = 0xFF;
    int iIndex;
    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = ( u_char )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( u_short )( ucCRCHi << 8 | ucCRCLo );
}

void SE_RECV::init(SerialAdapter seri)
{
    seri_recv=seri;
    recvmod=1;
    aspeed=0;
    bspeed=0;
    recvmod_s=1;
    aspeed_s=0;
    bspeed_s=0;
    x=0;
    y=0;
    theta=0;
    x_s=0;
    y_s=0;
    theta_s=0;
    isD=1;
    isD_s=1;
}
void SE_RECV::StartThread()
{
    pthread_mutex_init(&m_Mutexreceive,NULL);
    pthread_t a_thread;
    int res =pthread_create(&a_thread,NULL,ReceivingserMessage,this);
    if(res !=0)
    {
        printf("receiving thread create failed!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }
}
void SE_RECV::StartRun()
{
    DATA32 a;
    DATA32 b;
    DATA32 x_;
    DATA32 y_;
    DATA32 theta_;
    DATA16 CRCR;
    unsigned char rev_buf[1];
    unsigned char data_buf[24];
    int count;
    while(1)
    {
        int ret=seri_recv.Read(rev_buf,sizeof(rev_buf));
        if(ret==0)
        {
            count++;
            if(count==50)
            {
                count=0;
                recvmod=1;
                aspeed=0;
                bspeed=0;
            }
        }
        if(ret>0)
        {
            count=0;
            if(rev_buf[0]==0XAA)//帧头
            {
                data_buf[0]=rev_buf[0];
                for(int i=1;i<24;)
                {
                    ret=seri_recv.Read(rev_buf,sizeof(rev_buf));
                    if(ret>0)
                    {
                        data_buf[i]=rev_buf[0];
                        i++;
                    }
                }
            }
            CRCR.us=usRECVMBCRC16(data_buf,22);
            if((CRCR.uc[0]==data_buf[22])&&(CRCR.uc[1]==data_buf[23]))
            {
                recvmod=(int)data_buf[1];
                for(int i=0;i<4;i++)
                {
                    a.uc[i]=data_buf[i+2];
                    b.uc[i]=data_buf[i+6];
                    x_.uc[i]=data_buf[i+10];
                    y_.uc[i]=data_buf[i+14];
                    theta_.uc[i]=data_buf[i+18];
                }
                aspeed=a.f;
                bspeed=b.f;
                x=x_.f;
                y=y_.f;
                theta=theta_.f;
                isD=recvmod;
                //cout<<"isD:"<<recvmod<<endl;
              //  cout<<"mod:"<<recvmod<<".aspeed:"<<aspeed<<".bspeed:"<<bspeed<<endl;
                //   cout<<"x:"<<x<<"y:"<<y<<"theta:"<<theta<<endl;
            }
        }
        usleep(40000);
        //printf("X%x\n",rev_buf[0]);
        //cout<<"ss"<<endl;
    }
}
void SE_RECV::StopThread()
{
    pthread_mutex_destroy(&m_Mutexreceive);
}
void SE_RECV::getMessage()
{
    pthread_mutex_lock(&m_Mutexreceive);
    recvmod_s=recvmod;
    aspeed_s=aspeed;
    bspeed_s=bspeed;
    x_s=x;
    y_s=y;
    theta_s=theta;
    isD_s=isD;
    pthread_mutex_unlock(&m_Mutexreceive);
}
void * ReceivingserMessage(void *arg)
{
    SE_RECV *command = (SE_RECV *)arg;
    command->StartRun();
}

