#include "mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include <chrono>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stopFlag(false)
{
    ui->setupUi(this);

    // Initialize camera thread
    cameraThread = std::thread(&MainWindow::captureFrames, this);

    // Connect the run button to the slot
    connect(ui->run_btn, &QPushButton::clicked, this, &MainWindow::onRunButtonClicked);

    connect(ui->Developer_btn, &QPushButton::clicked, this, &MainWindow::onDeveloperbuttonClicked);

    // Connect the back button to the slot
    connect(ui->rw_back_btn, &QPushButton::clicked, this, &MainWindow::rwBackButtonClicked);

    connect(ui->dw_back_btn, &QPushButton::clicked, this, &MainWindow::dwBackButtonClicked);

    // Connect sliders or other UI elements to update exposure and offset line values
    connect(ui->exposure_spinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::updateExposure);
    connect(ui->offset_slider, static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged), this, &MainWindow::updateOffsetLineValue);
}

MainWindow::~MainWindow()
{
    stopFlag = true;
    if (cameraThread.joinable()) {
        cameraThread.join();
    }
    delete ui;
}

void MainWindow::captureFrames()
{
    cv::VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        qCritical("Error: Could not open the camera.");
        return;
    }

    while (!stopFlag) {
        cap.set(cv::CAP_PROP_EXPOSURE, exposureValue);  // Set the exposure
        cv::Mat frame;
        cap >> frame;

        if (!frame.empty()) {
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

            // Process the frame (implement your processing logic here)
            // For example, you can call your calibrate_feed and Users_live_feed functions here

            // Resize the frame to the target resolution
            cv::Mat resizedFrame;
            cv::resize(frame, resizedFrame, cv::Size(targetWidth, targetHeight));

            QImage qFrame(resizedFrame.data, resizedFrame.cols, resizedFrame.rows, resizedFrame.step, QImage::Format_RGB888);
            QMetaObject::invokeMethod(this, [this, qFrame]() {
                ui->Display_lable->setPixmap(QPixmap::fromImage(qFrame).scaled(ui->Display_lable->size(), Qt::KeepAspectRatio));
            }, Qt::QueuedConnection);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // Control frame rate (~30fps)
    }

    cap.release();
}

void MainWindow::updateFrame(const QImage &frame)
{
    QMetaObject::invokeMethod(this, [this, frame]() {
        ui->Display_lable->setPixmap(QPixmap::fromImage(frame).scaled(ui->Display_lable->size(), Qt::KeepAspectRatio));
    }, Qt::QueuedConnection);
}

void MainWindow::onRunButtonClicked()
{
    // Switch to the result window
    ui->stackedWidget->setCurrentWidget(ui->resultWindow);
}

void MainWindow::onDeveloperbuttonClicked()
{
    // Switch to the developer window
    ui->stackedWidget->setCurrentWidget(ui->developerWindow);
}

void MainWindow::rwBackButtonClicked()
{
    // Switch back to the main GUI window
    ui->stackedWidget->setCurrentWidget(ui->mainGuiWindow);
}

void MainWindow::dwBackButtonClicked()
{
    // Switch back to the main GUI window
    ui->stackedWidget->setCurrentWidget(ui->mainGuiWindow);
}

void MainWindow::updateExposure(int value)
{
    exposureValue = value;
}

void MainWindow::updateOffsetLineValue(int value)
{
    offsetLineValue = value;
    plotPoint = true;  // Set flag to plot the point
}