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
        return;
    }

    while (!stopFlag) {
        cv::Mat frame;
        cap >> frame;

        if (!frame.empty()) {
            // Convert to grayscale for chessboard detection
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

            // Find chessboard corners
            std::vector<cv::Point2f> corners;
            bool found = cv::findChessboardCorners(gray, cv::Size(gridColsInput->value(), gridRowsInput->value()), corners);

            if (found) {
                // Draw the chessboard corners on the frame
                cv::drawChessboardCorners(frame, cv::Size(gridColsInput->value(), gridRowsInput->value()), corners, found);
            }

            // Convert to RGB for displaying in the QLabel
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
     if (calibrationDone && !cameraMatrix.empty() && !distCoeffs.empty())
    {
        cv::FileStorage fs("camera_calibration.yml", cv::FileStorage::WRITE);
        fs << "camera_matrix" << cameraMatrix;
        fs << "distortion_coefficients" << distCoeffs;
        fs.release();

        maxSnapsStatus->setText("Calibration parameters saved to camera_calibration.yml");
    }
    else
    {
        maxSnapsStatus->setText("Calibration has not been completed yet.");
    }

}
void CameraApp::goToMainPage()
{
    stackedWidget->setCurrentIndex(1);
    disconnect(timer, &QTimer::timeout, this, &CameraApp::updateFrame);
    connect(timer, &QTimer::timeout, this, &CameraApp::updateInspectionFrame);

}
// Slot to update video frame (no longer needed as we handle updates in the thread)
void CameraApp::updateVideoFrame(const QImage &frame) {
    videoLabel->setPixmap(QPixmap::fromImage(frame));
}

void CameraApp::setSettings() { if (setButton->text() == "Set")
    {
        gridRowsInput->setEnabled(false);
        gridColsInput->setEnabled(false);
        gridPitchInput->setEnabled(false);
        maxSnapsInput->setEnabled(false);

        snapButton->setEnabled(true);
        maxSnapsStatus->setText("Settings applied. You can start snapping.");
        setButton->setText("Reset");
    }
    else
    {
        gridRowsInput->setValue(6);
        gridColsInput->setValue(9);
        gridPitchInput->setValue(10);
        maxSnapsInput->setValue(40);

        gridRowsInput->setEnabled(true);
        gridColsInput->setEnabled(true);
        gridPitchInput->setEnabled(true);
        maxSnapsInput->setEnabled(true);

        snapsTaken = 0;
        objPoints.clear();
        imgPoints.clear();
        snapButton->setEnabled(false);
        inspectButton->setEnabled(false);
        saveButton->setEnabled(false);
        maxSnapsStatus->setText("Settings reset. You can modify values and start snapping again.");
        setButton->setText("Set");
    }
}



void CameraApp::snapImage() {
     if (snapsTaken >= maxSnapsInput->value())
    {
        maxSnapsStatus->setText("You have reached the maximum number of snaps.");
        inspectButton->setEnabled(true);
        saveButton->setEnabled(true);
        snapButton->setEnabled(false);
        return;
    }

    cv::Mat frame;
    cap >> frame;
    if (!frame.empty())
    {
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(gray, cv::Size(gridColsInput->value(), gridRowsInput->value()), corners);

        if (found)
        {
            cv::Mat objp = cv::Mat::zeros(gridRowsInput->value() * gridColsInput->value(), 3, CV_32F);
            for (int i = 0; i < gridColsInput->value(); ++i)
            {
                for (int j = 0; j < gridRowsInput->value(); ++j)
                {
                    objp.at<float>(i * gridRowsInput->value() + j, 0) = i * gridPitchInput->value();
                    objp.at<float>(i * gridRowsInput->value() + j, 1) = j * gridPitchInput->value();
                }
            }

            objPoints.push_back(objp);
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));
            imgPoints.push_back(corners);

            snapsTaken++;
            cv::drawChessboardCorners(frame, cv::Size(gridColsInput->value(), gridRowsInput->value()), corners, found);
            maxSnapsStatus->setText(QString("Snap %1 captured successfully.").arg(snapsTaken));

            if (snapsTaken < maxSnapsInput->value())
            {
                inspectButton->setEnabled(false);
                saveButton->setEnabled(false);
            }
        }
        else
        {
            maxSnapsStatus->setText("Checkerboard not detected in the frame.");
        }
    }
}

void CameraApp::goToInspectPage() {
    stackedWidget->setCurrentIndex(1);
    disconnect(timer, &QTimer::timeout, this, &CameraApp::updateFrame);
    connect(timer, &QTimer::timeout, this, &CameraApp::updateInspectionFrame);
}

void CameraApp::closeApp() {
    close();
}
