#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include "calibrate_camera.h"  // Replace with your calibration header
#include "mainwindow.h"  // Replace with your live feed header
#include "ROI.h"  // Replace with your ROI header
#include "blueLaser_detection.h"  // Replace with your laser detection header
#include "depth_detection.h"  // Replace with your XYZ header

class CameraThread : public QThread {
    Q_OBJECT

public:
    CameraThread() {
        running = true;
        target_height = 240;
        target_width = 320;
        exposure_value = -5;
        offset_line_value = 130;
        plot_point = false;
    }

    void stop() {
        running = false;
        if (cap.isOpened()) {
            cap.release();
        }
    }

    void updateExposure(int value) {
        exposure_value = value;
    }

    void updateOffsetLineValue(int value) {
        offset_line_value = value;
    }

signals:
    void imageMain(const QImage &image);
    void imageResult(const QImage &image);

protected:
    void run() override {
        cap.open(0); // Open the default camera

        if (!cap.isOpened()) {
            qDebug() << "Failed to open the camera.";
            return;
        }
        cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.25); 
        cap.set(cv::CAP_PROP_EXPOSURE, exposure_value);

        while (running) {
            cv::Mat frame;
            if (!cap.read(frame)) {
                break;
            }

            CalibrateFeed calibrator(frame);
            frame = calibrator.calibrate();

            cv::Mat feed = frame.clone();
            cv::Mat user_feed;
            int y_start, y_end, p_position, bottom_p_position, px, py, bottom_px;
            std::tie(user_feed, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px) = Users_live_feed(frame, line_position_y_start, line_position_y_end, point_position, bottom_point_position, px, py, bottom_px);

            cv::Mat roi, not_roi, l, u;
            RegionOfInterest roi_processor(frame, y_start, y_end, cv::Point(px, py), cv::Point(bottom_px, bottom_line_y), offset_line_value);
            std::tie(roi, not_roi, std::ignore, std::ignore) = roi_processor.ROI();

            if (plot_point) {
                if (!u.empty() && !l.empty()) {
                    std::vector<cv::Point> not_mask = {
                        cv::Point(target_width, 0),
                        u,
                        l,
                        cv::Point(target_width, target_height)
                    };
                    cv::fillPoly(user_feed, std::vector<std::vector<cv::Point>>{not_mask}, cv::Scalar(0, 0, 0));
                }
            }

            cv::resize(user_feed, user_feed, cv::Size(target_width, target_height));
            QImage qimage(user_feed.data, user_feed.cols, user_feed.rows, user_feed.step, QImage::Format_RGB888);
            emit imageMain(qimage);

            LaserDetection laser_detector(roi);
            bool laser_detected;
            int x1, y1, x2, y2;
            laser_detected = laser_detector.laserDetection(x1, y1, x2, y2);

            if (!laser_detected) {
                qDebug() << "Not enough lines detected.";
                continue;
            }

            feed(cv::Rect(0, 0, roi.cols, roi.rows)) = roi;

            cv::Mat mask;
            cv::inRange(feed, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), mask);
            cv::Mat mask_inv;
            cv::bitwise_not(mask, mask_inv);

            cv::Mat background_part, overlay_part;
            cv::bitwise_and(frame, frame, background_part, mask);
            cv::bitwise_and(feed, feed, overlay_part, mask_inv);

            cv::Mat result;
            cv::add(background_part, overlay_part, result);

            cv::Mat frame_region, inverse_not_roi, background_region;
            cv::bitwise_and(frame, frame, frame_region, not_roi);
            cv::bitwise_not(not_roi, inverse_not_roi);
            cv::bitwise_and(result, result, background_region, inverse_not_roi);
            cv::add(frame_region, background_region, result);

            cv::line(result, cv::Point(px, py), cv::Point(bottom_px, bottom_line_y), cv::Scalar(0, 0, 150), 3);

            XYZ xyz_processor_above(x1, y1, x2, y2, /* other necessary parameters */);
            XYZ xyz_processor_below(px, py, bottom_px, bottom_line_y, /* other necessary parameters */);

            std::vector<cv::Point3f> xyz_above = xyz_processor_above.depth();
            std::vector<cv::Point3f> xyz_below = xyz_processor_below.depth();

            std::ofstream file_above("above.csv");
            file_above << "X (mm),Y (mm),Z (mm)\n";
            for (const auto &point : xyz_above) {
                file_above << point.x << "," << point.y << "," << point.z << "\n";
            }
            file_above.close();

            std::ofstream file_below("below.csv");
            file_below << "X (mm),Y (mm),Z (mm)\n";
            for (const auto &point : xyz_below) {
                file_below << point.x << "," << point.y << "," << point.z << "\n";
            }
            file_below.close();

            qDebug() << "3D coordinates saved.";

            // Resize and emit the result
            cv::resize(result, result, cv::Size(target_width, target_height));
            QImage result_qimage(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888);
            emit imageResult(result_qimage);
        }
    }

private:
    cv::VideoCapture cap;
    bool running;
    int target_height;
    int target_width;
    int exposure_value;
    int offset_line_value;
    bool plot_point;
    int bottom_line_y = 400; // Example value
    int image_width = 640;   // Example value
    int image_height = 480;  // Example value
    int line_position_y_start = 100; // Example value
    int line_position_y_end = 200;   // Example value
    int point_position = 150;        // Example value
    int bottom_point_position = 300; // Example value
    int line_thickness = 2;          // Example value
};

#endif // CAMERATHREAD_H
