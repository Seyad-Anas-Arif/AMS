#include "../inc/SharedVariables.h"

// Define global variables
int image_height;
int image_width ;
int line_position_y_start = image_height / 2;
int line_position_y_end = image_height / 2;
int point_position = image_width / 2;
int bottom_line_y = image_height;
int bottom_point_position = image_width / 2;
int line_thickness = 2;

// Define default values (initialized to initial values)
int default_line_position_y_start = line_position_y_start;
int default_line_position_y_end = line_position_y_end;
int default_point_position = point_position;
int default_bottom_point_position = bottom_point_position;