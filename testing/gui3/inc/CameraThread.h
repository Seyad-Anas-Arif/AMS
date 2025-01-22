#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <QDebug>
#include <QSlider>

#include "ui_mainwindow.h"
#include "CalibrateFeed.h"     
#include "mainwindow.h"    
#include "RegionOfInterest.h"    
#include "BlueLaserDetection.h"       
#include "DepthDetection.h" 


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

    // image_height =0;
    // image_width =0;
    // // Define global variables
    // int line_position_y_start = image_height / 2;
    // int line_position_y_end = image_height / 2;
    // int point_position = image_width / 2;
    // int bottom_line_y = image_height;
    // int bottom_point_position = image_width / 2;
    // int line_thickness = 2;    

    // Define default values (initialized to initial values)
    //  default_line_position_y_start = line_position_y_start;
    //  default_line_position_y_end = line_position_y_end;
    //  default_point_position = point_position;
    //  default_bottom_point_position = bottom_point_position;


    void processFrame(cv::Mat &frame);
   // std::tuple<cv::Mat, int, int, int, int, int, int, int> UsersLiveFeed(const cv::Mat& frame);
    Ui::MainWindow *ui;

};

#ifdef ENABLE_CAM_DEBUG
    #define CAM_FILE_DEBUG qDebug()
#else
    #define CAM_FILE_DEBUG if (false) qDebug()
#endif


#endif // CAMERATHREAD_H
