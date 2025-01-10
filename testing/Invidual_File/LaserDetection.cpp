#include "LaserDetection.h"

LaserDetection::LaserDetection(const cv::Mat& frame) : frame(frame), optimal_line(), laser() {
    if (frame.empty()) {
        throw std::invalid_argument("Image not found or unable to load.");
    }
}

cv::Vec4i LaserDetection::selectOptimalLine(const std::vector<cv::Vec4i>& lines) {
    if (lines.empty()) {
        return cv::Vec4i();
    }

    int height = frame.rows;
    int width = frame.cols;
    std::vector<std::tuple<cv::Vec4i, double, double>> angles_lengths;

    for (const auto& line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];

        if (x1 < BORDER_THRESHOLD || x2 < BORDER_THRESHOLD || x1 > width - BORDER_THRESHOLD || x2 > width - BORDER_THRESHOLD ||
            y1 < BORDER_THRESHOLD || y2 < BORDER_THRESHOLD || y1 > height - BORDER_THRESHOLD || y2 > height - BORDER_THRESHOLD) {
            continue;
        }

        double angle = atan2(y2 - y1, x2 - x1);
        double length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        angles_lengths.emplace_back(line, angle, length);
    }

    if (angles_lengths.empty()) {
        return cv::Vec4i();
    }

    std::map<double, int> angle_counts;
    for (const auto& [line, angle, length] : angles_lengths) {
        angle_counts[angle]++;
    }

    auto most_common_angle = std::max_element(angle_counts.begin(), angle_counts.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    })->first;

    cv::Vec4i longest_line;
    double max_length = 0;
    for (const auto& [line, angle, length] : angles_lengths) {
        if (angle == most_common_angle && length > max_length) {
            longest_line = line;
            max_length = length;
        }
    }

    return longest_line;
}

cv::Vec4i LaserDetection::extendLineToBoundaries(const cv::Vec4i& line, const cv::Size& image_size, int midpoint) {
    if (line == cv::Vec4i()) {
        return cv::Vec4i();
    }

    int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
    int height = image_size.height;
    int width = image_size.width;

    if (y1 == y2) {
        return cv::Vec4i(0, y1 + midpoint, width, y2 + midpoint);
    } else if (x1 == x2) {
        return cv::Vec4i(x1, 0, x2, height);
    } else {
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        double intercept = y1 - slope * x1;

        int new_x1 = 0;
        int new_y1 = static_cast<int>(slope * new_x1 + intercept);
        int new_x2 = width;
        int new_y2 = static_cast<int>(slope * new_x2 + intercept);

        return cv::Vec4i(new_x1, new_y1 + midpoint, new_x2, new_y2 + midpoint);
    }
}

std::tuple<cv::Mat, int, int, int, int> LaserDetection::plotOptimalLine(int midpoint) {
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(laser, lines, 1, CV_PI / 180, 50, 0, 100);

    if (lines.empty()) {
        return {frame, -1, -1, -1, -1};
    }

    cv::Vec4i optimal_line = selectOptimalLine(lines);
    this->optimal_line = optimal_line;
    cv::Vec4i extended_line = extendLineToBoundaries(optimal_line, frame.size(), midpoint);

    if (extended_line != cv::Vec4i()) {
        cv::line(frame, cv::Point(optimal_line[0], optimal_line[1]),
                 cv::Point(optimal_line[2], optimal_line[3]), cv::Scalar(0, 0, 150), 2);
        return {frame, optimal_line[0], optimal_line[1], optimal_line[2], optimal_line[3]};
    }
    return {frame, -1, -1, -1, -1};
}

std::tuple<cv::Mat, int, int, int, int> LaserDetection::laserDetection() {
    cv::Mat hsv_frame;
    cv::cvtColor(frame, hsv_frame, cv::COLOR_BGR2HSV);

    cv::Mat hsv_blur;
    cv::bilateralFilter(hsv_frame, hsv_blur, 2, 90, 90);

    cv::Scalar lower_blue(90, 90, 40);
    cv::Scalar upper_blue(125, 255, 255);
    cv::Mat blue_mask;
    cv::inRange(hsv_blur, lower_blue, upper_blue, blue_mask);

    cv::Mat blue_regions;
    cv::bitwise_and(frame, frame, blue_regions, blue_mask);

    std::vector<cv::Mat> channels;
    cv::split(blue_regions, channels);
    cv::Mat gray_frame = channels[0];

    cv::Mat row_intensity_mask = cv::Mat::zeros(gray_frame.size(), CV_8U);

    for (int i = 0; i < gray_frame.rows; ++i) {
        double max_intensity;
        cv::minMaxLoc(gray_frame.row(i), nullptr, &max_intensity);

        cv::Mat mask;
        cv::compare(gray_frame.row(i), max_intensity, mask, cv::CMP_EQ);
        row_intensity_mask.row(i).setTo(max_intensity, mask);
    }

    cv::Mat com_mask = cv::Mat::zeros(gray_frame.size(), CV_8U);
    for (int i = 0; i < row_intensity_mask.rows; ++i) {
        auto row = row_intensity_mask.row(i);

        std::vector<int> white_indices;
        for (int j = 0; j < row.cols; ++j) {
            if (row.at<uchar>(0, j) > 0) {
                white_indices.push_back(j);
            }
        }

        if (!white_indices.empty()) {
            int a = white_indices.front();
            int b = white_indices.back();
            int com = (a + b) / 2;
            com_mask.at<uchar>(i, com) = 255;
        }
    }

    laser = com_mask;
    return plotOptimalLine();
}

/*


// LaserDetection.cpp
#include "LaserDetection.h"

LaserDetection::LaserDetection(const cv::Mat& frame) {
    if (frame.empty()) {
        throw std::invalid_argument("Image not found or unable to load.");
    }
    this->frame = frame;
}

std::vector<int> LaserDetection::selectOptimalLine(const std::vector<cv::Vec4i>& lines) {
    if (lines.empty()) {
        return {};
    }

    int height = frame.rows;
    int width = frame.cols;
    std::vector<std::tuple<std::vector<int>, double, double>> angles_lengths;

    for (const auto& line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];

        if (x1 < BORDER_THRESHOLD || x2 < BORDER_THRESHOLD ||
            x1 > width - BORDER_THRESHOLD || x2 > width - BORDER_THRESHOLD ||
            y1 < BORDER_THRESHOLD || y2 < BORDER_THRESHOLD ||
            y1 > height - BORDER_THRESHOLD || y2 > height - BORDER_THRESHOLD) {
            continue;
        }

        double angle = std::atan2(y2 - y1, x2 - x1);
        double length = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        angles_lengths.emplace_back(std::vector<int>{x1, y1, x2, y2}, angle, length);
    }

    if (angles_lengths.empty()) {
        return {};
    }

    std::unordered_map<double, int> angle_counts;
    for (const auto& [line, angle, length] : angles_lengths) {
        angle_counts[angle]++;
    }

    double common_angle = std::max_element(angle_counts.begin(), angle_counts.end(),
                                           [](const auto& a, const auto& b) {
                                               return a.second < b.second;
                                           })->first;

    std::vector<int> longest_line;
    double max_length = 0;
    for (const auto& [line, angle, length] : angles_lengths) {
        if (angle == common_angle && length > max_length) {
            longest_line = line;
            max_length = length;
        }
    }

    return longest_line;
}

std::vector<int> LaserDetection::extendLineToBoundaries(const std::vector<int>& line, const cv::Size& imageShape, int midpoint) {
    if (line.empty()) {
        return {};
    }

    int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
    int height = imageShape.height;
    int width = imageShape.width;

    int new_x1, new_y1, new_x2, new_y2;
    if (y1 == y2) {
        new_y1 = new_y2 = y1;
        new_x1 = 0;
        new_x2 = width;
    } else if (x1 == x2) {
        new_x1 = new_x2 = x1;
        new_y1 = 0;
        new_y2 = height;
    } else {
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        double intercept = y1 - slope * x1;
        new_x1 = 0;
        new_y1 = static_cast<int>(slope * new_x1 + intercept);
        new_x2 = width;
        new_y2 = static_cast<int>(slope * new_x2 + intercept);
    }

    return {new_x1, new_y1 + midpoint, new_x2, new_y2 + midpoint};
}

cv::Mat LaserDetection::plotOptimalLine(int& x1, int& y1, int& x2, int& y2, int midpoint) {
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(laser, lines, 1, CV_PI / 180, 50, 0, 100);

    if (lines.empty()) {
        x1 = y1 = x2 = y2 = -1;
        return frame;
    }

    std::vector<int> optimal_line = selectOptimalLine(lines);
    std::vector<int> extended_line = extendLineToBoundaries(optimal_line, frame.size(), midpoint);

    if (!extended_line.empty()) {
        x1 = extended_line[0];
        y1 = extended_line[1];
        x2 = extended_line[2];
        y2 = extended_line[3];
        cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), 3);
    } else {
        x1 = y1 = x2 = y2 = -1;
    }

    return frame;
}

cv::Mat LaserDetection::laserDetection(int& x1, int& y1, int& x2, int& y2) {
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    cv::Mat hsvBlur;
    cv::bilateralFilter(hsvFrame, hsvBlur, 2, 90, 90);

    cv::Scalar lowerBlue(90, 90, 40);
    cv::Scalar upperBlue(125, 255, 255);
    cv::Mat blueMask;
    cv::inRange(hsvBlur, lowerBlue, upperBlue, blueMask);

    cv::Mat blueRegions;
    cv::bitwise_and(frame, frame, blueRegions, blueMask);

    std::vector<cv::Mat> channels;
    cv::split(blueRegions, channels);
    cv::Mat grayFrame = channels[0];

    int height = grayFrame.rows;
    int width = grayFrame.cols;
    cv::Mat rowIntensityMask = cv::Mat::zeros(grayFrame.size(), CV_8U);

    for (int i = 0; i < height; ++i) {
        double minVal, maxVal;
        cv::Point minIdx, maxIdx;
        cv::minMaxLoc(grayFrame.row(i), &minVal, &maxVal, &minIdx, &maxIdx);

        if (maxVal > 0) {
            rowIntensityMask.at<uchar>(i, maxIdx.x) = static_cast<uchar>(maxVal);
        }
    }

    cv::Mat comMask = cv::Mat::zeros(grayFrame.size(), CV_8U);
    for (int i = 0; i < height; ++i) {
        auto row = rowIntensityMask.row(i);
        std::vector<int> whiteIndices;
        for (int j = 0; j < width; ++j) {
            if (row.at<uchar>(j) > 0) {
                whiteIndices.push_back(j);
            }
        }

        if (!whiteIndices.empty()) {
            int a = whiteIndices.front();
            int b = whiteIndices.back();
            int com = (a + b) / 2;
            comMask.at<uchar>(i, com) = 255;
        }
    }

    laser = comMask;
    return plotOptimalLine(x1, y1, x2, y2, 0);
}


*/

/*

// LaserDetection.h
#ifndef LASER_DETECTION_H
#define LASER_DETECTION_H

#include <opencv2/opencv.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xio.hpp>
#include <cmath>
#include <vector>

class LaserDetection {
public:
    static const int BORDER_THRESHOLD = 1;

    LaserDetection(const cv::Mat& frame);
    cv::Mat laserDetection(int& x1, int& y1, int& x2, int& y2);

private:
    cv::Mat frame;
    cv::Mat laser;
    std::vector<int> selectOptimalLine(const std::vector<cv::Vec4i>& lines);
    std::vector<int> extendLineToBoundaries(const std::vector<int>& line, const cv::Size& imageShape, int midpoint);
    cv::Mat plotOptimalLine(int& x1, int& y1, int& x2, int& y2, int midpoint);
};

#endif // LASER_DETECTION_H


*/