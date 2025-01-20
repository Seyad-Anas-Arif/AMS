#ifndef ROI_H
#define ROI_H

#include <opencv2/opencv.hpp>

// Global parameters
extern int image_height;
extern int image_width;
extern int line_position_y_start;
extern int line_position_y_end;
extern int line_thickness;
extern int point_position;
extern int bottom_line_y;
extern int bottom_point_position;

// Function declarations
cv::Mat ROI(cv::Mat &frame);
void update_y_axis_start(int value);
void update_y_axis_end(int value);
void update_point_position(int value);
void update_bottom_point_position(int value);

#endif // ROI_H
