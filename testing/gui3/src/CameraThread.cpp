#define ENABLE_CAM_DEBUG 1
#include "../inc/CameraThread.h"
#include <iostream>
#include "../inc/SharedVariables.h"

CameraThread::CameraThread()
    : running(true), target_height(240), target_width(320), exposure_value(100), offset_line_value(130)
{
}

CameraThread::~CameraThread()
{
    stop();
}

void CameraThread::run()
{
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, target_width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, target_height);
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
    bottom_line_y = image_height;
    CAM_FILE_DEBUG << "px1: " << px << " py1: " << py;

    while (running)
    {
        cap.set(cv::CAP_PROP_EXPOSURE, exposure_value);

        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Could not read a frame." << std::endl;
            break;
        }

        // Process the frame
        processFrame(frame);
    }

    cap.release();
}

void CameraThread::processFrame(cv::Mat &frame)
{
    CAM_FILE_DEBUG << "Processing frame";
    // Create calibrator and process the frame
    CalibrateFeed calibrator(frame); // Replace with actual constructor
    frame = calibrator.calibrate("data/camera_calibration.yml");
    CAM_FILE_DEBUG << "image size: " << image_height << "X" << image_width;

    cv::Mat userFeed, roi, not_roi;
    // int y_start = image_height/2 , y_end =image_height/2, px = image_width/2 , py = image_height/2  , bottom_px = image_width/2 , p_position =image_width/2 , bottom_p_position = image_height;
    int y_start, y_end, bottom_px, p_position, bottom_p_position;
    // std::pair<int, int> u, l;
    cv::Point u, l;

    std::tie(userFeed, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px) = UsersLiveFeed(frame);
    // Replace with actual function
    std::cout << "UsersLiveFeed is completed " << std::endl;
    RegionOfInterest roiExtractor(frame, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px, offset_line_value);
    std::tie(roi, not_roi, l, u) = roiExtractor.ROI();
    if (not_roi.type() != CV_8U)
    {
        cv::cvtColor(not_roi, not_roi, cv::COLOR_BGR2GRAY); // Convert to grayscale if needed
        not_roi.convertTo(not_roi, CV_8U);                 // Convert to 8-bit single-channel
    }
        if (not_roi.size() != frame.size())
    {
        cv::resize(not_roi, not_roi, frame.size());
    }


    // when offset is added

    if (plot_point)
    {
        if (!(u == cv::Point(0, 0)) && !(l == cv::Point(0, 0)))
        {
            // std::vector<cv::Point> not_mask1 = {
            //     cv::Point(image_width, 0),           // Top-right corner
            //     cv::Point(u.x, 0),                   // Point along the top edge
            //     cv::Point(u.x, u.y),                 // Upper point (u)
            //     cv::Point(l.x, l.y),                 // Lower point (l)
            //     cv::Point(image_width, image_height) // Bottom-right corner
            // };
            // cv::fillPoly(userFeed, std::vector<std::vector<cv::Point>>{not_mask1}, cv::Scalar(0, 0, 0));

            // Second mask
            // std::vector<cv::Point> not_mask2 = {
            //     cv::Point(0, line_position_y_start),         // Left edge at line_position_y_start
            //     cv::Point(image_width, line_position_y_end), // Right edge at line_position_y_end
            //     cv::Point(image_width, image_height),        // Bottom-right corner
            //     cv::Point(0, image_height)                   // Bottom-left corner
            // };
            // cv::fillPoly(userFeed, std::vector<std::vector<cv::Point>>{not_mask2}, cv::Scalar(0, 0, 0));
        }
    }

    //------------------------------------------------------------------------------------------------------
    //  cv::line(userFeed, cv::Point(0, line_position_y_start), cv::Point(image_width - 1, line_position_y_end), cv::Scalar(255, 255, 255), line_thickness);

    cv::Mat resizedUserFeed;
    cv::resize(userFeed, resizedUserFeed, cv::Size(target_width, target_height));
    QImage mainImage(resizedUserFeed.data, resizedUserFeed.cols, resizedUserFeed.rows, resizedUserFeed.step, QImage::Format_RGB888);
    emit imageMain(mainImage);

    // Laser detection
    LaserDetection laserDetector(roi);
    // cv::Mat laserDetected;
    //  int x1, y1, x2, y2;
    //  cv::Mat laserDetectedImage = laserDetector.laserDetection(x1, y1, x2, y2);
    auto [laserDetected, x1, y1, x2, y2] = laserDetector.laserDetection();

    // Combine overlays

    CAM_FILE_DEBUG << "combine overloading is started";
    cv::Mat feed = frame.clone();
    if (roi.cols > feed.cols || roi.rows > feed.rows)
    {
        std::cerr << "ROI dimensions exceed feed dimensions!" << std::endl;
        return;
    }
    if (laserDetected.channels() == 1 && feed.channels() == 3)
    {
        cv::cvtColor(laserDetected, laserDetected, cv::COLOR_GRAY2BGR); // Convert grayscale to 3-channel BGR
    }

    laserDetected.copyTo(feed(cv::Rect(0, 0, roi.cols, roi.rows)));
    CAM_FILE_DEBUG<< "laser detected is copied to feed and moving to bitwise oprration";
    cv::Mat mask, mask_inv, backgroundPart, overlayPart, result;
    cv::inRange(laserDetected, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), mask);
    cv::bitwise_not(mask, mask_inv);
    cv::bitwise_and(frame, frame, backgroundPart, mask);
    cv::bitwise_and(feed, feed, overlayPart, mask_inv);
    cv::add(backgroundPart, overlayPart, result);
    cv::inRange(laserDetected, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), mask);
    if (mask.type() != CV_8U)
    {
        mask.convertTo(mask, CV_8U);
    }


    CV_Assert(mask.type() == CV_8U);
    CV_Assert(not_roi.type() == CV_8U);

    CAM_FILE_DEBUG << "combine overloading is completed";
    cv::Mat frame_region, inverse_not_roi, background_region, result_feed;
    bitwise_and(frame, frame, frame_region, not_roi);

    // Invert the ROI mask
    cv::bitwise_not(not_roi, inverse_not_roi);
    cv::bitwise_and(result, result, background_region, inverse_not_roi);
    cv::add(frame_region, background_region, result_feed);
    // Draw a line on the result feed
    // cv::line(result_feed, cv::Point(px, py), cv::Point(bottom_px, bottom_line_y), cv::Scalar(150, 0, 0), 3);

    // Generate the QImage for main feed
    // cv::Mat resizedUserFeed;
    // cv::resize(userFeed, resizedUserFeed, cv::Size(target_width, target_height));
    // QImage mainImage(resizedUserFeed.data, resizedUserFeed.cols, resizedUserFeed.rows, resizedUserFeed.step, QImage::Format_RGB888);
    // emit imageMain(mainImage);
    // std::cout << "resized image is emited to main image" << std::endl;

    // Generate the QImage for result feed
    // cv::Mat resizedResultFeed;
    // cv::resize(result, resizedResultFeed, cv::Size(target_width, target_height));
    // QImage resultImage(resizedResultFeed.data, resizedResultFeed.cols, resizedResultFeed.rows, resizedResultFeed.step, QImage::Format_RGB888);
    // emit imageResult(resultImage);
}

void CameraThread::stop()
{
    running = false;
    if (cap.isOpened())
    {
        cap.release();
    }
}

void CameraThread::updateExposure(int value)
{
    // CAM_FILE_DEBUG << "Updating exposure";
    exposure_value = value;
}

void CameraThread::updateOffsetLineValue(int value)
{
    // CAM_FILE_DEBUG << "updating offset line value";
    offset_line_value = value;
}
