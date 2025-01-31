#ifndef REGIONOFINTEREST_H
#define REGIONOFINTEREST_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>
#include <QDebug>

class RegionOfInterest {
private:
    cv::Mat frame;
    int offset;
    int line_position_y_start;
    int line_position_y_end;
    int point_position;
    int bottom_point_position;
    int px_run;
    int py_run;
    int bottom_px_run;
    int image_height;
    int image_width;
    int bottom_py_run;

    std::vector<cv::Point> calculateExtendedIntersections(int start_x, int start_y, int end_x, int end_y);
     cv::Point findIntersection(cv::Point point_1, cv::Point point_2) ;

public:
    RegionOfInterest(cv::Mat frame, int line_position_y_start, int line_position_y_end, 
                     int point_position, int bottom_point_position, int px_run, 
                     int py_run, int bottom_px_run, int offset);

    std::tuple<cv::Mat, cv::Mat, cv::Point, cv::Point> ROI() ;
};

#ifdef ENABLE_ROI_DEBUG
    #define ROI_FILE_DEBUG qDebug()
#else
    #define ROI_FILE_DEBUG if (false) qDebug()
#endif

#endif // REGIONOFINTEREST_H
