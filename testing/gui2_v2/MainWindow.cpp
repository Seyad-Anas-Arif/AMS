#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

//--------------------------------------ui part of mainwindow--------------
    MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
        ui(new Ui::MainWindow)
    {
        ui->setupUi(this);

        // Additional setup for sliders or components
        connect(ui->Default_btn, &QPushButton::clicked, this, &MainWindow::setDefaultSettings);
        connect(ui->save_btn, &QPushButton::clicked, this, &MainWindow::saveSettings);
        connect(ui->apply_btn, &QPushButton::clicked, this, &MainWindow::applySettings);
        connect(ui->run_btn, &QPushButton::clicked, this, &MainWindow::runOperation);
        connect(ui->exit_btn, &QPushButton::clicked, this, &MainWindow::close);

        // Customize stacked widget pages
        QWidget *page1 = new QWidget;
        QLabel *label1 = new QLabel("Page 1 Content", page1);
        QVBoxLayout *layout1 = new QVBoxLayout(page1);
        layout1->addWidget(label1);

        QWidget *page2 = new QWidget;
        QPushButton *button2 = new QPushButton("Page 2 Button", page2);
        QVBoxLayout *layout2 = new QVBoxLayout(page2);
        layout2->addWidget(button2);

        ui->result_window->addWidget(page1);
        ui->result_window->addWidget(page2);

        ui->result_window->setCurrentIndex(0);
    }

    MainWindow::~MainWindow()
    {
        delete ui;
    }

    // Example slots for button functionality
    void MainWindow::setDefaultSettings()
    {
        // Reset all sliders to default
        ui->y_start_slider->setValue(0);
        ui->y_end_slider->setValue(0);
        ui->x_start_slider->setValue(0);
        ui->X_end_slider->setValue(0);
        ui->offset_slider->setValue(0);
        ui->exposure_spinbox->setValue(10);  // Default value
    }

    void MainWindow::saveSettings()
    {
        // Save current settings logic
        qDebug() << "Settings saved.";
    }

    void MainWindow::applySettings()
    {
        // Apply current settings logic
        qDebug() << "Settings applied.";
    }

    void MainWindow::runOperation()
{
    // Run the operation logic
    qDebug() << "Running operation.";
}
//-----------------------------------------above ui part is completed and below i given main logic and coding part----------


// Global variables
cv::VideoCapture cap(0);
int image_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
int image_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
int line_position_y_start = image_height / 2;
int line_position_y_end = image_height / 2;
int line_thickness = 2;
int point_position = image_width / 2;
int bottom_line_y = image_height;
int bottom_point_position = image_width / 2;

// Default values
int default_line_position_y_start = line_position_y_start;
int default_line_position_y_end = line_position_y_end;
int default_point_position = point_position;
int default_bottom_point_position = bottom_point_position;

// Function to fit a plane to 3D points
cv::Vec3f fit_plane(const std::vector<cv::Vec3f>& points) {
    cv::Vec3f centroid(0, 0, 0);
    for (const auto& point : points) {
        centroid += point;
    }
    centroid /= static_cast<float>(points.size());

    cv::Mat centered(points.size(), 3, CV_32F);
    for (size_t i = 0; i < points.size(); ++i) {
        centered.at<float>(i, 0) = points[i][0] - centroid[0];
        centered.at<float>(i, 1) = points[i][1] - centroid[1];
        centered.at<float>(i, 2) = points[i][2] - centroid[2];
    }

    cv::Mat w, u, vt;
    cv::SVD::compute(centered, w, u, vt);

    cv::Vec3f normal(vt.at<float>(2, 0), vt.at<float>(2, 1), vt.at<float>(2, 2));
    return normal / cv::norm(normal);
}

// Function to calculate angle between two planes
double angle_between_planes(const cv::Vec3f& normal1, const cv::Vec3f& normal2) {
    double dot_product = normal1.dot(normal2);
    dot_product = std::clamp(dot_product, -1.0, 1.0);
    double angle = std::acos(dot_product);
    return angle * (180.0 / CV_PI);
}

// Function to process the live feed
void users_live_feed(const cv::Mat& frame, cv::Mat& output) {
    output = frame.clone();

    // Draw horizontal line
    cv::line(output, cv::Point(0, line_position_y_start), cv::Point(image_width - 1, line_position_y_end), cv::Scalar(255, 255, 255), line_thickness);

    // Calculate and draw vertical line from point to bottom
    int px = point_position;
    int py = line_position_y_start + static_cast<int>((static_cast<float>(point_position) / image_width) * (line_position_y_end - line_position_y_start));

    px = std::clamp(px, 0, image_width - 1);
    py = std::clamp(py, 0, image_height - 1);

    int bottom_px = bottom_point_position;
    bottom_px = std::clamp(bottom_px, 0, image_width - 1);

    cv::line(output, cv::Point(px, py), cv::Point(bottom_px, bottom_line_y), cv::Scalar(255, 255, 255), line_thickness);
}

// Update functions
void update_y_axis_start(int value) {
    line_position_y_start = image_height - value;
}

void update_y_axis_end(int value) {
    line_position_y_end = image_height - value;
}

void update_point_position(int value) {
    point_position = value;
}

void update_bottom_point_position(int value) {
    bottom_point_position = value;
}
