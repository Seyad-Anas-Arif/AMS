#include <iostream>
#include <opencv2/opencv.hpp>
#include "LaserDetection.h"

int main() {
    cv::VideoCapture cap(0, cv::CAP_V4L2);

    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open the video feed." << std::endl;
        return -1;
    }

    //cv::namedWindow("Laser Detection", cv::WINDOW_NORMAL);

    while (true) {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "Error: Unable to capture frame from video feed." << std::endl;
            break;
        } else {
            LaserDetection laser_detection(frame);
            auto [laser, x1, y1, x2, y2] = laser_detection.laserDetection();

            cv::imshow("Laser Detection", laser);

            if (cv::waitKey(1) == 27) {
                break;
            }
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}