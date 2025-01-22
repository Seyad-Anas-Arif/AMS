#ifndef SHAREDVARIABLES_H
#define SHAREDVARIABLES_H
#include <opencv2/opencv.hpp>

extern int image_height;
extern int image_width;

extern int default_line_position_y_start;
extern int default_line_position_y_end;
extern int default_point_position;
extern int default_bottom_point_position;

extern int line_thickness;
extern int line_position_y_start ;
extern int line_position_y_end ;
extern int point_position ;
extern int bottom_line_y ;
extern int bottom_point_position ;
extern int line_thickness;
extern int offset_line_value;

extern int bottom_px;
extern int py;
extern int px;
extern bool plot_point;


extern int X_of_vb;
extern int Y_of_vb;
extern int X_of_vt;
extern int Y_of_vt;
extern int X_of_hl;
extern int Y_of_hl;
extern int X_of_hr;
extern int Y_of_hr;


extern cv::VideoCapture cap;
#endif // SHAREDVARIABLES_H
