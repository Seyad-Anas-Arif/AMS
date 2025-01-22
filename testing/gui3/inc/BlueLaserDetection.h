#ifndef BLUELASERDETECTION_H
#define BLUELASERDETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <QDebug>

#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <stdexcept>

class LaserDetection {
public:
    static const int BORDER_THRESHOLD = 1;

    LaserDetection(const cv::Mat& frame);

    cv::Vec4i selectOptimalLine(const std::vector<cv::Vec4i>& lines);

    cv::Vec4i extendLineToBoundaries(const cv::Vec4i& line, const cv::Size& image_size, int midpoint = 0);

    std::tuple<cv::Mat, int, int, int, int> plotOptimalLine(int midpoint = 0);

    std::tuple<cv::Mat, int, int, int, int> laserDetection();

private:
    cv::Mat frame;
    cv::Vec4i optimal_line;
    cv::Mat laser;
};
#ifdef ENABLE_LASER_DEBUG
    #define LASER_FILE_DEBUG qDebug()
#else
    #define LASER_FILE_DEBUG if (false) qDebug()
#endif
#endif // BLUELASERDETECTION_H
