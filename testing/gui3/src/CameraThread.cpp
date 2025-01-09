
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
    int simage_height = int(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    image_height = simage_height;
    int simage_width = int(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    image_width = simage_width;
    std::cout << "Camera resolution: " << simage_width << "x" << simage_height << std::endl;
    
    cap.set(cv::CAP_PROP_FRAME_WIDTH, target_width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, target_height);
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);

    while (running) {
        cap.set(cv::CAP_PROP_EXPOSURE, exposure_value);

        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Could not read a frame." << std::endl;
            break;
        }

        // Process the frame
        processFrame(frame);
    }

    cap.release();
}

void CameraThread::processFrame(cv::Mat &frame) {
    std::cout << "Processing frame is running" << std::endl;
    // Create calibrator and process the frame
    CalibrateFeed calibrator(frame); // Replace with actual constructor
    frame = calibrator.calibrate("data/camera_calibration.yml");

    cv::Mat userFeed, roi, not_roi;
    int y_start, y_end, px, py, bottom_px, bottom_line_y, p_position, bottom_p_position;
    int line_position_y_start = 0, line_position_y_end = 0;

   std::tie(userFeed, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px) = 
        UsersLiveFeed(frame, line_position_y_start, line_position_y_end, p_position, bottom_p_position, px, py, bottom_px);
 // Replace with actual function
    std::cout << "UsersLiveFeed is completed " << std::endl;
    RegionOfInterest roiExtractor(frame, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px, offset_line_value);
    std::tie(roi, not_roi, std::ignore, std::ignore) = roiExtractor.ROI();

    // Laser detection
    LaserDetection laserDetector(roi);
    cv::Mat laserDetected;
    int x1, y1, x2, y2;
    cv::Mat laserDetectedImage = laserDetector.laserDetection(x1, y1, x2, y2);

    // Combine overlays
    std::cout <<"combine overloding is started"<<std::endl; 
    cv::Mat feed = frame.clone();
    laser_detected.copyTo(feed(Rect(0, 0, roi.cols, roi.rows)));
    cv::Mat mask, mask_inv, backgroundPart, overlayPart;
    cv::inRange(laserDetected, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), mask);
    cv::bitwise_not(mask, mask_inv);
    cv::bitwise_and(frame, frame, backgroundPart, mask);
    cv::bitwise_and(feed, feed, overlayPart, mask_inv);
    cv::add(backgroundPart, overlayPart, result);
    std::cout <<"combine overloding is completed"<<std::endl;
     Mat frame_region, inverse_not_roi, background_region, result_feed;
    bitwise_and(frame, frame, frame_region, not_roi);

    // Invert the ROI mask
    cv::bitwise_not(not_roi, inverse_not_roi);
    cv::bitwise_and(result, result, background_region, inverse_not_roi);
    cv::add(frame_region, background_region, result_feed);
    // Draw a line on the result feed
    cv::line(result_feed, Point(px, py), Point(bottom_px, bottom_line_y), Scalar(150, 0, 0), 3);


    // Generate the QImage for main feed
    cv::Mat resizedUserFeed;
    cv::resize(userFeed, resizedUserFeed, cv::Size(target_width, target_height));
    QImage mainImage(resizedUserFeed.data, resizedUserFeed.cols, resizedUserFeed.rows, resizedUserFeed.step, QImage::Format_RGB888);
    emit imageMain(mainImage);
    std::cout <<"resized image is emited to main image"<<std::endl;

    // Generate the QImage for result feed
    // cv::Mat resizedResultFeed;
    // cv::resize(result, resizedResultFeed, cv::Size(target_width, target_height));
    // QImage resultImage(resizedResultFeed.data, resizedResultFeed.cols, resizedResultFeed.rows, resizedResultFeed.step, QImage::Format_RGB888);
    // emit imageResult(resultImage);
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

std::tuple<cv::Mat, int, int, int, int, int, int, int> CameraThread::UsersLiveFeed(const cv::Mat& frame, int line_position_y_start, int line_position_y_end, int point_position, int bottom_point_position, int px, int py, int bottom_px) {
    std::cout << "UsersLiveFeed is running" << std::endl;
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