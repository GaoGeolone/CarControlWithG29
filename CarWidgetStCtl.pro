#-------------------------------------------------
#
# Project created by QtCreator 2018-05-31T11:42:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CarWidgetStCtl
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
QT += network

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    video_cap.cpp \
    serial_send.cpp \
    serial_recv.cpp \
    serial_adapter.cpp \
    robot.cpp

HEADERS += \
        mainwindow.h \
    video_cap.h \
    serial_send.h \
    serial_recv.h \
    serial_adapter.h \
    robot.h

FORMS += \
        mainwindow.ui

INCLUDEPATH +=/usr/local/include \
    /usr/local/include/opencv2

LIBS += \
    -L/usr/local/lib \
    -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_ml -lopencv_calib3d -lopencv_video -lopencv_objdetect \
    -lopencv_features2d -lopencv_flann -lopencv_nonfree \
