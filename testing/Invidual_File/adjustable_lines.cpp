#include <opencv2/opencv.hpp>
#include <iostream>

// Window and Trackbar names
const std::string window_name = "Adjustable Lines";
const std::string trackbar_horizontal_left = "Horizontal Left";
const std::string trackbar_horizontal_right = "Horizontal Right";
const std::string trackbar_vertical_top = "Vertical Line Top";
const std::string trackbar_vertical_bottom = "Vertical Line Bottom";

// Frame dimensions
const int frame_width = 800;
const int frame_height = 600;

// Line parameters
int horizontal_left = 0;
int horizontal_right = frame_width;
int vertical_line_x_top = frame_width / 2;
int vertical_line_x_bottom = frame_width / 2;
int horizontal_line_y = frame_height / 2;  // This is the y-coordinate of the horizontal line

// Function to update and display the frame
void updateFrame(cv::Mat& frame) {
    // Clear the frame
    frame = cv::Mat::zeros(frame_height, frame_width, CV_8UC3);

    // Draw the horizontal line between the two points
    cv::line(frame, cv::Point(horizontal_left, horizontal_line_y), cv::Point(horizontal_right, horizontal_line_y), cv::Scalar(0, 255, 0), 2);

    // Draw the vertical line (top point follows the horizontal line)
    cv::line(frame, cv::Point(vertical_line_x_top, horizontal_line_y), cv::Point(vertical_line_x_bottom, frame_height), cv::Scalar(255, 0, 0), 2);

    // Show the frame
    cv::imshow(window_name, frame);
}

// Callback functions for trackbars
void onHorizontalLineLeft(int, void* userdata) {
    cv::Mat& frame = *(cv::Mat*)userdata;
    updateFrame(frame);
}

void onHorizontalLineRight(int, void* userdata) {
    cv::Mat& frame = *(cv::Mat*)userdata;
    updateFrame(frame);
}

void onVerticalLineTop(int, void* userdata) {
    cv::Mat& frame = *(cv::Mat*)userdata;
    updateFrame(frame);
}

void onVerticalLineBottom(int, void* userdata) {
    cv::Mat& frame = *(cv::Mat*)userdata;
    updateFrame(frame);
}

void onHorizontalLineY(int, void* userdata) {
    cv::Mat& frame = *(cv::Mat*)userdata;
    updateFrame(frame);
}

int main() {
    // Create a black frame
    cv::Mat frame = cv::Mat::zeros(frame_height, frame_width, CV_8UC3);

    // Create a window
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

    // Create trackbars for horizontal line left and right points
    cv::createTrackbar(trackbar_horizontal_left, window_name, &horizontal_left, frame_width, onHorizontalLineLeft, &frame);
    cv::createTrackbar(trackbar_horizontal_right, window_name, &horizontal_right, frame_width, onHorizontalLineRight, &frame);

    // Create trackbars for vertical line top and bottom points
    cv::createTrackbar(trackbar_vertical_top, window_name, &vertical_line_x_top, frame_width, onVerticalLineTop, &frame);
    cv::createTrackbar(trackbar_vertical_bottom, window_name, &vertical_line_x_bottom, frame_width, onVerticalLineBottom, &frame);

    // Create a trackbar for adjusting the horizontal line's y-coordinate (vertical position)
    cv::createTrackbar("Horizontal Line Y", window_name, &horizontal_line_y, frame_height - 1, onHorizontalLineY, &frame);

    // Initial display
    updateFrame(frame);

    // Wait until user exits the program
    cv::waitKey(0);
    return 0;
}
