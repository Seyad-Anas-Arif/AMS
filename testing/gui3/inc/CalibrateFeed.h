#ifndef CALIBRATEFEED_H
#define CALIBRATEFEED_H

#include <opencv2/opencv.hpp>
#include <string>
#include <QDebug>

class CalibrateFeed {
private:
    cv::Mat frame;

public:
    explicit CalibrateFeed(const cv::Mat& frame);
    cv::Mat calibrate(const std::string& calibrationFilePath);
};
#ifdef ENABLE_CALIBRATE_DEBUG
    #define CALIBRATE_FILE_DEBUG qDebug()
#else
    #define CALIBRATE_FILE_DEBUG if (false) qDebug()
#endif

#endif // CALIBRATEFEED_H
