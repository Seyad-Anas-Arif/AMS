#include "fproc.h"
#include <QObject>
#include <QImage>
#include <QThread> 


FrameProcessor::FrameProcessor(QObject *parent)
    : QObject(parent), running(false)
{
    cap.open(0, cv::CAP_V4L2); // Open the default camera
}

FrameProcessor::~FrameProcessor()
{
    cap.release();
}

void FrameProcessor::processFrames()
{
    running = true;

    while (running)
    {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty())
        {
            emit error("Failed to capture frame.");
            continue;
        }

        // Convert BGR to RGB for displaying in QLabel
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        emit frameReady(qimg);

        QThread::msleep(300); // Sleep for 30ms to control the frame rate
    }
}

void FrameProcessor::stopProcessing()
{
    running = false;
}
