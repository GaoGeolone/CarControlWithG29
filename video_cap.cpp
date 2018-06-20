// coded by Luo Junren
// 2018.05.24
#include<opencv2/opencv.hpp>
#include<opencv/cv.h>
#include<stdio.h>
#include <iostream>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include"robot.h"
#include"video_cap.h"
//#include"roadutilities.h"
using namespace cv;
using namespace std;

  void * videomessage(void *arg);
  int captureFrame(int src, string dst, int sample_interval , int width , int height , int fps);

  VideoCap::VideoCap()
  {}
  VideoCap::~VideoCap()
  {}

  void VideoCap::init() //ROBOT rb)
  {
       // robot_v=rb;
  }

  void  VideoCap::StartThread()
  {
      pthread_mutex_init(&m_Mutexvideocap,NULL);
      pthread_t a_thread;
      int res =pthread_create(&a_thread,NULL,videomessage,this);
      if(res !=0)
      {
          printf("videocapture thread create failed!!!!!!!!!!!!!!!!!!!!!!!!\n");
      }
  }

  void  VideoCap::StopThread()
  {
          pthread_mutex_destroy(&m_Mutexvideocap);
  }

  void * videomessage(void *arg)
  {
      VideoCap *command = (VideoCap *)arg;
      command->StartRun();
  }

  void  VideoCap::StartRun()
  {
       int number= captureFrame(0, "../images/", 10, 640, 480, 10);
//       cout<<"toal image"<<number<<"success"<<endl;
  }


  int captureFrame(int src, string dst, int sample_interval , int width , int height , int fps)
  {
      int name=1;
      char str[80];
      int count=1;
      VideoCapture cap(src);
      if(!cap.isOpened())
      {
          cerr << "video not opened!" << endl;
          return 0;
      }
      cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
      cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
      cap.set(CV_CAP_PROP_FPS, fps);
      int cnt = 0;
      while(true)
      {
          count--;
          vector<Vec4i> lines;
          Mat frame, frame1, frame2,norm, blur, thresh, erodee, dil, edge;
          for (int i = 0; i < sample_interval; i++)
          {
              cap.grab();
              cap.retrieve(frame);
              imshow("frame from camera",frame);//640x480 col*row width*height
              sprintf(str,"./images/%d.jpg",name);
              imwrite(str,frame);

              //cout<<"ave image"<<name<<"success"<<endl;
              cvtColor(frame, frame1, CV_BGR2GRAY);//hui du
              //cvtColor(frame, frame2, CV_BGR2HSV);
              equalizeHist(frame1, norm);//zhi fang tu
              //imshow("equalizeHist", norm);
              //sprintf(str,"./imageshist/%d.jpg",name);
              //imwrite(str,norm);

              GaussianBlur( norm, blur, Size( 5, 5 ), 0, 0 );//
              //imshow("fGaussianBlur",blur);
              //sprintf(str,"./imagesblur/%d.jpg",name);
              //imwrite(str,blur);

             // threshold(blur, thresh, 125, 255, THRESH_BINARY);  //95 - 115
              threshold(blur, thresh, 120, 130, THRESH_BINARY);  //95 - 115
              imshow("threshold", thresh);
              sprintf(str,"./imagesthresh/%d.jpg",name);
              imwrite(str,thresh);

              erode(thresh,erodee,Mat());//fu shi
              dilate(erodee,dil,Mat());//peng zhang
              //imshow("erodee", dil);
              //sprintf(str,"./imagesterode/%d.jpg",name);
              //imwrite(str,dil);

              Canny( dil, edge, 200, 300, 3);
              imshow("Canny", edge);
              //sprintf(str,"./imagescanny/%d.jpg",name);
              //imwrite(str,edge);

              HoughLinesP( edge, lines, 1, CV_PI/180, 80, 10, 50);
              for(int i = 0; i < lines.size(); i++ )
              {
                line(frame1, Point(lines[i][0], lines[i][1]),
                Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
              }
              imshow("hist and lines", frame1 );
              sprintf(str,"./imageshough/%d.jpg",name);
              imwrite(str,frame1);
              name++;
          }
          cv::waitKey(50);
      }
     return cnt;
     //return lines;
  }



