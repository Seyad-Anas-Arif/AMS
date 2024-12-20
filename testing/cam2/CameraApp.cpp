#include "CameraApp.h"
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include <chrono>

CameraApp::CameraApp(QWidget *parent) : QWidget(parent) {
    stackedWidget = new QStackedWidget(this);

    createMainPage();
    createInspectionPage();

    stackedWidget->addWidget(page1);
    stackedWidget->addWidget(page2);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);

    setWindowTitle("Camera Calibration App");

    // Initialize camera thread
    stopFlag = false;
    cameraThread = std::thread(&CameraApp::captureFrames, this);
}

CameraApp::~CameraApp() {
    stopFlag = true;
    if (cameraThread.joinable()) {
        cameraThread.join();
    }
}

void CameraApp::createMainPage() {
    page1 = new QWidget(this);

    videoLabel = new QLabel(page1);
    videoLabel->setFixedSize(640, 480);

    gridRowsInput = new QSpinBox(page1);
    gridRowsInput->setValue(6);
    gridRowsInput->setFixedSize(60, 25);
    gridColsInput = new QSpinBox(page1);
    gridColsInput->setValue(9);
    gridColsInput->setFixedSize(60, 25);
    gridPitchInput = new QDoubleSpinBox(page1);
    gridPitchInput->setValue(29.0);
    gridPitchInput->setFixedSize(60, 25);
    maxSnapsInput = new QSpinBox(page1);
    maxSnapsInput->setValue(15);
    maxSnapsInput->setFixedSize(60, 25);

    maxSnapsStatus = new QLabel("", page1);

    setButton = new QPushButton("Set", page1);
    snapButton = new QPushButton("Snap", page1);
    inspectButton = new QPushButton("Inspect", page1);
    saveButton = new QPushButton("Save", page1);
    backButton = new QPushButton("Back", page1);

    connect(setButton, &QPushButton::clicked, this, &CameraApp::setSettings);
    connect(snapButton, &QPushButton::clicked, this, &CameraApp::snapImage);
    connect(inspectButton, &QPushButton::clicked, this, &CameraApp::goToInspectPage);
    connect(saveButton, &QPushButton::clicked, this, &CameraApp::saveCalibrationData);
    connect(backButton, &QPushButton::clicked, this, &CameraApp::closeApp);

    QFormLayout *gridLayout = new QFormLayout;
    gridLayout->addRow("Grid Rows:", gridRowsInput);
    gridLayout->addRow("Grid Columns:", gridColsInput);
    gridLayout->addRow("Grid Pitch (mm):", gridPitchInput);
    gridLayout->addRow("Max Snaps:", maxSnapsInput);
    gridLayout->addWidget(maxSnapsStatus);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(setButton);
    buttonLayout->addWidget(snapButton);
    buttonLayout->addWidget(inspectButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(backButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(page1);
    mainLayout->addWidget(videoLabel);
    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(buttonLayout);
}

// Placeholder methods for other pages
void CameraApp::createInspectionPage() {
    page2 = new QWidget(this);
    distortedLabel = new QLabel(page2);
    undistortedLabel = new QLabel(page2);

    QVBoxLayout *layout = new QVBoxLayout(page2);
    layout->addWidget(distortedLabel);
    layout->addWidget(undistortedLabel);
}

// Capture frames in a separate thread
void CameraApp::captureFrames() {
    cv::VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        qCritical("Error: Could not open the camera.");
        cv::VideoCapture cap(0, cv::CAP_V4L2);
        //return;
    }

    while (!stopFlag) {
        cv::Mat frame;
        cap >> frame;

        if (!frame.empty()) {
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

            QImage qFrame(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            QMetaObject::invokeMethod(this, [this, qFrame]() {
                videoLabel->setPixmap(QPixmap::fromImage(qFrame).scaled(videoLabel->size(), Qt::KeepAspectRatio));
            }, Qt::QueuedConnection);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // Control frame rate (~30fps)
    }

    cap.release();
}

void CameraApp::updateDistortedFrame(const QImage &frame)
{

}
void CameraApp::updateUndistortedFrame(const QImage &frame)
{

}
void CameraApp::saveCalibrationData()
{

}
void CameraApp::goToMainPage()
{

}
// Slot to update video frame (no longer needed as we handle updates in the thread)
void CameraApp::updateVideoFrame(const QImage &frame) {
    videoLabel->setPixmap(QPixmap::fromImage(frame));
}

void CameraApp::setSettings() {
    maxSnapsStatus->setText("Settings applied.");
}



void CameraApp::snapImage() {
    maxSnapsStatus->setText("Image snapped.");
}

void CameraApp::goToInspectPage() {
    stackedWidget->setCurrentIndex(1);
}

void CameraApp::closeApp() {
    close();
}
