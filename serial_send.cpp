#include"serial_send.h"

void * SendMessage(void *arg);
SE_SEND::SE_SEND()
{
}
SE_SEND::~SE_SEND()
{
}
unsigned short usSENDMBCRC16(u_char *pucFrame, unsigned short usLen)
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
void SE_SEND::init(SerialAdapter seri)
{
    seri_send=seri;
    sendmod=0;
    av=0;
    bv=0;
    sendmod_s=0;
    av_s=0;
    bv_s=0;
}
void SE_SEND::StartThread()
{
    pthread_mutex_init(&m_Mutexsend,NULL);
    pthread_t a_thread;
    int res =pthread_create(&a_thread,NULL,SendMessage,this);
    if(res !=0)
    {
        printf("receiving thread create failed!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }
}
void SE_SEND::StopThread()
{
    pthread_mutex_destroy(&m_Mutexsend);
}
void SE_SEND::setMessage()
{
    pthread_mutex_lock(&m_Mutexsend);
    av=av_s;
    bv=bv_s;
    sendmod=sendmod_s;
    pthread_mutex_unlock(&m_Mutexsend);
}
void SE_SEND::StartRun()
{
    DATA32 a_v;
    DATA32 b_v;
    DATA16 CRC;

    while (1)
    {
        if(sendmod==oldsendmod&&sendmod==2&&oldav==av&&oldbv==bv)
            continue;
        oldsendmod=sendmod;
        oldav=av;
        oldbv=bv;

        a_v.f=av;
        b_v.f=bv;

        unsigned char sendbuf[12];
        memset(&sendbuf,0,sizeof(sendbuf));
        sendbuf[0]=0XAA;
        sendbuf[1]=(char)sendmod;
        for(int i=0;i<4;i++)
        {
            sendbuf[i+2]=a_v.uc[i];
            sendbuf[i+6]=b_v.uc[i];
        }
        CRC.us=usSENDMBCRC16((u_char*)sendbuf,10);
        sendbuf[10]=CRC.uc[0];
        sendbuf[11]=CRC.uc[1];
//        for(int i=0;i<12;i++)
//        {
//            printf("0X%x\n",sendbuf[i]);
//        }
       int slen= seri_send.Write(sendbuf,sizeof(sendbuf));

        usleep(80000);
        //cout<<"write success"<<endl;
        //if(ser>0)
        //cout<<"write success"<<endl;
    }
}
void * SendMessage(void *arg)
{
    SE_SEND *command = (SE_SEND *)arg;
    command->StartRun();
}
