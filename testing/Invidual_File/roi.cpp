#include "roi.h"
#include <opencv2/opencv.hpp>
#include <iostream>

// Global parameters
int image_height = 500;
int image_width = 800;
int line_position_y_start = image_height / 2;
int line_position_y_end = image_height / 2;
int line_thickness = 2;
int point_position = image_width / 2;
int bottom_line_y = image_height;
int bottom_point_position = image_width / 2;

cv::Mat ROI(cv::Mat &frame) {
    cv::Mat img = frame.clone();

    // Draw horizontal white line
    cv::line(img, cv::Point(0, line_position_y_start), cv::Point(image_width - 1, line_position_y_end), 
             cv::Scalar(255, 255, 255), line_thickness);

    // Calculate and draw the point on the first line
    int px = point_position;
    int py = line_position_y_start + static_cast<int>((point_position / static_cast<float>(image_width)) * 
                                                       (line_position_y_end - line_position_y_start));
    px = std::clamp(px, 0, image_width - 1);
    py = std::clamp(py, 0, image_height - 1);

    // Calculate and draw the point on the bottom line
    int bottom_px = bottom_point_position;
    bottom_px = std::clamp(bottom_px, 0, image_width - 1);

    // Draw the line connecting both points
    cv::line(img, cv::Point(px, py), cv::Point(bottom_px, bottom_line_y), cv::Scalar(0, 0, 0), 3);

    // Display result
    cv::imshow("User's Viewpoint", img);

    // Implement further logic for intersections and region marking here
    // ...

    return img;
}

void update_y_axis_start(int value) {
    line_position_y_start = image_height - value;
    std::cout << "Start Y-Axis: " << line_position_y_start << std::endl;
}

void update_y_axis_end(int value) {
    line_position_y_end = image_height - value;
    std::cout << "End Y-Axis: " << line_position_y_end << std::endl;
}

void update_point_position(int value) {
    point_position = value;
    std::cout << "Point Position: " << value << std::endl;
}

void update_bottom_point_position(int value) {
    bottom_point_position = value;
    std::cout << "Bottom Point Position: " << value << std::endl;
}
