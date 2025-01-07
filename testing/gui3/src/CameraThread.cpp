
#include "../inc/CameraThread.h"

  

CameraThread::CameraThread()
    : running(true),
      target_height(240),
      target_width(320),
      exposure_value(-5),
      offset_line_value(130) {
}

CameraThread::~CameraThread() {
    stop();
}

void CameraThread::run() {
    cap.open(0, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, target_width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, target_height);
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);

    while (running) {
        cap.set(cv::CAP_PROP_EXPOSURE, exposure_value);

        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) {
            break;
        }

        // Process the frame
        processFrame(frame);
    }

    cap.release();
}

void CameraThread::processFrame(cv::Mat &frame) {
    // Create calibrator and process the frame
    CalibrateFeed calibrator(frame); // Replace with actual constructor
    frame = calibrator.calibrate("data/camera_calibration.yml");

    cv::Mat userFeed, roi, not_roi;
    int y_start, y_end, px, py, bottom_px, bottom_line_y, p_position, bottom_p_position;
    int line_position_y_start = 0, line_position_y_end = 0;

   std::tie(userFeed, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px) = 
        UsersLiveFeed(frame, line_position_y_start, line_position_y_end, p_position, bottom_p_position, px, py, bottom_px);
 // Replace with actual function

    RegionOfInterest roiExtractor(frame, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px, offset_line_value);
    std::tie(roi, not_roi, std::ignore, std::ignore) = roiExtractor.ROI();

    // Laser detection
    LaserDetection laserDetector(roi);
    cv::Mat laserDetected;
    int x1, y1, x2, y2;
    cv::Mat laserDetectedImage = laserDetector.laserDetection(x1, y1, x2, y2);

    // Combine overlays
    cv::Mat result = frame.clone();
    cv::Mat mask, mask_inv, backgroundPart, overlayPart;
    cv::inRange(laserDetected, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), mask);
    cv::bitwise_not(mask, mask_inv);
    cv::bitwise_and(frame, frame, backgroundPart, mask);
    cv::bitwise_and(laserDetected, laserDetected, overlayPart, mask_inv);
    cv::add(backgroundPart, overlayPart, result);

    // Generate the QImage for main feed
    cv::Mat resizedUserFeed;
    cv::resize(userFeed, resizedUserFeed, cv::Size(target_width, target_height));
    QImage mainImage(resizedUserFeed.data, resizedUserFeed.cols, resizedUserFeed.rows, resizedUserFeed.step, QImage::Format_RGB888);
    emit imageMain(mainImage);

    // Generate the QImage for result feed
    cv::Mat resizedResultFeed;
    cv::resize(result, resizedResultFeed, cv::Size(target_width, target_height));
    QImage resultImage(resizedResultFeed.data, resizedResultFeed.cols, resizedResultFeed.rows, resizedResultFeed.step, QImage::Format_RGB888);
    emit imageResult(resultImage);
}

void CameraThread::stop() {
    running = false;
    if (cap.isOpened()) {
        cap.release();
    }
}

void CameraThread::updateExposure(int value) {
    exposure_value = value;
}

void CameraThread::updateOffsetLineValue(int value) {
    offset_line_value = value;
}
