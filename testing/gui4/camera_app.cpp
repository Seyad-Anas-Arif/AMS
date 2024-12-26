// this application for gui only i tried using copilot but its given only out line 
#include "camera_app.h"

CameraThread::CameraThread() : running(true), targetHeight(240), targetWidth(320), exposureValue(-5), offsetLineValue(130), plotPoint(false) {}

void CameraThread::run() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        return;
    }

    while (running) {
        cap.set(cv::CAP_PROP_EXPOSURE, exposureValue);

        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) {
            break;
        }

        // Process the frame (similar to the Python code)
        // ...

        // Emit the processed image
        QImage qimage = QImage((const unsigned char*)frame.data, frame.cols, frame.rows, QImage::Format_RGB888);
        emit imageMain(qimage);
    }

    cap.release();
}

void CameraThread::stop() {
    running = false;
}

void CameraThread::updateExposure(int value) {
    exposureValue = value;
}

void CameraThread::updateOffsetLineValue(int value) {
    offsetLineValue = value;
}