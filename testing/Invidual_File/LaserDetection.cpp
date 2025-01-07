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
