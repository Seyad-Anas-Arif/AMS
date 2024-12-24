#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

class RegionOfInterest {
public:
    RegionOfInterest(cv::Mat frame, int line_position_y_start, int line_position_y_end, cv::Point point_position, 
                     cv::Point bottom_point_position, int px_run, int py_run, int bottom_px_run, int offset)
        : frame(frame), offset(offset), line_position_y_start(line_position_y_start), line_position_y_end(line_position_y_end),
          point_position(point_position), bottom_point_position(bottom_point_position), px_run(px_run), py_run(py_run),
          bottom_px_run(bottom_px_run) {
        image_height = frame.rows;
        image_width = frame.cols;
        bottom_py_run = image_height;
    }

    std::vector<cv::Point> calculate_extended_intersections(int start_x, int start_y, int end_x, int end_y) {
        std::vector<cv::Point> intersections;

        double slope = (end_x != start_x) ? (static_cast<double>(end_y - start_y) / (end_x - start_x)) : std::numeric_limits<double>::infinity();

        if (slope != std::numeric_limits<double>::infinity()) {
            // Intersection with the left boundary (x = 0)
            double y_at_x0 = start_y - slope * (start_x + offset);
            if (y_at_x0 >= 0 && y_at_x0 < image_height) {
                intersections.emplace_back(0, static_cast<int>(y_at_x0));
            }

            // Intersection with the right boundary (x = image_width - 1)
            double y_at_x_max = start_y + slope * (image_width - (start_x + offset) - 1);
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
            intersections.emplace_back(start_x + offset, 0);
            intersections.emplace_back(start_x + offset, image_height - 1);
        }

        // Ensure distinct points
        std::vector<cv::Point> unique_intersections;
        for (const auto& point : intersections) {
            if (std::find(unique_intersections.begin(), unique_intersections.end(), point) == unique_intersections.end()) {
                unique_intersections.push_back(point);
            }
        }

        return unique_intersections;
    }

    cv::Point find_intersection(const cv::Point& point_1, const cv::Point& point_2) {
        const double epsilon = 1e-6; // Small value to check for parallel lines

        double x1 = point_1.x, y1 = point_1.y;
        double x2 = point_2.x, y2 = point_2.y;

        double m_y, c_y;
        if (x1 == x2) {
            m_y = std::numeric_limits<double>::infinity();
            c_y = 0;
        } else {
            m_y = (y2 - y1) / (x2 - x1);
            c_y = y1 - m_y * x1;
        }

        double m_w = static_cast<double>(line_position_y_end - line_position_y_start) / (image_width - 1);
        double c_w = line_position_y_start;

        if (std::abs(m_y - m_w) < epsilon) {
            return cv::Point(-1, -1); // Parallel lines
        }

        double x_intersect, y_intersect;
        if (m_y == std::numeric_limits<double>::infinity()) {
            x_intersect = x1;
            y_intersect = m_w * x_intersect + c_w;
        } else if (m_w == std::numeric_limits<double>::infinity()) {
            x_intersect = 0;
            y_intersect = m_y * x_intersect + c_y;
        } else {
            x_intersect = (c_w - c_y) / (m_y - m_w);
            y_intersect = m_y * x_intersect + c_y;
        }

        if (x_intersect >= 0 && x_intersect < image_width && y_intersect >= 0 && y_intersect < image_height) {
            return cv::Point(static_cast<int>(x_intersect), static_cast<int>(y_intersect));
        }

        return cv::Point(-1, -1); // No valid intersection
    }

    std::tuple<cv::Mat, cv::Mat, cv::Point, cv::Point> ROI() {
        cv::Mat img = frame.clone();

        // Calculate intersections with the image boundaries
        auto original_intersections = calculate_extended_intersections(px_run, py_run, bottom_px_run, bottom_py_run);

        // Filter out invalid intersection points
        original_intersections.erase(std::remove_if(original_intersections.begin(), original_intersections.end(),
            [this](const cv::Point& point) {
                return point.x < 0 || point.x >= image_width || point.y < 0 || point.y >= image_height;
            }), original_intersections.end());

        // Create a mask
        cv::Mat mask = cv::Mat::zeros(image_height, image_width, CV_8UC1);
        std::vector<cv::Point> mask_1_points = {
            {0, line_position_y_start},
            {image_width, line_position_y_end},
            {image_width, image_height},
            {0, image_height}
        };
        cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{mask_1_points}, cv::Scalar(255));

        cv::Point upper_endpoint(image_width, 0), lower_endpoint(image_width, image_height);

        if (original_intersections.size() >= 2) {
            upper_endpoint = *std::min_element(original_intersections.begin(), original_intersections.end(), [](const cv::Point& a, const cv::Point& b) {
                return a.y < b.y;
            });
            lower_endpoint = *std::max_element(original_intersections.begin(), original_intersections.end(), [](const cv::Point& a, const cv::Point& b) {
                return a.y < b.y;
            });

            std::vector<cv::Point> mask_2_points = {
                {image_width, 0},
                {upper_endpoint.x, 0},
                upper_endpoint,
                lower_endpoint,
                {image_width, image_height}
            };
            cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{mask_2_points}, cv::Scalar(0));

            auto intersection = find_intersection(original_intersections[0], original_intersections[1]);
            if (intersection.x != -1 && intersection.y != -1) {
                upper_endpoint.x = std::max(upper_endpoint.x, intersection.x);
                upper_endpoint.y = std::max(intersection.y, line_position_y_start);
            }
        }

        int x = upper_endpoint.x;
        int y = upper_endpoint.y;
        x = std::clamp(x, 1, image_width - 1);
        y = std::clamp(y, 1, image_height - 1);

        cv::Mat roi = img(cv::Rect(0, 0, x, y));
        return {roi, mask, lower_endpoint, upper_endpoint};
    }

private:
    cv::Mat frame;
    int offset;
    int line_position_y_start;
    int line_position_y_end;
    cv::Point point_position;
    cv::Point bottom_point_position;
    int px_run;
    int py_run;
    int bottom_px_run;
    int bottom_py_run;
    int image_height;
    int image_width;
};
