#include "../inc/CalibrateFeed.h"

CalibrateFeed::CalibrateFeed(const cv::Mat& inputFrame) : frame(inputFrame) {}

cv::Mat CalibrateFeed::calibrate(const std::string& calibrationFilePath) {
    // Load camera calibration data from the YAML file
    //qDebug() << "Calibrating the feed";
    cv::FileStorage fs(calibrationFilePath, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        throw std::runtime_error("Could not open calibration file: " + calibrationFilePath);
    }

    // Read the camera matrix and distortion coefficients
    cv::Mat cameraMatrix, distCoeffs;
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    fs.release();

    if (cameraMatrix.empty() || distCoeffs.empty()) {
        throw std::runtime_error("Invalid calibration data in file: " + calibrationFilePath);
    }

    int h = frame.rows;
    int w = frame.cols;

    // Get the optimal new camera matrix for undistortion
    cv::Mat newCameraMatrix;
    cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(w, h), 1, cv::Size(w, h), nullptr);

    // Undistort the current frame without cropping
    cv::Mat undistortedFrame;
    cv::undistort(frame, undistortedFrame, cameraMatrix, distCoeffs, newCameraMatrix);

    return undistortedFrame;
}
