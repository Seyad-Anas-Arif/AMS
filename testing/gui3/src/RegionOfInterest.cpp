#define ENABLE_ROI_DEBUG 1
#include "../inc/RegionOfInterest.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>


RegionOfInterest::RegionOfInterest(cv::Mat frame, int line_position_y_start, int line_position_y_end, int point_position, int bottom_point_position, 
                     int px_run, int py_run, int bottom_px_run, int offset)
        : frame(frame), offset(offset), line_position_y_start(line_position_y_start), line_position_y_end(line_position_y_end),
          point_position(point_position), bottom_point_position(bottom_point_position), px_run(px_run), py_run(py_run), 
          bottom_px_run(bottom_px_run) {

        image_height = frame.rows;
        image_width = frame.cols;
        bottom_py_run = image_height;
    }

std::vector<cv::Point> RegionOfInterest::calculateExtendedIntersections(int start_x, int start_y, int end_x, int end_y) {
    ROI_FILE_DEBUG << "Calculating the extended intersections";
        std::vector<cv::Point> intersections;
        double slope;

        // Calculate slope
        if (end_x != start_x) {
            slope = static_cast<double>(end_y - start_y) / (end_x - start_x);
        } else {
            slope = std::numeric_limits<double>::infinity();
        }

        // Calculate intersections with the image boundaries
        if (slope != std::numeric_limits<double>::infinity()) {
            // Intersection with the left boundary (x = 0)
            double y_at_x0 = start_y - slope * (start_x + offset);
            if (y_at_x0 >= 0 && y_at_x0 < image_height) {
                intersections.emplace_back(0, static_cast<int>(y_at_x0));
            }

            // Intersection with the right boundary (x = image_width - 1)
            double y_at_x_max = (start_y + slope) * (image_width - (start_x + offset) - 1);
            if (y_at_x_max >= 0 && y_at_x_max < image_height) {
                intersections.emplace_back(image_width - 1, static_cast<int>(y_at_x_max));
            }

            // Intersection with the top boundary (y = 0)
            double x_at_y0 = (start_x + offset) + (0 - start_y) / slope;
            if (x_at_y0 >= 0 && x_at_y0 < image_width) {
                intersections.emplace_back(static_cast<int>(x_at_y0), 0);
            }

            // Intersection with the bottom boundary (y = image_height - 1)
            double x_at_y_max = (start_x + offset) + (image_height - 1 - start_y) / slope;
            if (x_at_y_max >= 0 && x_at_y_max < image_width) {
                intersections.emplace_back(static_cast<int>(x_at_y_max), image_height - 1);
            }
        } else {
            // Vertical line case
            intersections = { {start_x + offset, 0}, {start_x + offset, image_height - 1} };
        }

        // Ensure unique points
        std::vector<cv::Point> unique_intersections;
        for (const auto& point : intersections) {
            if (std::find(unique_intersections.begin(), unique_intersections.end(), point) == unique_intersections.end()) {
                unique_intersections.push_back(point);
            }
        }

        return unique_intersections;
    }

    cv::Point RegionOfInterest::findIntersection(cv::Point point_1, cv::Point point_2) {
        ROI_FILE_DEBUG << "Finding the intersection";
        const double epsilon = 1e-6;  // Small value to check for parallel lines

        double m_y, c_y;
        if (point_1.x == point_2.x) {
            // Vertical line case
            m_y = std::numeric_limits<double>::infinity();
            c_y = 0;
        } else {
            m_y = static_cast<double>(point_2.y - point_1.y) / (point_2.x - point_1.x);
            c_y = point_1.y - m_y * point_1.x;
        }

        double m_w = static_cast<double>(line_position_y_end - line_position_y_start) / (image_width - 1);
        double c_w = line_position_y_start;

        // Check for parallel lines
        if (std::abs(m_y - m_w) < epsilon) {
            return cv::Point(-1, -1); // No intersection
        }

        double x_intersect, y_intersect;

        if (m_y == std::numeric_limits<double>::infinity()) {
            x_intersect = point_1.x;
            y_intersect = m_w * x_intersect + c_w;
        } else {
            x_intersect = (c_w - c_y) / (m_y - m_w);
            y_intersect = m_y * x_intersect + c_y;
        }

        if (x_intersect >= 0 && x_intersect < image_width && y_intersect >= 0 && y_intersect < image_height) {
            return cv::Point(static_cast<int>(x_intersect), static_cast<int>(y_intersect));
        } else {
            return cv::Point(-1, -1); // Intersection is out of bounds
        }
    }

    std::tuple<cv::Mat, cv::Mat, cv::Point, cv::Point> RegionOfInterest::ROI() {
        ROI_FILE_DEBUG << "Calculating the region of interest";
        cv::Mat img = frame.clone();
        cv::Mat mask = cv::Mat::zeros(image_height, image_width, CV_8UC1);

        // Calculate intersections with the image boundaries
        auto original_intersections = calculateExtendedIntersections(px_run, py_run, bottom_px_run, bottom_py_run);

        // Filter out invalid intersection points
        original_intersections.erase(
            std::remove_if(original_intersections.begin(), original_intersections.end(), [&](cv::Point p) {
                return !(p.x >= 0 && p.x < image_width && p.y >= 0 && p.y < image_height);
            }),
            original_intersections.end()
        );

        // Create a mask with the green area
        std::vector<cv::Point> mask_1_points = {
            {0, line_position_y_start},
            {image_width, line_position_y_end},
            {image_width, image_height},
            {0, image_height}
        };
        cv::fillPoly(mask, mask_1_points, cv::Scalar(255));
        cv::fillPoly(img, mask_1_points, cv::Scalar(0, 0, 0));

        cv::Point upper_endpoint(image_width, 0);
        cv::Point lower_endpoint(image_width, image_height);

        if (original_intersections.size() >= 2) {
            upper_endpoint = *std::min_element(original_intersections.begin(), original_intersections.end(), [](cv::Point a, cv::Point b) {
                return a.y < b.y;
            });
            lower_endpoint = *std::max_element(original_intersections.begin(), original_intersections.end(), [](cv::Point a, cv::Point b) {
                return a.y < b.y;
            });

            std::vector<cv::Point> mask_2_points = {
                {image_width, 0},
                {upper_endpoint.x, 0},
                upper_endpoint,
                lower_endpoint,
                {image_width, image_height}
            };
            cv::fillPoly(img, mask_2_points, cv::Scalar(0, 0, 0));

            auto intersection = findIntersection(original_intersections[0], original_intersections[1]);
            int x, y;
            if (intersection.x != -1) {
                x = std::max(upper_endpoint.x, intersection.x);
                y = std::max(intersection.y, line_position_y_start);
            } else {
                x = image_width;
                y = std::max(line_position_y_start, line_position_y_end);
            }

            if (x <= 0) x = 1;
            if (x >= image_width) x = image_width - 1;
            if (y <= 0) y = 1;
            if (y >= image_height) y = image_height - 1;

            cv::Rect roi_rect(0, 0, x, y);
            cv::Mat roi = img(roi_rect);

            return std::make_tuple(roi, mask, lower_endpoint, upper_endpoint);
        }
        ROI_FILE_DEBUG << "ROI completed";
        // Default ROI when fewer than 2 intersections
        return std::make_tuple(cv::Mat(), mask, lower_endpoint, upper_endpoint);
    }


