#include "serial_adapter.h"
//串口相关的头文件
#include<stdio.h>      /*标准输入输出定义*/
#include<stdlib.h>     /*标准函数库定义*/
#include<unistd.h>     /*Unix 标准函数定义*/
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>      /*文件控制定义*/
#include<termios.h>    /*PPSIX 终端控制定义*/
#include<errno.h>      /*错误号定义*/
#include<string.h>

//宏定义
#define FALSE  -1
#define TRUE   0

SerialAdapter::SerialAdapter()
{
}

void SerialAdapter::Initialize(const char *dev, int baudrate)
{
    int flow_ctrl = 0;
    int databits = 8;
    int stopbits = 1;
    int parity = 'N';
    int   i;
    int   status;
    int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};
    int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};

    struct termios options;

    // step 1 open the serial device
    fd_ = open( dev, O_RDWR|O_NOCTTY|O_NDELAY);
    if (FALSE == fd_)
    {
        perror("Can't Open Serial Port");
        return;
    }
    //恢复串口为阻塞状态
    if(fcntl(fd_, F_SETFL, 0) < 0)
    {
        printf("fcntl failed!\n");
        return;
    }
    else
    {
        printf("fcntl=%d\n",fcntl(fd_, F_SETFL,0));
    }
    //测试是否为终端设备
    if(0 == isatty(STDIN_FILENO))
    {
        printf("standard input is not a terminal device\n");
        return;
    }
    else
    {
        printf("isatty success!\n");
    }
    printf("fd_->open=%d\n",fd_);

    // step 2 set baudrate
    /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.*/
    if( tcgetattr( fd_,&options)  !=  0)
    {
        perror("SetupSerial 1");
        return;
    }

    //设置串口输入波特率和输出波特率
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if  (baudrate == name_arr[i])
        {
            cfsetispeed(&options, speed_arr[i]);
            cfsetospeed(&options, speed_arr[i]);
        }
    }

    //修改控制模式，保证程序不会占用串口
    options.c_cflag |= CLOCAL | CREAD;
    //修改控制模式，使得能够从串口中读取输入数据
    options.c_cflag |= CREAD;

    //设置数据流控制
    switch(flow_ctrl)
    {

    case 0 ://不使用流控制
        options.c_cflag &= ~CRTSCTS;
        break;

    case 1 ://使用硬件流控制
        options.c_cflag |= CRTSCTS;
        break;
    case 2 ://使用软件流控制
        options.c_cflag |= IXON | IXOFF | IXANY;
        break;
    }
    //设置数据位
    //屏蔽其他标志位
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 5    :
        options.c_cflag |= CS5;
        break;
    case 6    :
        options.c_cflag |= CS6;
        break;
    case 7    :
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr,"Unsupported data size\n");
        return;
    }
    //设置校验位
    switch (parity)
    {
    case 'n':
    case 'N': //无奇偶校验位。
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        break;
    case 'o':
    case 'O'://设置为奇校验
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E'://设置为偶校验
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
        break;
    case 's':
    case 'S': //设置为空格
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr,"Unsupported parity\n");
        return;
    }
    // 设置停止位
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB; break;
    case 2:
        options.c_cflag |= CSTOPB; break;
    default:
        fprintf(stderr,"Unsupported stop bits\n");
        return;
    }

    //修改输出模式，原始数据输出
    options.c_oflag &= ~OPOST;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//我加的
    //options.c_lflag &= ~(ISIG | ICANON);
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    
    //设置等待时间和最小接收字符
    options.c_cc[VTIME] = 0; /* 读取一个字符等待1*(1/10)s */
    options.c_cc[VMIN] = 0; /* 读取字符的最少个数为1 */

    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读
    tcflush(fd_,TCIFLUSH);

    //激活配置 (将修改后的termios数据设置到串口中）
    if (tcsetattr(fd_,TCSANOW,&options) != 0)
    {
        perror("com set error!\n");
        return;
    }
    printf("set done!\n");
    return;
}

int SerialAdapter::Read(u_char *rcv_buf, int data_len)
{
    int len,fs_sel;
    fd_set fs_read;

    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(fd_,&fs_read);

    time.tv_sec = 0;
    time.tv_usec = 0;

//    使用select实现串口的多路通信
    fs_sel = select(fd_+1,&fs_read,NULL,NULL,&time);
    if(fs_sel)
    {
        len = read(fd_,rcv_buf,data_len);
        // printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);
        return len;
    }
    return 0;
}

int SerialAdapter::ReadNonBlock(u_char *rcv_buf, int data_len)
{
    int len,fs_sel;
    fd_set fs_read;

    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(fd_,&fs_read);

    time.tv_sec = 0;
    time.tv_usec = 1000;

//    使用select实现串口的多路通信
    fs_sel = select(fd_+1,&fs_read,NULL,NULL,&time);
    if(fs_sel)
    {
        len = read(fd_,rcv_buf,data_len);
//        printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);
        return len;
    }
    return 0;
}

int SerialAdapter::Write(u_char *send_buf, int data_len)
{
    int len = 0;

    len = write(fd_,send_buf,data_len);

    if (len != data_len )
    {
        tcflush(fd_,TCOFLUSH);
    }
    return len;
}

