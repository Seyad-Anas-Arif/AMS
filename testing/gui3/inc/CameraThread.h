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

    int image_height;
    int image_width;
    // Define global variables
    int line_position_y_start = image_height / 2;
    int line_position_y_end = image_height / 2;
    int point_position = image_width / 2;
    int bottom_line_y = image_height;
    int bottom_point_position = image_width / 2;
    int line_thickness = 2;    

    // Define default values (initialized to initial values)
    int default_line_position_y_start = line_position_y_start;
    int default_line_position_y_end = line_position_y_end;
    int default_point_position = point_position;
    int default_bottom_point_position = bottom_point_position;


    cv::VideoCapture cap;

    void processFrame(cv::Mat &frame);
    std::tuple<cv::Mat, int, int, int, int, int, int, int> UsersLiveFeed(const cv::Mat& frame, int line_position_y_start, int line_position_y_end, int point_position, int bottom_point_position, int px, int py, int bottom_px);

};

#endif // CAMERATHREAD_H
