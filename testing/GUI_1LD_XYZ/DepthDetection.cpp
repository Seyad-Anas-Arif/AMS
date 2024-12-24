#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

using namespace std;

// Define a structure to hold 3D points
struct Point3D {
    double X, Y, Z; // X, Y, Z coordinates of the point
};

// Bresenham's algorithm to generate pixel coordinates along a line
vector<pair<int, int>> bresenham_line(int x1, int y1, int x2, int y2) {
    vector<pair<int, int>> points;
    int dx = abs(x2 - x1); // Difference in x
    int dy = abs(y2 - y1); // Difference in y
    int sx = (x1 < x2) ? 1 : -1; // Step direction for x
    int sy = (y1 < y2) ? 1 : -1; // Step direction for y
    int err = dx - dy; // Error term

    while (true) {
        points.emplace_back(x1, y1); // Add the current point to the list
        if (x1 == x2 && y1 == y2) break; // Stop if the end point is reached
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }

    return points; // Return the list of points along the line
}

class XYZ {
private:
    int x1, y1, x2, y2; // Start and end points of the laser line in pixels
    double pixel_size_mm; // Size of each pixel in mm
    double focal_length_mm_x, focal_length_mm_y; // Focal lengths in mm
    double principal_point_mm_x, principal_point_mm_y; // Principal points in mm
    double baseline; // Distance between the two cameras in mm
    double alpha; // Camera tilt angle in radians

public:
    // Constructor to initialize the XYZ class with necessary parameters
    XYZ(int x1, int y1, int x2, int y2, double pixel_size_mm,
        double focal_length_mm_x, double focal_length_mm_y,
        double principal_point_mm_x, double principal_point_mm_y,
        double baseline, double alpha)
        : x1(x1), y1(y1), x2(x2), y2(y2), pixel_size_mm(pixel_size_mm),
          focal_length_mm_x(focal_length_mm_x), focal_length_mm_y(focal_length_mm_y),
          principal_point_mm_x(principal_point_mm_x), principal_point_mm_y(principal_point_mm_y),
          baseline(baseline), alpha(alpha) {}

    // Calculate the 3D depth information based on pixel coordinates
    vector<Point3D> depth() {
        vector<pair<int, int>> laser_pixel_coordinates = bresenham_line(x1, y1, x2, y2); // Get pixel coordinates along the laser line
        vector<Point3D> laser_points_3D;

        for (const auto &pixel : laser_pixel_coordinates) {
            int x_p = pixel.first; // x-coordinate of the pixel
            int y_p = pixel.second; // y-coordinate of the pixel

            // Convert pixel coordinates to real-world coordinates
            double x = x_p * pixel_size_mm; // Convert x pixel to mm
            double y = y_p * pixel_size_mm; // Convert y pixel to mm
            double p = x - principal_point_mm_x; // Distance from principal point in x-direction
            double beta = atan(p / focal_length_mm_x); // Angle beta based on the pixel distance
            
            // Calculate Z-coordinate (depth) using triangulation formula
            double Z = cos(alpha) * (baseline * tan(alpha) + (baseline * (1 / tan(alpha)) / tan(alpha - beta)));

            // Calculate Y and X coordinates based on Z
            double Y = Z * (y - principal_point_mm_y) / focal_length_mm_y;
            double X = Z * (x - principal_point_mm_x) / focal_length_mm_x;

            Point3D point_3D = {X, Y, Z}; // Create a 3D point
            laser_points_3D.push_back(point_3D); // Add to the list of 3D points
        }

        return laser_points_3D; // Return the list of 3D points
    }
};

int main() {
    // Camera calibration parameters
    double fx = 1000.0; // Example value for focal length along x-axis in pixels
    double fy = 1000.0; // Example value for focal length along y-axis in pixels
    double cx = 960.0;  // Principal point x-coordinate in pixels
    double cy = 600.0;  // Principal point y-coordinate in pixels

    // Sensor specifications
    int sensor_width_pixels = 1920; // Width of the sensor in pixels
    int sensor_height_pixels = 1200; // Height of the sensor in pixels
    double pixel_size_mm = 0.003; // Pixel size in mm

    // Calculate sensor dimensions and real-world focal length
    double sensor_width_mm = sensor_width_pixels * pixel_size_mm; // Sensor width in mm
    double sensor_height_mm = sensor_height_pixels * pixel_size_mm; // Sensor height in mm
    double focal_length_mm_x = fx * pixel_size_mm; // Focal length in mm (x-axis)
    double focal_length_mm_y = fy * pixel_size_mm; // Focal length in mm (y-axis)
    double principal_point_mm_x = cx * pixel_size_mm; // Principal point in mm (x-axis)
    double principal_point_mm_y = cy * pixel_size_mm; // Principal point in mm (y-axis)

    // Triangulation parameters
    double baseline = 88.60; // Baseline in mm
    double alpha = M_PI / 6; // Camera tilt angle converted from 30 degrees to radians

    // Initialize the XYZ class with input parameters
    XYZ xyz(0, 0, 100, 100, pixel_size_mm, focal_length_mm_x, focal_length_mm_y,
            principal_point_mm_x, principal_point_mm_y, baseline, alpha);

    // Calculate depth to generate 3D points
    vector<Point3D> laser_points_3D = xyz.depth();

    // Output the 3D points
    for (const auto &point : laser_points_3D) {
        cout << "X: " << point.X << ", Y: " << point.Y << ", Z: " << point.Z << endl; // Print each 3D point
    }

    return 0; // End of program
}


/* ------------------------------------Alternative -------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>

class XYZ {
public:
    int x1, y1, x2, y2;

    XYZ(int x1, int y1, int x2, int y2) {
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
    }

    std::vector<cv::Point3f> depth(float baseline, float alpha, float fx, float fy, float cx, float cy, float pixel_size_mm, float focal_length_mm_x, float focal_length_mm_y, float principal_point_mm_x, float principal_point_mm_y) {
        std::vector<cv::Point3f> laser_points_3D;
        
        std::vector<cv::Point> laser_pixel_coordinates = bresenham_line(x1, y1, x2, y2);

        for (const auto& point : laser_pixel_coordinates) {
            int x_p = point.x;
            int y_p = point.y;

            // Convert pixel coordinates (x', y') to real-world coordinates using provided equations
            float x = x_p * pixel_size_mm;
            float y = y_p * pixel_size_mm;
            float p = x - principal_point_mm_x;
            float beta = std::atan(p / focal_length_mm_x);
            float Z = std::cos(alpha) * (baseline * std::tan(alpha) + (baseline * (1 / std::tan(alpha)) / std::tan(alpha - beta)));
            float Y = Z * (y - principal_point_mm_y) / focal_length_mm_y;
            float X = Z * (x - principal_point_mm_x) / focal_length_mm_x;

            laser_points_3D.push_back(cv::Point3f(X, Y, Z));
        }

        return laser_points_3D;
    }

private:
    // Bresenham's algorithm to generate pixel coordinates along a line from (x1, y1) to (x2, y2)
    std::vector<cv::Point> bresenham_line(int x1, int y1, int x2, int y2) {
        std::vector<cv::Point> points;
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            points.push_back(cv::Point(x1, y1));
            if (x1 == x2 && y1 == y2) {
                break;
            }
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y1 += sy;
            }
        }

        return points;
    }
};

int main() {
    // Camera calibration data (replace with actual file path)
    std::string excel_path = "camera_calibration_results.xlsx";
    
    // Load the camera calibration matrix and distortion coefficients from a file
    std::ifstream calibration_file(excel_path);
    std::string line;
    std::string camera_matrix_str, dist_coeffs_str;
    
    if (calibration_file.is_open()) {
        std::getline(calibration_file, line);
        camera_matrix_str = line;
        std::getline(calibration_file, line);
        dist_coeffs_str = line;
    }
    
    // Parse the camera matrix and distortion coefficients
    std::stringstream ss_camera_matrix(camera_matrix_str);
    std::vector<float> camera_matrix_vals;
    std::string val;
    while (std::getline(ss_camera_matrix, val, ',')) {
        camera_matrix_vals.push_back(std::stof(val));
    }

    std::stringstream ss_dist_coeffs(dist_coeffs_str);
    std::vector<float> dist_coeffs_vals;
    while (std::getline(ss_dist_coeffs, val, ',')) {
        dist_coeffs_vals.push_back(std::stof(val));
    }

    // Create the camera matrix
    cv::Mat camera_matrix(3, 3, CV_32F, camera_matrix_vals.data());
    cv::Mat dist_coeffs(1, dist_coeffs_vals.size(), CV_32F, dist_coeffs_vals.data());

    // Extract focal length and principal points
    float fx = camera_matrix.at<float>(0, 0);  // Focal length along the x-axis in pixels
    float fy = camera_matrix.at<float>(1, 1);  // Focal length along the y-axis in pixels
    float cx = camera_matrix.at<float>(0, 2);
    float cy = camera_matrix.at<float>(1, 2);

    // Sensor specifications
    int sensor_width_pixels = 1920;
    int sensor_height_pixels = 1200;
    float pixel_size_mm = 0.003f;  // Pixel size in mm

    // Calculate sensor dimensions
    float sensor_width_mm = sensor_width_pixels * pixel_size_mm;
    float sensor_height_mm = sensor_height_pixels * pixel_size_mm;
    float focal_length_mm_x = fx * pixel_size_mm;
    float focal_length_mm_y = fy * pixel_size_mm;
    float principal_point_mm_x = cx * pixel_size_mm;
    float principal_point_mm_y = cy * pixel_size_mm;

    // Define triangulation parameters
    float baseline = 88.60f;  // Baseline (mm)
    float alpha = 30.0f * M_PI / 180.0f;  // Camera tilt angle (degrees to radians)

    // Example of usage with XYZ class
    XYZ xyz(100, 200, 300, 400);  // Example pixel coordinates
    std::vector<cv::Point3f> points_3D = xyz.depth(baseline, alpha, fx, fy, cx, cy, pixel_size_mm, focal_length_mm_x, focal_length_mm_y, principal_point_mm_x, principal_point_mm_y);

    // Output the 3D points
    for (const auto& point : points_3D) {
        std::cout << "3D Point: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
    }

    return 0;
}
*/