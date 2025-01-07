#include "../inc/DepthDetection.h"

// Initialize static members
cv::Mat XYZ::cameraMatrix;
cv::Mat XYZ::distCoeffs;
double XYZ::fx = 0, XYZ::fy = 0, XYZ::cx = 0, XYZ::cy = 0;
double XYZ::pixelSizeMM = 0.003;
double XYZ::sensorWidthMM = 0, XYZ::sensorHeightMM = 0;
double XYZ::focalLengthMMX = 0, XYZ::focalLengthMMY = 0;
double XYZ::principalPointMMX = 0, XYZ::principalPointMMY = 0;
double XYZ::baseline = 88.60;
double XYZ::alpha = M_PI / 6.0; // 30 degrees in radians

XYZ::XYZ(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

// Load camera calibration data from a .yml file
void XYZ::loadCalibrationData(const std::string &calibrationFile) {
    cv::FileStorage fs(calibrationFile, cv::FileStorage::READ);

    if (!fs.isOpened()) {
        throw std::runtime_error("Failed to open calibration file: " + calibrationFile);
    }

    fs["CameraMatrix"] >> cameraMatrix;
    fs["DistortionCoefficients"] >> distCoeffs;

    fx = cameraMatrix.at<double>(0, 0);
    fy = cameraMatrix.at<double>(1, 1);
    cx = cameraMatrix.at<double>(0, 2);
    cy = cameraMatrix.at<double>(1, 2);

    sensorWidthMM = cameraMatrix.cols * pixelSizeMM;
    sensorHeightMM = cameraMatrix.rows * pixelSizeMM;
    focalLengthMMX = fx * pixelSizeMM;
    focalLengthMMY = fy * pixelSizeMM;
    principalPointMMX = cx * pixelSizeMM;
    principalPointMMY = cy * pixelSizeMM;

    fs.release();
}

// Bresenham's algorithm to generate pixel coordinates along a line
std::vector<cv::Point> XYZ::bresenhamLine(int x1, int y1, int x2, int y2) {
    std::vector<cv::Point> points;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        points.emplace_back(x1, y1);
        if (x1 == x2 && y1 == y2)
            break;

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

// Calculate 3D depth
std::vector<cv::Point3f> XYZ::depth() {
    auto laserPixelCoordinates = bresenhamLine(x1, y1, x2, y2);
    std::vector<cv::Point3f> laserPoints3D;

    for (const auto &pixel : laserPixelCoordinates) {
        double x_p = pixel.x * pixelSizeMM;
        double y_p = pixel.y * pixelSizeMM;

        double p = x_p - principalPointMMX;
        double beta = atan(p / focalLengthMMX);
        double Z = cos(alpha) * (baseline * tan(alpha) + (baseline * (1 / tan(alpha)) / tan(alpha - beta)));
        double Y = Z * (y_p - principalPointMMY) / focalLengthMMY;
        double X = Z * (x_p - principalPointMMX) / focalLengthMMX;

        laserPoints3D.emplace_back(X, Y, Z);
    }

    return laserPoints3D;
}
