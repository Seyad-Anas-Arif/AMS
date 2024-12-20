#include <QApplication>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPixmap>
#include <thread>
#include <atomic>
#include <chrono>

// Function to convert cv::Mat to QImage
QImage matToQImage(const cv::Mat &mat) {
    if (mat.channels() == 3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage((const unsigned char *)rgb.data, rgb.cols, rgb.rows, QImage::Format_RGB888);
    } else if (mat.channels() == 1) {
        return QImage((const unsigned char *)mat.data, mat.cols, mat.rows, QImage::Format_Grayscale8);
    } else {
        return QImage();
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create QLabel for video display
    QLabel label;
    label.setWindowTitle("Live Camera Feed");
    label.resize(800, 600);
    label.show();

    std::atomic<bool> stopFlag(false);

    // Thread to capture frames
    std::thread cameraThread([&label, &stopFlag]() {
        cv::VideoCapture cap(0, cv::CAP_V4L2);
        if (!cap.isOpened()) {
            qCritical("Error: Could not open the camera.");
            return;
        }

        while (!stopFlag) {
            cv::Mat frame;
            cap >> frame;

            if (!frame.empty()) {
                QImage qimg = matToQImage(frame);

                // Use Qt's event loop to safely update QLabel
                QMetaObject::invokeMethod(&label, [&label, qimg]() {
                    label.setPixmap(QPixmap::fromImage(qimg).scaled(label.size(), Qt::KeepAspectRatio));
                }, Qt::QueuedConnection);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(30)); // Simulate ~30fps
        }

        cap.release();
    });

    // Stop the thread when the application is about to quit
    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
        stopFlag = true;
        if (cameraThread.joinable()) {
            cameraThread.join();
        }
    });

    return app.exec();
}
