#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>

class CalibrateFeed {
private:
    cv::Mat frame;

public:
    // Constructor to initialize the frame
    CalibrateFeed(const cv::Mat& inputFrame) : frame(inputFrame) {}

    // Calibration method
    cv::Mat calibrate() {
        // Path to the calibration data file
        std::string ymlPath = "camera_calibration.yml";

        // Variables to hold camera matrix and distortion coefficients
        cv::Mat cameraMatrix, distCoeffs;

        // Open the .yml file
        cv::FileStorage fs(ymlPath, cv::FileStorage::READ);
        if (!fs.isOpened()) {
            std::cerr << "Failed to open the calibration data file: " << ymlPath << std::endl;
            exit(EXIT_FAILURE);
        }

        // Read the calibration data from the .yml file
        fs["camera_matrix"] >> cameraMatrix;
        fs["distortion_coefficients"] >> distCoeffs;
        fs.release();

        // Get the dimensions of the frame
        int h = frame.rows;
        int w = frame.cols;

        // Get the optimal camera matrix for undistortion
        cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(w, h), 1, cv::Size(w, h));

        // Undistort the frame without cropping
        cv::Mat undistortedFrame;
        cv::undistort(frame, undistortedFrame, cameraMatrix, distCoeffs, newCameraMatrix);

        return undistortedFrame;
    }
};

int main() {
    // Load a sample frame (replace with your own image or video frame)
    cv::Mat frame = cv::imread("sample_image.jpg");
    if (frame.empty()) {
        std::cerr << "Error: Could not load the image." << std::endl;
        return -1;
    }

    // Create a CalibrateFeed object and calibrate the frame
    CalibrateFeed calibrator(frame);
    cv::Mat undistortedFrame = calibrator.calibrate();

    // Display the original and undistorted frames
    cv::imshow("Original Frame", frame);
    cv::imshow("Undistorted Frame", undistortedFrame);

    // Wait for a key press
    cv::waitKey(0);

    return 0;
}
