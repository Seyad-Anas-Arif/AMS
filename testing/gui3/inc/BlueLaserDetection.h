#ifndef BLUELASERDETECTION_H
#define BLUELASERDETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>

class LaserDetection {
public:
    // Constructor
    explicit LaserDetection(const cv::Mat& frame);

    // Main function for detecting the laser
    cv::Mat laserDetection(int& x1, int& y1, int& x2, int& y2);

private:
    cv::Mat frame;
    cv::Mat laser;
    std::vector<cv::Vec4i> optimalLine;

    // Border threshold for ignoring lines near image boundaries
    static const int BORDER_THRESHOLD = 1;

    // Function to select the optimal line based on angle and length
    std::vector<cv::Vec4i> selectOptimalLine(const std::vector<cv::Vec4i>& lines);

    // Function to extend the line to image boundaries
    std::tuple<int, int, int, int> extendLineToBoundaries(const cv::Vec4i& line, const cv::Size& imageShape, int midpoint = 0);

    // Function to plot the optimal line on the frame
    cv::Mat plotOptimalLine(int& x1, int& y1, int& x2, int& y2, int midpoint = 0);
};

#endif // BLUELASERDETECTION_H
