#include "CameraApp.h"
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
#include <thread>


CameraApp::CameraApp(QWidget *parent) 
: QWidget(parent), 
calibrationDone(false), 
snapsTaken(0) 
{
    
    initUI();
    stopFlag = false;
    PauseFlag = false;
    cameraThread = std::thread(&CameraApp::updateFrame, this);
    

    // cap.open(0, cv::CAP_V4L2); // Open the default camera
    // timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, this, &CameraApp::updateFrame);
    // timer->start(30);
}

CameraApp::~CameraApp()
{
     stopFlag = true;
    if (cameraThread.joinable()) {
        cameraThread.join();
    }
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
    initgridRows = 6;
    initgridCols = 9;
    initgridPitch = 29;
    initmaxSnaps = 20;

    videoLabel = new QLabel(page1);
    videoLabel->setFixedSize(640, 480);

    gridRowsInput = new QSpinBox(page1);
    gridRowsInput->setValue(initgridRows);
    gridRowsInput->setFixedSize(60, 25);

    gridColsInput = new QSpinBox(page1);
    gridColsInput->setValue(initgridCols);
    gridColsInput->setFixedSize(60, 25);

    gridPitchInput = new QDoubleSpinBox(page1);
    gridPitchInput->setValue(initgridPitch);
    gridPitchInput->setFixedSize(60, 25);

    maxSnapsInput = new QSpinBox(page1);
    maxSnapsInput->setValue(initmaxSnaps);
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
   cv::VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        qCritical("Error: Could not open the camera.");
        return;
    }

    while (!stopFlag) {
        cv::Mat frame;
        cap >> frame;
        if (!PauseFlag){

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
    }

    cap.release();
}
//completed  untill tread

void CameraApp::displayImage(const cv::Mat &img, QLabel *label)
{
    if (img.empty())
    {
        qWarning("Image is empty, cannot display.");
        return;
    }

    cv::Mat displayImg;
    if (img.channels() == 1)
    {
        // Convert grayscale to RGB
        cv::cvtColor(img, displayImg, cv::COLOR_GRAY2RGB);
    }
    else if (img.channels() == 3)
    {
        displayImg = img;
    }
    else
    {
        qWarning("Unsupported number of channels in the image.");
        return;
    }

    QImage qimg(displayImg.data, displayImg.cols, displayImg.rows, displayImg.step, QImage::Format_RGB888);
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

        gridRows = gridRowsInput->value();
        gridCols = gridColsInput->value();
        gridPitch = gridPitchInput->value();
        maxSnaps = maxSnapsInput->value();

        snapButton->setEnabled(true);
        maxSnapsStatus->setText("Settings applied. You can start snapping.");
        setButton->setText("Reset");
    }
    else
    {
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
     if (snapsTaken >= maxSnaps)
    {
        maxSnapsStatus->setText("You have reached the maximum number of snaps.");
        inspectButton->setEnabled(true);
        saveButton->setEnabled(true);
        snapButton->setEnabled(false);
        return;
    }

    cv::Mat frame;
    bool ret = cap.read(frame);
    if (ret)
    {
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> corners;
        ret = cv::findChessboardCorners(gray, cv::Size(gridColsInput->value(), gridRowsInput->value()), corners);

        if (ret)
        {
            std::vector<cv::Point3f> objp;
            for (int i = 0; i < gridRowsInput->value(); i++)
            {
                for (int j = 0; j < gridColsInput->value(); j++)
                {
                    objp.push_back(cv::Point3f(j * gridPitchInput->value(), i * gridPitchInput->value(), 0.0f));
                }
            }

            objPoints.push_back(objp);

            // Refine corner positions
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001));

            imgPoints.push_back(corners);

            snapsTaken++;
            cv::drawChessboardCorners(frame, cv::Size(gridColsInput->value(), gridRowsInput->value()), corners, ret);

            maxSnapsStatus->setText(QString("Snap %1 captured successfully.").arg(snapsTaken));

            if (snapsTaken < maxSnaps)
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
    else
    {
        maxSnapsStatus->setText("Failed to capture frame from camera.");
    }
}

void CameraApp::goToInspectPage()
{
    stackedWidget->setCurrentIndex(1);
    PauseFlag = true;
    //disconnect(timer, &QTimer::timeout, this, &CameraApp::updateFrame);
    //connect(timer, &QTimer::timeout, this, &CameraApp::updateInspectionFrame);
    cv::VideoCapture cap(0, cv::CAP_V4L2);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CameraApp::updateInspectionFrame);
    timer->start(30);
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
    cap.release();
    disconnect(timer, &QTimer::timeout, this, &CameraApp::updateInspectionFrame);
    PauseFlag = false;
    //connect(timer, &QTimer::timeout, this, &CameraApp::updateFrame);
}

void CameraApp::closeApp()
{
    cap.release();
    close();
}
