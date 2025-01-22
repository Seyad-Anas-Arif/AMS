#ifndef DEPTHDETECTION_H
#define DEPTHDETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <string>
#include <QDebug>

class XYZ {
public:
    XYZ(int x1, int y1, int x2, int y2);
    std::vector<cv::Point3f> depth();

private:
    int x1, y1, x2, y2;

    std::vector<cv::Point> bresenhamLine(int x1, int y1, int x2, int y2);

    // Camera calibration parameters
    static cv::Mat cameraMatrix;
    static cv::Mat distCoeffs;
    static double fx, fy, cx, cy;
    static double pixelSizeMM;
    static double sensorWidthMM, sensorHeightMM;
    static double focalLengthMMX, focalLengthMMY;
    static double principalPointMMX, principalPointMMY;
    static double baseline;
    static double alpha; // Camera tilt angle in radians

    static void loadCalibrationData(const std::string &calibrationFile);
};
#ifdef ENABLE_DD_DEBUG
    #define DD_FILE_DEBUG qDebug()
#else
    #define DD_FILE_DEBUG if (false) qDebug()
#endif

#endif // DEPTHDETECTION_H
