#include "../inc/RegionOfInterest.h"

RegionOfInterest::RegionOfInterest(cv::Mat frame, int line_position_y_start, int line_position_y_end, 
                                   int point_position, int bottom_point_position, int px_run, 
                                   int py_run, int bottom_px_run, int offset)
    : frame(frame), line_position_y_start(line_position_y_start), line_position_y_end(line_position_y_end),
      point_position(point_position), bottom_point_position(bottom_point_position), px_run(px_run), 
      py_run(py_run), bottom_px_run(bottom_px_run), offset(offset) {
    image_height = frame.rows;
    image_width = frame.cols;
    bottom_py_run = image_height;
}

std::vector<std::pair<int, int>> RegionOfInterest::calculateExtendedIntersections(int start_x, int start_y, int end_x, int end_y) {
    std::vector<std::pair<int, int>> intersections;
    std::cout<<"region of interst is running"<<std::endl;
    double slope = (end_x != start_x) ? static_cast<double>(end_y - start_y) / (end_x - start_x) : std::numeric_limits<double>::infinity();

    if (slope != std::numeric_limits<double>::infinity()) {
        int y_at_x0 = start_y - static_cast<int>(slope * (start_x + offset));
        if (y_at_x0 >= 0 && y_at_x0 < image_height) {
            intersections.emplace_back(0, y_at_x0);
        }

        int y_at_x_max = start_y + static_cast<int>(slope * (image_width - start_x - offset - 1));
        if (y_at_x_max >= 0 && y_at_x_max < image_height) {
            intersections.emplace_back(image_width - 1, y_at_x_max);
        }

        int x_at_y0 = start_x + offset + static_cast<int>((0 - start_y) / slope);
        if (x_at_y0 >= 0 && x_at_y0 < image_width) {
            intersections.emplace_back(x_at_y0, 0);
        }

        int x_at_y_max = start_x + offset + static_cast<int>((image_height - 1 - start_y) / slope);
        if (x_at_y_max >= 0 && x_at_y_max < image_width) {
            intersections.emplace_back(x_at_y_max, image_height - 1);
        }
    } else {
        intersections.emplace_back(start_x + offset, 0);
        intersections.emplace_back(start_x + offset, image_height - 1);
    }
    std::cout<<"region of intressed is completed"<<std::endl;
    return intersections;
}

std::pair<int, int> RegionOfInterest::findIntersection(const std::pair<int, int>& point1, const std::pair<int, int>& point2) {
    const double epsilon = 1e-6;

    int x1 = point1.first, y1 = point1.second;
    int x2 = point2.first, y2 = point2.second;

    double m_y = (x1 != x2) ? static_cast<double>(y2 - y1) / (x2 - x1) : std::numeric_limits<double>::infinity();
    double c_y = (x1 != x2) ? y1 - m_y * x1 : 0;

    double m_w = static_cast<double>(line_position_y_end - line_position_y_start) / (image_width - 1);
    double c_w = line_position_y_start;

    if (std::abs(m_y - m_w) < epsilon) {
        return {-1, -1};
    }

    int x_intersect, y_intersect;
    if (m_y == std::numeric_limits<double>::infinity()) {
        x_intersect = x1;
        y_intersect = static_cast<int>(m_w * x_intersect + c_w);
    } else {
        x_intersect = static_cast<int>((c_w - c_y) / (m_y - m_w));
        y_intersect = static_cast<int>(m_y * x_intersect + c_y);
    }

    if (x_intersect >= 0 && x_intersect < image_width && y_intersect >= 0 && y_intersect < image_height) {
        return {x_intersect, y_intersect};
    } else {
        return {-1, -1};
    }
}

std::tuple<cv::Mat, cv::Mat, std::pair<int, int>, std::pair<int, int>> RegionOfInterest::ROI() {
    cv::Mat img = frame.clone();
    cv::Mat mask = cv::Mat::zeros(image_height, image_width, CV_8U);

    auto intersections = calculateExtendedIntersections(px_run, py_run, bottom_px_run, bottom_py_run);
    if (intersections.size() < 2) {
        return {img, mask, {image_width, image_height}, {image_width, 0}};
    }

    auto upper_endpoint = *std::min_element(intersections.begin(), intersections.end(), [](auto& a, auto& b) { return a.second < b.second; });
    auto lower_endpoint = *std::max_element(intersections.begin(), intersections.end(), [](auto& a, auto& b) { return a.second < b.second; });

    std::vector<cv::Point> mask1_points = {
        {0, line_position_y_start}, {image_width, line_position_y_end},
        {image_width, image_height}, {0, image_height}
    };
    cv::fillPoly(mask, mask1_points, 255);

    return {img, mask, lower_endpoint, upper_endpoint};
}
