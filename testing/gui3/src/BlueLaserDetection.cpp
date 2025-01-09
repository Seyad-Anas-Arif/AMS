#include "../inc/BlueLaserDetection.h"
#include <cmath>
#include <tuple>
#include <algorithm>
#include <iostream>

// Constructor
LaserDetection::LaserDetection(const cv::Mat& frame) : frame(frame) {
    if (frame.empty()) {
        throw std::invalid_argument("Image not found or unable to load.");
    }
}

// Function to select the optimal line
std::vector<cv::Vec4i> LaserDetection::selectOptimalLine(const std::vector<cv::Vec4i>& lines) {
    if (lines.empty()) return {};

    int height = frame.rows;
    int width = frame.cols;

    std::vector<std::tuple<cv::Vec4i, double, double>> anglesLengths;

    for (const auto& line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];

        // Ignore lines near the border
        if (x1 < BORDER_THRESHOLD || x2 < BORDER_THRESHOLD || x1 > width - BORDER_THRESHOLD || 
            x2 > width - BORDER_THRESHOLD || y1 < BORDER_THRESHOLD || y2 < BORDER_THRESHOLD || 
            y1 > height - BORDER_THRESHOLD || y2 > height - BORDER_THRESHOLD) {
            continue;
        }

        double angle = atan2(y2 - y1, x2 - x1);
        double length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        anglesLengths.emplace_back(line, angle, length);
    }

    if (anglesLengths.empty()) return {};

    // Determine the most common angle
    std::map<double, int> angleCounts;
    for (const auto& [line, angle, length] : anglesLengths) {
        angleCounts[angle]++;
    }

    double commonAngle = std::max_element(angleCounts.begin(), angleCounts.end(),
                                          [](const auto& a, const auto& b) { return a.second < b.second; })->first;

    // Find the longest line with the most common angle
    cv::Vec4i longestLine;
    double maxLength = 0.0;
    for (const auto& [line, angle, length] : anglesLengths) {
        if (angle == commonAngle && length > maxLength) {
            longestLine = line;
            maxLength = length;
        }
    }

    return {longestLine};
}

// Function to extend line to image boundaries
std::tuple<int, int, int, int> LaserDetection::extendLineToBoundaries(const cv::Vec4i& line, const cv::Size& imageShape, int midpoint) {
    int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
    int height = imageShape.height;
    int width = imageShape.width;

    if (y1 == y2) {
        return {0, y1 + midpoint, width, y2 + midpoint};
    } else if (x1 == x2) {
        return {x1, 0, x2, height};
    } else {
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        double intercept = y1 - slope * x1;
        int new_x1 = 0;
        int new_y1 = static_cast<int>(slope * new_x1 + intercept);
        int new_x2 = width;
        int new_y2 = static_cast<int>(slope * new_x2 + intercept);
        return {new_x1, new_y1 + midpoint, new_x2, new_y2 + midpoint};
    }
}

// Function to plot the optimal line
cv::Mat LaserDetection::plotOptimalLine(int& x1, int& y1, int& x2, int& y2, int midpoint) {
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(laser, lines, 1, CV_PI / 180, 50, 0, 100);

    if (lines.empty()) return frame;

    optimalLine = selectOptimalLine(lines);
    if (optimalLine.empty()) return frame;

    auto [new_x1, new_y1, new_x2, new_y2] = extendLineToBoundaries(optimalLine[0], frame.size(), midpoint);

    cv::line(frame, cv::Point(new_x1, new_y1), cv::Point(new_x2, new_y2), cv::Scalar(0, 0, 150), 3);
    x1 = new_x1;
    y1 = new_y1;
    x2 = new_x2;
    y2 = new_y2;

    return frame;
}

// Main function for laser detection
cv::Mat LaserDetection::laserDetection(int& x1, int& y1, int& x2, int& y2) {
    std::cout<<"Laser detection is runnig"<<std::endl;
    cv::Mat hsvFrame, hsvBlur, blueMask, blueRegions;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);
    cv::bilateralFilter(hsvFrame, hsvBlur, 2, 90, 90);

    cv::Scalar lowerBlue(90, 90, 40), upperBlue(125, 255, 255);
    cv::inRange(hsvBlur, lowerBlue, upperBlue, blueMask);

    cv::bitwise_and(frame, frame, blueRegions, blueMask);
    cv::Mat b, g, r;
    cv::split(blueRegions, std::vector<cv::Mat>{b, g, r});

    cv::Mat grayFrame = b;
    cv::Mat rowIntensityMask = cv::Mat::zeros(grayFrame.size(), CV_8U);
    int height = grayFrame.rows;

    for (int i = 0; i < height; ++i) {
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(grayFrame.row(i), &minVal, &maxVal, &minLoc, &maxLoc);

        if (maxVal > 0) rowIntensityMask.at<uchar>(i, maxLoc.x) = static_cast<uchar>(maxVal);
    }

    laser = rowIntensityMask;
    return plotOptimalLine(x1, y1, x2, y2);
}
