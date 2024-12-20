#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>

class FrameProcessor : public QObject
{
    Q_OBJECT

public:
    FrameProcessor(QObject *parent = nullptr);
    ~FrameProcessor();

public slots:
    void processFrames();       // Slot to start processing frames
    void stopProcessing();      // Slot to stop processing frames

signals:
    void frameReady(const QImage &frame);  // Signal emitted when a frame is ready
    void error(const QString &errorMsg);   // Signal emitted in case of error

private:
    cv::VideoCapture cap;
    bool running;
};

#endif // FRAMEPROCESSOR_H
