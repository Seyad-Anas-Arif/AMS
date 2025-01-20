#include "../inc/CameraThread.h"
#include <iostream>
#include "../inc/SharedVariables.h"

CameraThread::CameraThread()
    : running(true), target_height(240), target_width(320), exposure_value(100), offset_line_value(130)
{
    // Initialization code can go here if needed
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
    px = image_width / 2;
    py = image_height / 2;
    bottom_px = image_width / 2;
    bottom_line_y = image_height;
      qDebug() << "px1: " << px << " py1: " << py;


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
   // qDebug() << "Processing frame";
    // Create calibrator and process the frame
    CalibrateFeed calibrator(frame); // Replace with actual constructor
    frame = calibrator.calibrate("data/camera_calibration.yml");
    //qDebug() << "image size: " << image_height << "X" << image_width;

    cv::Mat userFeed, roi, not_roi;
   // int y_start = image_height/2 , y_end =image_height/2, px = image_width/2 , py = image_height/2  , bottom_px = image_width/2 , p_position =image_width/2 , bottom_p_position = image_height; 
   int y_start, y_end, bottom_px, p_position, bottom_p_position;
 



    std::tie(userFeed, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px) =UsersLiveFeed(frame);
    // Replace with actual function
    std::cout << "UsersLiveFeed is completed " << std::endl;
    RegionOfInterest roiExtractor(frame, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px, offset_line_value);
    std::tie(roi, not_roi, std::ignore, std::ignore) = roiExtractor.ROI();
/*// when offset is added
    if (plotPoint) {
        if (!u.empty() && !l.empty()) {
            vector<Point> notMask = {
                Point(imageWidth, 0),
                Point(u.x, u.y),
                Point(l.x, l.y),
                Point(imageWidth, imageHeight)
            };
            fillPoly(userFeed, vector<vector<Point>>{notMask}, Scalar(0, 0, 0));
        }

        vector<Point> notMask2 = {
            Point(0, linePositionYStart),
            Point(imageWidth, linePositionYEnd),
            Point(imageWidth, imageHeight),
            Point(0, imageHeight)
        };
        fillPoly(userFeed, vector<vector<Point>>{notMask2}, Scalar(0, 0, 0));
 }*/
   

//------------------------------------------------------------------------------------------------------
  //  cv::line(userFeed, cv::Point(0, line_position_y_start), cv::Point(image_width - 1, line_position_y_end), cv::Scalar(255, 255, 255), line_thickness);

    //qDebug() <<"px: " << px << " py: " << py << " y Strat: " << y_start << " y end : " << y_end << " bottom px: " << bottom_px << " bottom line y: " << bottom_line_y;
    cv::Mat resizedUserFeed;
    cv::resize(userFeed, resizedUserFeed, cv::Size(target_width, target_height));
    QImage mainImage(resizedUserFeed.data, resizedUserFeed.cols, resizedUserFeed.rows, resizedUserFeed.step, QImage::Format_RGB888);
    emit imageMain(mainImage);

    // Laser detection
    LaserDetection laserDetector(roi);
    //cv::Mat laserDetected;
    // int x1, y1, x2, y2;
    // cv::Mat laserDetectedImage = laserDetector.laserDetection(x1, y1, x2, y2);
    auto [laserDetected, x1, y1, x2, y2] = laserDetector.laserDetection();
    
    // Combine overlays

   // qDebug() << "combine overloading is started";
    cv::Mat feed = frame.clone();
    if (roi.cols > feed.cols || roi.rows > feed.rows)
    {
        std::cerr << "ROI dimensions exceed feed dimensions!" << std::endl;
        return;
    }
    std::cout << "laserDetected type: " << laserDetected.type()
              << ", channels: " << laserDetected.channels() << std::endl;
    std::cout << "feed type: " << feed.type()
              << ", channels: " << feed.channels() << std::endl;
    if (laserDetected.channels() == 1 && feed.channels() == 3)
    {
        cv::cvtColor(laserDetected, laserDetected, cv::COLOR_GRAY2BGR); // Convert grayscale to 3-channel BGR
    }
    laserDetected.copyTo(feed(cv::Rect(0, 0, roi.cols, roi.rows)));
    cv::Mat mask, mask_inv, backgroundPart, overlayPart, result;
    cv::inRange(laserDetected, cv::Scalar(0, 0, 0), cv::Scalar(0, 0, 0), mask);
    cv::bitwise_not(mask, mask_inv);
    cv::bitwise_and(frame, frame, backgroundPart, mask);
    cv::bitwise_and(feed, feed, overlayPart, mask_inv);
    cv::add(backgroundPart, overlayPart, result);
    
    CV_Assert(mask.type() == CV_8U);
    CV_Assert(not_roi.type() == CV_8U);

   // qDebug() << "combine overloading is completed";
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
    //std::cout << "resized image is emited to main image" << std::endl;

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
    //qDebug() << "Updating exposure";
    exposure_value = value;
}

void CameraThread::updateOffsetLineValue(int value)
{
    //qDebug() << "updating offset line value";
    offset_line_value = value;
}

std::tuple<cv::Mat, int, int, int, int, int, int, int> CameraThread::UsersLiveFeed(const cv::Mat &frame)
{
    //qDebug() << "Users live feed is runnig";
    int horizontal_left = 0;
    int horizontal_right = image_width;
    int vertical_line_x_top = image_width / 2;
    int vertical_line_x_bottom = image_width / 2;
    int horizontal_line_y = image_height / 2;  // This is the y-coordinate of the horizontal line

    cv::Mat img = frame.clone();

    // Draw horizontal line
    cv::line(img, cv::Point(0, line_position_y_start), cv::Point(image_width - 1, line_position_y_end), cv::Scalar(255, 255, 255), line_thickness);

    // Calculate and draw vertical line from point to bottom
    px = std::clamp(px, 0, image_width - 1);
    py = std::clamp(py, 0, image_height - 1);
    qDebug ()<<"hight * width "<<image_height<<" * "<<image_width;
    qDebug() << "px: " << px << " py: " << py;

    bottom_px = std::clamp(bottom_px, 0, image_width - 1);

    cv::line(img, cv::Point(vertical_line_x_top, horizontal_line_y), cv::Point(vertical_line_x_bottom, image_height), cv::Scalar(255, 255, 255), 2);
    cv::line(img, cv::Point(px, py), cv::Point(bottom_px, bottom_line_y), cv::Scalar(255, 255, 255), line_thickness);
    qDebug() << "px: " << px << " py: " << py << " bottom_px: " << bottom_px;

    return std::make_tuple(img, line_position_y_start, line_position_y_end, point_position, bottom_point_position, px, py, bottom_px);
}
