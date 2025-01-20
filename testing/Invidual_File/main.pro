QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GUI3
TEMPLATE = app

SOURCES += main.cpp ControlWindow.cpp roi.cpp
HEADERS += ControlWindow.h roi.h

# OpenCV configuration using pkg-config
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio \
        -lopencv_imgcodecs -lopencv_features2d -lopencv_dnn \
        -lopencv_calib3d -lopencv_objdetect
INCLUDEPATH += /usr/include/opencv4

# matplotlib-cpp configuration
LIBS += -lpython3.10
INCLUDEPATH += /home/seyadanasarif/2024/Angle_measurement_sensor/AMS/testing/gui5/matplotlib-cpp \
               /usr/include/python3.10 \
               /usr/include/eigen3/
