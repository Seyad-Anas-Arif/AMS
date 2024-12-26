QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AM_APP
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h \
           ui_mainwindow.h

# OpenCV configuration using pkg-config
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio \
        -lopencv_imgcodecs -lopencv_features2d -lopencv_dnn \
        -lopencv_calib3d -lopencv_objdetect
INCLUDEPATH += /usr/include/opencv4
