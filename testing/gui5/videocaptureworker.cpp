#include "videocaptureworker.h"
#include <QImage>

VideoCaptureWorker::VideoCaptureWorker(int device, int apiPreference, QObject *parent) :
    QObject(parent),
    running(true),
    cap(device, apiPreference) // Initialize VideoCapture with device and API preference
{
    if (!cap.isOpened()) {
        // Handle error if the camera could not be opened
        running = false;
    }
}

VideoCaptureWorker::~VideoCaptureWorker()
{
    running = false;
    cap.release();
}

void VideoCaptureWorker::process()
{
    while (running) {
        cv::Mat frame;
        cap >> frame; // Capture a new frame from the camera

        if (frame.empty()) {
            continue; // Handle empty frame
        }

        // Convert the frame to RGB format
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        // Convert the frame to QImage
        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

        // Emit the frameCaptured signal
        emit frameCaptured(qimg);

        QThread::msleep(30); // Sleep for 30 ms (approx. 33 FPS)
    }
}