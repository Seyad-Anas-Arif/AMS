#include "../inc/SharedVariables.h"
#include <opencv2/opencv.hpp>

// Define global variables
int image_height;
int image_width ;
int line_position_y_start = image_height / 2;
int line_position_y_end = image_height / 2;
int point_position = image_width / 2;
int bottom_line_y = image_height;
int bottom_point_position = image_width / 2;
int line_thickness = 2;
int offset_line_value = 130;

// Define default values (initialized to initial values)
int default_line_position_y_start = line_position_y_start;
int default_line_position_y_end = line_position_y_end;
int default_point_position = point_position;
int default_bottom_point_position = bottom_point_position;

 cv::VideoCapture cap;

int bottom_px;
int py = image_height / 2;
int px = image_width / 2;

//creating varible for  drwaing line in frame
//x1 is bottom point of vertical line (where y is maximum)
int X_of_vb = image_width / 2;
int Y_of_vb = image_height;
//x2 is top point of vertical line (where x and y changes/ starts from middle of the image)
int X_of_vt = image_width / 2;
int Y_of_vt = image_height / 2;
//y1 is left border point of horizontal line (where x is maximum)
int X_of_hl = image_width; 
int Y_of_hl = image_height / 2;
// y2 is right border point of horizontal line (where x is minimum)
int X_of_hr = 0;
int Y_of_hr = image_height / 2;