// 
#include <iostream>
#include <opencv2/opencv.hpp>
#include <QApplication>
#include "roi.h"
#include "ControlWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ControlWindow controlWindow;
    controlWindow.show();

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }

        cv::resize(frame, frame, cv::Size(image_width, image_height));
        cv::Mat roiFrame = ROI(frame);
        cv::imshow("After ROI", roiFrame);

        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return app.exec();
}
