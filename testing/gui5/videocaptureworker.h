#ifndef VIDEOCAPTUREWORKER_H
#define VIDEOCAPTUREWORKER_H

#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>

class VideoCaptureWorker : public QObject
{
    Q_OBJECT

public:
    explicit VideoCaptureWorker(int device = 0, int apiPreference = cv::CAP_V4L2, QObject *parent = nullptr);
    ~VideoCaptureWorker();

signals:
    void frameCaptured(const QImage &frame);

public slots:
    void process();

private:
    cv::VideoCapture cap;
    bool running;
};

#endif // VIDEOCAPTUREWORKER_H