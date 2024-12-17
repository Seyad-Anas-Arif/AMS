#include "cameraapp.h"
#include <QApplication>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QTimerEvent>

CameraApp::CameraApp(QWidget *parent) 
: QWidget(parent), 
calibrationDone(false), 
snapsTaken(0) 
{
    gridRows = 6;
    gridCols = 9;
    gridPitch = 29;
    maxSnaps = 15;
//cap("v4l2src device=/dev/video2 ! videoconvert ! appsink", cv::CAP_GSTREAMER)
    initUI();

     {
    // if (!cap.isOpened()) {
    //     std::cerr << "Error: Could not open video stream using GStreamer pipeline!" << std::endl;
    // }
}
    cap.open(0); // Open the default camera
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CameraApp::updateFrame);
    timer->start(30);
}

CameraApp::~CameraApp()
{
    cap.release();
}

void CameraApp::initUI()
{
    stackedWidget = new QStackedWidget(this);

    page1 = new QWidget(this);
    createMainPage();

    page2 = new QWidget(this);
    createInspectionPage();

    stackedWidget->addWidget(page1);
    stackedWidget->addWidget(page2);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);

    setWindowTitle("Camera Calibration App");
}

void CameraApp::createMainPage()
{
    videoLabel = new QLabel(page1);
    videoLabel->setFixedSize(640, 480);

    gridRowsInput = new QSpinBox(page1);
    gridRowsInput->setValue(gridRows);
    gridRowsInput->setFixedSize(60, 25);

    gridColsInput = new QSpinBox(page1);
    gridColsInput->setValue(gridCols);
    gridColsInput->setFixedSize(60, 25);

    gridPitchInput = new QDoubleSpinBox(page1);
    gridPitchInput->setValue(gridPitch);
    gridPitchInput->setFixedSize(60, 25);

    maxSnapsInput = new QSpinBox(page1);
    maxSnapsInput->setValue(maxSnaps);
    maxSnapsInput->setFixedSize(60, 25);

    maxSnapsStatus = new QLabel("", page1);

    setButton = new QPushButton("Set", page1);
    connect(setButton, &QPushButton::clicked, this, &CameraApp::setSettings);

    snapButton = new QPushButton("Snap", page1);
    snapButton->setEnabled(false);
    connect(snapButton, &QPushButton::clicked, this, &CameraApp::snapImage);

    inspectButton = new QPushButton("Inspect", page1);
    inspectButton->setEnabled(false);
    connect(inspectButton, &QPushButton::clicked, this, &CameraApp::goToInspectPage);

    saveButton = new QPushButton("Save", page1);
    saveButton->setEnabled(false);
    connect(saveButton, &QPushButton::clicked, this, &CameraApp::saveCalibrationData);

    backButton = new QPushButton("Back", page1);
    connect(backButton, &QPushButton::clicked, this, &CameraApp::closeApp);

    QFormLayout *gridLayout = new QFormLayout;
    gridLayout->addRow("Grid Rows:", gridRowsInput);
    gridLayout->addRow("Grid Columns:", gridColsInput);
    gridLayout->addRow("Grid Pitch (mm):", gridPitchInput);
    gridLayout->addRow("Max Snaps:", maxSnapsInput);
    gridLayout->addWidget(maxSnapsStatus);
    gridLayout->setAlignment(maxSnapsStatus, Qt::AlignRight);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(setButton);
    buttonLayout->addWidget(snapButton);
    buttonLayout->addWidget(inspectButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(backButton);

    QVBoxLayout *page1Layout = new QVBoxLayout(page1);
    page1Layout->addWidget(videoLabel);
    page1Layout->addLayout(gridLayout);
    page1Layout->addLayout(buttonLayout);
}

void CameraApp::createInspectionPage()
{
    distortedLabel = new QLabel(page2);
    distortedLabel->setFixedSize(320, 240);

    undistortedLabel = new QLabel(page2);
    undistortedLabel->setFixedSize(320, 240);

    backToMainButton = new QPushButton("Back", page2);
    connect(backToMainButton, &QPushButton::clicked, this, &CameraApp::goToMainPage);

    QHBoxLayout *inspectionLayout = new QHBoxLayout(page2);
    inspectionLayout->addWidget(distortedLabel);
    inspectionLayout->addWidget(undistortedLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(backToMainButton);

    QVBoxLayout *page2Layout = new QVBoxLayout(page2);
    page2Layout->addLayout(inspectionLayout);
    page2Layout->addLayout(buttonLayout);
}

void CameraApp::updateFrame()
{
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
            cv::drawChessboardCorners(frame, cv::Size(gridColsInput->value(), gridRowsInput->value()), corners, found);
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        displayImage(frame, videoLabel);
    }
}

void CameraApp::displayImage(const cv::Mat &img, QLabel *label)
{
    QImage qimg(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(qimg);
    label->setPixmap(pixmap);
}

void CameraApp::setSettings()
{
    if (setButton->text() == "Set")
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

void CameraApp::snapImage()
{
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

void CameraApp::goToInspectPage()
{
    stackedWidget->setCurrentIndex(1);
    disconnect(timer, &QTimer::timeout, this, &CameraApp::updateFrame);
    connect(timer, &QTimer::timeout, this, &CameraApp::updateInspectionFrame);
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

void CameraApp::updateInspectionFrame()
{
    if (!calibrationDone)
    {
        if (objPoints.empty() || imgPoints.empty())
        {
            maxSnapsStatus->setText("No calibration data available. Please take some snaps first.");
            return;
        }

        cv::Mat frame;
        cap >> frame;
        if (!frame.empty())
        {
            cv::Size imageSize = frame.size();
            std::vector<cv::Mat> rvecs, tvecs;
            bool success = cv::calibrateCamera(objPoints, imgPoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);

            if (success)
            {
                newCameraMtx = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, nullptr);
                calibrationDone = true;
                maxSnapsStatus->setText("Calibration successful. Showing undistorted frames.");
            }
            else
            {
                maxSnapsStatus->setText("Calibration failed. Please take more snaps.");
                return;
            }
        }
    }

    cv::Mat frame, undistorted;
    cap >> frame;
    if (!frame.empty())
    {
        cv::undistort(frame, undistorted, cameraMatrix, distCoeffs, newCameraMtx);

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        cv::cvtColor(undistorted, undistorted, cv::COLOR_BGR2RGB);

        displayImage(frame, distortedLabel);
        displayImage(undistorted, undistortedLabel);
    }
}

void CameraApp::goToMainPage()
{
    stackedWidget->setCurrentIndex(0);
    disconnect(timer, &QTimer::timeout, this, &CameraApp::updateInspectionFrame);
    connect(timer, &QTimer::timeout, this, &CameraApp::updateFrame);
}

void CameraApp::closeApp()
{
    cap.release();
    close();
}
