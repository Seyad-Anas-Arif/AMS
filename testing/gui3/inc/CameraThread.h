#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include "CalibrateFeed.h"     
#include "mainwindow.h"    
#include "RegionOfInterest.h"    
#include "BlueLaserDetection.h"       
#include "DepthDetection.h"  

#include <QThread>
#include <QImage>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>



class CameraThread : public QThread {
    Q_OBJECT

signals:
    void imageMain(const QImage &image);
    void imageResult(const QImage &image);

public:
    CameraThread();
    ~CameraThread();

    void stop();
    void updateExposure(int value);
    void updateOffsetLineValue(int value);

protected:
    void run() override;

private:
    bool running;
    int target_height;
    int target_width;
    int exposure_value;
    int offset_line_value;

    cv::VideoCapture cap;

    void processFrame(cv::Mat &frame);
};

#endif // CAMERATHREAD_H
