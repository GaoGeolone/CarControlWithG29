#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ACC->setMinimum(0);
    ui->ACC->setMaximum(255);
    ui->Brake->setMinimum(0);
    ui->Brake->setMaximum(255);
    ui->cluch->setMinimum(0);
    ui->cluch->setMaximum(255);
    ui->Wheel->setDigitCount(4);
    ui->Wheel->setSmallDecimalPoint(1);
    ui->FORWARD->setCheckable(1);
    ui->NULLGEAR->setCheckable(1);
    ui->RETURNWARD->setCheckable(1);
    timer = new QTimer();
    timer->setInterval(100);//10fps

    //Init robot
    tiger.init("/dev/ttyS2",9600);
    tiger.runAsync(true);
}

MainWindow::~MainWindow()
{
    tiger.disconnect();
    delete ui;
}

void MainWindow::on_Start_clicked(void)
{
    //start the main process
    capture.open(0);//若测试摄像头有没有打开，/*
    if(!capture.isOpened())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("camera open failed"));
        msgBox.exec();
    }
    else
    {
        timer->start();//begin to Count time
        connect(timer,SIGNAL(timeout()),this,SLOT(OntimeRun()));
        //Init the UDP for receive Control signal and send Image Signal
        CtlRecever = new QUdpSocket(this);
        CtlRecever->bind(6665);//port 6665 meant for ctl
        connect(CtlRecever,SIGNAL(readyRead()),this,SLOT(CtlAction()));//active the respond
        //about the Image send
        ImgSender  = new QUdpSocket(this);
        ImgSender->bind(6666); //port 6666 meant for Image
    }

}
void MainWindow::OntimeRun(void)
{
    //this meant for sent Image
    capture >> frame;  //读取当前帧
    //640x480 col*row width*height
    //transform cv::mat to QImage
    cv::cvtColor(frame,frame,CV_BGR2RGB);
    QImage img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
    //first sent it to PC
    QByteArray buffer;
    buffer.resize(sizeof(img));
    memcpy(buffer.data(),&img,sizeof(img));
    //
    //distribute the frame

    int dataLength=buffer.size();

    unsigned char *dataBuffer=(unsigned char *)buffer.data();

    int packetNum = 0;
    int lastPaketSize = 0;
    packetNum = dataLength / UDP_MAX_SIZE;
    lastPaketSize = dataLength % UDP_MAX_SIZE;
    int currentPacketIndex = 0;
    if (lastPaketSize != 0)
    {
        packetNum = packetNum + 1;
    }

    PackageHeader packageHead;

    packageHead.uTransPackageHdrSize=sizeof(packageHead);
    packageHead.uDataSize = dataLength;
    packageHead.uDataPackageNum = packetNum;

    unsigned char frameBuffer[1024*1000];
    memset(frameBuffer,0,1024*1000);
    while (currentPacketIndex < packetNum)
    {
        if (currentPacketIndex < (packetNum-1))
        {
            packageHead.uTransPackageSize = sizeof(PackageHeader)+UDP_MAX_SIZE;
            packageHead.uDataPackageCurrIndex = currentPacketIndex+1;
            packageHead.uDataPackageOffset = currentPacketIndex*UDP_MAX_SIZE;
            memcpy(frameBuffer, &packageHead, sizeof(PackageHeader));
            memcpy(frameBuffer+sizeof(PackageHeader), dataBuffer+packageHead.uDataPackageOffset, UDP_MAX_SIZE);

            int length=ImgSender->writeDatagram(
                        (const char*)frameBuffer, packageHead.uTransPackageSize,
                        QHostAddress::Broadcast, 6666);

            if(length!=packageHead.uTransPackageSize)
            {
              qDebug()<<"Failed to send image";
            }

            currentPacketIndex++;
        }
        else
        {
            packageHead.uTransPackageSize = sizeof(PackageHeader)+(dataLength-currentPacketIndex*UDP_MAX_SIZE);
            packageHead.uDataPackageCurrIndex = currentPacketIndex+1;
            packageHead.uDataPackageOffset = currentPacketIndex*UDP_MAX_SIZE;
            memcpy(frameBuffer, &packageHead, sizeof(PackageHeader));
            memcpy(frameBuffer+sizeof(PackageHeader), dataBuffer+packageHead.uDataPackageOffset, dataLength-currentPacketIndex*UDP_MAX_SIZE);
            int length=ImgSender->writeDatagram(
                        (const char*)frameBuffer, packageHead.uTransPackageSize,
                        QHostAddress::Broadcast, 6666);

            if(length!=packageHead.uTransPackageSize)
            {
              qDebug()<<"Failed to send image";
            }

            currentPacketIndex++;
        }
    }
    //all the partion have been sent out
    //
    ui->OpencvCapture->clear();
    ui->OpencvCapture->setPixmap(QPixmap::fromImage(img));
    ui->OpencvCapture->resize(ui->OpencvCapture->pixmap()->size());
    ui->OpencvCapture->show();//disp
}
void MainWindow::CtlAction(void)
{//transmit the Control signal
    QByteArray temp;
    //get signal from socket
   // while (CtlRecever->hasPendingDatagrams())
    //{
        temp.resize(CtlRecever->pendingDatagramSize());
        CtlRecever->readDatagram(temp.data(),temp.size());
   //}
    //use the displayVideo function to display the image(Type of cv::Mat)
    CtlDara = (Actual_JoyStick_CrlData*)temp.data();
    ui->Wheel->display(CtlDara->Wheel);
    ui->ACC->setValue(CtlDara->Acc);
    ui->Brake->setValue(CtlDara->brake);
    ui->cluch->setValue(CtlDara->clutch);
    ui->FORWARD->setChecked(CtlDara->shiftgears==1);
    ui->NULLGEAR->setChecked(CtlDara->shiftgears==0);
    ui->RETURNWARD->setChecked(CtlDara->shiftgears==-1);
}
