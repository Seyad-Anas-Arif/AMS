#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::FileStorage fs("camera_calibration.yml", cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Could not open or parse the file!" << std::endl;
        return -1;
    }

    cv::Mat cameraMatrix, distCoeffs;
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    fs.release();

    if (cameraMatrix.empty() || distCoeffs.empty()) {
        std::cerr << "Invalid data in the file!" << std::endl;
        return -1;
    }

    std::cout << "Camera Matrix: " << cameraMatrix << std::endl;
    std::cout << "Distortion Coefficients: " << distCoeffs << std::endl;

    return 0;
}
