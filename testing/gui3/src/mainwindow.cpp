#include "../inc/mainwindow.h"
#include "../inc/ui_mainwindow.h" // Include the generated UI header
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <tuple>
using namespace std;


// Define global variables
int line_position_y_start = 100; // Example value
int line_position_y_end = 200;   // Example value
int point_position = 150;        // Example value
int bottom_point_position = 300; // Example value
int image_width = 640;           // Example value
int image_height = 480;          // Example value
int line_thickness = 2;          // Example value
int bottom_line_y = 400;         // Example value

std::tuple<cv::Mat, int, int, int, int, int, int, int> UsersLiveFeed(const cv::Mat& frame, int line_position_y_start, int line_position_y_end, int point_position, int bottom_point_position, int px, int py, int bottom_px) {
    cv::Mat img = frame.clone();

    // Draw horizontal line
    cv::line(img, cv::Point(0, line_position_y_start), cv::Point(image_width - 1, line_position_y_end), cv::Scalar(255, 255, 255), line_thickness);

    // Calculate and draw vertical line from point to bottom
    px = std::clamp(px, 0, image_width - 1);
    py = std::clamp(py, 0, image_height - 1);

    bottom_px = std::clamp(bottom_px, 0, image_width - 1);

    cv::line(img, cv::Point(px, py), cv::Point(bottom_px, bottom_line_y), cv::Scalar(255, 255, 255), line_thickness);

    return std::make_tuple(img, line_position_y_start, line_position_y_end, point_position, bottom_point_position, px, py, bottom_px);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cameraThread(new CameraThread)
{
    ui->setupUi(this);

    // Connect signals from CameraThread to slots in MainWindow
    connect(cameraThread, &CameraThread::imageMain, this, &MainWindow::updateMainImage);
    connect(cameraThread, &CameraThread::imageResult, this, &MainWindow::updateMainImage);

    // Connect the run button to the slot
    connect(ui->run_btn, &QPushButton::clicked, this, &MainWindow::onRunButtonClicked);

    connect(ui->Developer_btn, &QPushButton::clicked, this, &MainWindow::onDeveloperbuttonClicked);

    // Connect the back button to the slot
    connect(ui->rw_back_btn, &QPushButton::clicked, this, &MainWindow::rwBackButtonClicked);

    connect(ui->dw_back_btn, &QPushButton::clicked, this, &MainWindow::dwBackButtonClicked);

    // Connect sliders or other UI elements to update exposure and offset line values
    connect(ui->exposure_spinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::updateExposure);
    connect(ui->offset_slider, static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged), this, &MainWindow::updateOffsetLineValue);

    // Start the camera thread
    cameraThread->start();
}

MainWindow::~MainWindow()
{
    cameraThread->stop();
    cameraThread->wait();
    delete cameraThread;
    delete ui;
}

void MainWindow::updateMainImage(const QImage &frame)
{
    ui->Display_lable->setPixmap(QPixmap::fromImage(frame).scaled(ui->Display_lable->size(), Qt::KeepAspectRatio));
}

void MainWindow::updateResultImage(const QImage &frame)
{
    // Update the result image label (if you have one)
    // ui->Result_lable->setPixmap(QPixmap::fromImage(frame).scaled(ui->Result_lable->size(), Qt::KeepAspectRatio));
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
    cameraThread->updateExposure(value);
}

void MainWindow::updateOffsetLineValue(int value)
{
    cameraThread->updateOffsetLineValue(value);
}