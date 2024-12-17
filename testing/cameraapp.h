#ifndef CAMERAAPP_H
#define CAMERAAPP_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <opencv2/opencv.hpp>

class CameraApp : public QWidget {
    Q_OBJECT

public:
    CameraApp(QWidget *parent = nullptr);
    ~CameraApp();

private slots:
    void updateFrame();
    void captureImage();
    void displayCapturedImage();

private:
    QLabel *videoLabel;
    QLabel *imageLabel;
    QPushButton *shootButton;
    QPushButton *viewButton;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;

    QTimer *timer;
    cv::VideoCapture cap;
    cv::Mat currentFrame;
    cv::Mat capturedImage;

    void displayMat(cv::Mat &mat, QLabel *label);
};

#endif // CAMERAAPP_H
