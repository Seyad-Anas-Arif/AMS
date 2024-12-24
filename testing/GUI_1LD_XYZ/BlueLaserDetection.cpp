#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>
#include <stdexcept>

class LaserDetection {
public:
    static const int BORDER_THRESHOLD = 1;

    LaserDetection(cv::Mat& frame) : frame(frame) {
        if (frame.empty()) {
            throw std::invalid_argument("Image not found or unable to load.");
        }
        optimal_line = cv::Vec4i();
        laser = cv::Mat();
    }

    cv::Vec4i selectOptimalLine(const std::vector<cv::Vec4i>& lines) {
        if (lines.empty()) return cv::Vec4i();

        int height = frame.rows;
        int width = frame.cols;
        std::vector<std::tuple<cv::Vec4i, double, double>> angles_lengths;

        for (const auto& line : lines) {
            int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];

            if (x1 < BORDER_THRESHOLD || x2 < BORDER_THRESHOLD ||
                x1 > width - BORDER_THRESHOLD || x2 > width - BORDER_THRESHOLD ||
                y1 < BORDER_THRESHOLD || y2 < BORDER_THRESHOLD ||
                y1 > height - BORDER_THRESHOLD || y2 > height - BORDER_THRESHOLD) {
                continue;
            }

            double angle = atan2(y2 - y1, x2 - x1);
            double length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
            angles_lengths.emplace_back(line, angle, length);
        }

        if (angles_lengths.empty()) return cv::Vec4i();

        std::map<double, int> angle_counts;
        for (const auto& [_, angle, __] : angles_lengths) {
            angle_counts[angle]++;
        }

        double common_angle = std::max_element(angle_counts.begin(), angle_counts.end(),
                                               [](const auto& a, const auto& b) {
                                                   return a.second < b.second;
                                               })->first;

        cv::Vec4i longest_line;
        double max_length = 0;
        for (const auto& [line, angle, length] : angles_lengths) {
            if (angle == common_angle && length > max_length) {
                longest_line = line;
                max_length = length;
            }
        }

        return longest_line;
    }

    cv::Vec4i extendLineToBoundaries(const cv::Vec4i& line, const cv::Size& image_shape, int midpoint = 0) {
        if (line == cv::Vec4i()) return cv::Vec4i();

        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        int width = image_shape.width;
        int height = image_shape.height;

        if (y1 == y2) {
            return {0, y1 + midpoint, width, y2 + midpoint};
        } else if (x1 == x2) {
            return {x1, 0, x2, height};
        } else {
            double slope = static_cast<double>(y2 - y1) / (x2 - x1);
            double intercept = y1 - slope * x1;

            int new_x1 = 0;
            int new_y1 = static_cast<int>(slope * new_x1 + intercept);
            int new_x2 = width;
            int new_y2 = static_cast<int>(slope * new_x2 + intercept);

            return {new_x1, new_y1 + midpoint, new_x2, new_y2 + midpoint};
        }
    }

    void plotOptimalLine(int midpoint = 0) {
        std::vector<cv::Vec4i> lines;
        cv::HoughLinesP(laser, lines, 1, CV_PI / 180, 50, 0, 100);

        if (lines.empty()) return;

        cv::Vec4i optimal_line = selectOptimalLine(lines);
        this->optimal_line = optimal_line;
        cv::Vec4i extended_line = extendLineToBoundaries(optimal_line, frame.size(), midpoint);

        if (extended_line != cv::Vec4i()) {
            cv::line(frame, {optimal_line[0], optimal_line[1]}, {optimal_line[2], optimal_line[3]}, cv::Scalar(0, 0, 150), 3);
        }
    }

    void laserDetection() {
        cv::Mat hsv_frame;
        cv::cvtColor(frame, hsv_frame, cv::COLOR_BGR2HSV);

        cv::Mat hsv_blur;
        cv::bilateralFilter(hsv_frame, hsv_blur, 2, 90, 90);

        cv::Scalar lower_blue(90, 90, 40);
        cv::Scalar upper_blue(125, 255, 255);

        cv::Mat blue_mask;
        cv::inRange(hsv_blur, lower_blue, upper_blue, blue_mask);

        cv::Mat blue_regions;
        cv::bitwise_and(frame, frame, blue_regions, blue_mask);

        std::vector<cv::Mat> channels;
        cv::split(blue_regions, channels);

        cv::Mat gray_frame = channels[0];
        int height = gray_frame.rows;

        cv::Mat row_intensity_mask = cv::Mat::zeros(gray_frame.size(), gray_frame.type());
        for (int i = 0; i < height; ++i) {
            double max_intensity;
            cv::Point max_loc;
            cv::minMaxLoc(gray_frame.row(i), nullptr, &max_intensity, nullptr, &max_loc);
            row_intensity_mask.at<uchar>(i, max_loc.x) = gray_frame.at<uchar>(i, max_loc.x);
        }

        cv::Mat com_mask = cv::Mat::zeros(gray_frame.size(), gray_frame.type());
        for (int i = 0; i < height; ++i) {
            const uchar* row = row_intensity_mask.ptr<uchar>(i);
            int a = -1, b = -1;

            for (int j = 0; j < gray_frame.cols; ++j) {
                if (row[j] > 0) {
                    if (a == -1) a = j;
                    if (j == gray_frame.cols - 1 || row[j + 1] == 0) {
                        b = j;
                        break;
                    }
                }
            }

            if (a != -1 && b != -1) {
                int com = (a + b) / 2;
                com_mask.at<uchar>(i, com) = 255;
            }
        }

        laser = com_mask;
        plotOptimalLine();
    }

private:
    cv::Mat frame;
    cv::Mat laser;
    cv::Vec4i optimal_line;
};

int main() {
    cv::Mat frame = cv::imread("input_image.jpg");
    if (frame.empty()) {
        std::cerr << "Error: Unable to load the image!" << std::endl;
        return -1;
    }

    try {
        LaserDetection detector(frame);
        detector.laserDetection();

        cv::imshow("Detected Laser", frame);
        cv::waitKey(0);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

/*-------------------------------------alter native 
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>
#include <map>

class LaserDetection {
public:
    static const int BORDER_THRESHOLD = 1;
    cv::Mat frame;
    cv::Mat laser;
    cv::Vec4i optimalLine;

    LaserDetection(const cv::Mat& inputFrame) : frame(inputFrame) {
        if (frame.empty()) {
            throw std::invalid_argument("Image not found or unable to load.");
        }
    }

    cv::Vec4i selectOptimalLine(const std::vector<std::vector<cv::Vec4i>>& lines) {
        if (lines.empty()) {
            return cv::Vec4i(-1, -1, -1, -1);  // Return an invalid line if no lines
        }

        int height = frame.rows;
        int width = frame.cols;
        std::vector<std::tuple<cv::Vec4i, float, float>> anglesLengths;

        for (const auto& lineGroup : lines) {
            for (const auto& line : lineGroup) {
                int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];

                // Ignore lines near the border
                if (x1 < BORDER_THRESHOLD || x2 < BORDER_THRESHOLD || 
                    x1 > width - BORDER_THRESHOLD || x2 > width - BORDER_THRESHOLD ||
                    y1 < BORDER_THRESHOLD || y2 < BORDER_THRESHOLD || 
                    y1 > height - BORDER_THRESHOLD || y2 > height - BORDER_THRESHOLD) {
                    continue;
                }

                // Calculate angle and length of the line
                float angle = std::atan2(y2 - y1, x2 - x1);
                float length = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
                anglesLengths.push_back(std::make_tuple(line, angle, length));
            }
        }

        if (anglesLengths.empty()) {
            return cv::Vec4i(-1, -1, -1, -1);  // Return an invalid line if no valid lines
        }

        // Find the most common angle
        std::map<float, int> angleCounts;
        for (const auto& angleLength : anglesLengths) {
            float angle = std::get<1>(angleLength);
            angleCounts[angle]++;
        }

        // Get the most common angle
        float commonAngle = std::max_element(angleCounts.begin(), angleCounts.end(),
                                             [](const std::pair<float, int>& a, const std::pair<float, int>& b) {
                                                 return a.second < b.second;
                                             })->first;

        // Find the longest line with the most common angle
        cv::Vec4i longestLine;
        float maxLength = 0;
        for (const auto& angleLength : anglesLengths) {
            const cv::Vec4i& line = std::get<0>(angleLength);
            float angle = std::get<1>(angleLength);
            float length = std::get<2>(angleLength);
            if (angle == commonAngle && length > maxLength) {
                longestLine = line;
                maxLength = length;
            }
        }

        return longestLine;
    }

    cv::Vec4i extendLineToBoundaries(const cv::Vec4i& line, const cv::Size& imageSize, int midpoint = 0) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        int height = imageSize.height;
        int width = imageSize.width;

        // Extend line to the boundaries
        if (y1 == y2) {
            return cv::Vec4i(0, y1 + midpoint, width, y2 + midpoint);
        } else if (x1 == x2) {
            return cv::Vec4i(x1, 0, x2, height);
        } else {
            float slope = float(y2 - y1) / (x2 - x1);
            float intercept = y1 - slope * x1;
            int newX1 = 0;
            int newY1 = int(slope * newX1 + intercept);
            int newX2 = width;
            int newY2 = int(slope * newX2 + intercept);

            return cv::Vec4i(newX1, newY1 + midpoint, newX2, newY2 + midpoint);
        }
    }

    cv::Mat plotOptimalLine(int midpoint = 0) {
        std::vector<std::vector<cv::Vec4i>> lines;
        cv::HoughLinesP(laser, lines, 1, CV_PI / 180, 50, 0, 100);
        if (lines.empty()) {
            return frame;  // Return original frame if no line is detected
        }

        optimalLine = selectOptimalLine(lines);
        cv::Vec4i extendedLine = extendLineToBoundaries(optimalLine, frame.size(), midpoint);

        if (extendedLine[0] != -1) {
            cv::line(frame, cv::Point(optimalLine[0], optimalLine[1]), cv::Point(optimalLine[2], optimalLine[3]), 
                     cv::Scalar(0, 0, 150), 3);
        }

        return frame;
    }

    cv::Mat laserDetection() {
        // Convert the frame from BGR to HSV
        cv::Mat hsvFrame;
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

        // Apply a bilateral filter to smooth the image while preserving edges
        cv::Mat hsvBlur;
        cv::bilateralFilter(hsvFrame, hsvBlur, 2, 90, 90);

        // Define the range for detecting 450 nm blue laser in HSV
        cv::Scalar lowerBlue(90, 90, 40);
        cv::Scalar upperBlue(125, 255, 255);

        // Create a mask for the blue color
        cv::Mat blueMask;
        cv::inRange(hsvBlur, lowerBlue, upperBlue, blueMask);

        // Apply the mask to get the blue regions in the image
        cv::Mat blueRegions;
        cv::bitwise_and(frame, frame, blueRegions, blueMask);

        // Split channels and use the blue channel as the grayscale frame
        std::vector<cv::Mat> channels;
        cv::split(blueRegions, channels);
        cv::Mat grayFrame = channels[0];  // Assuming blue laser has highest intensity in blue channel

        // Get the dimensions of the grayscale frame
        int height = grayFrame.rows;
        int width = grayFrame.cols;

        // Create an empty mask to store the result
        cv::Mat rowIntensityMask = cv::Mat::zeros(grayFrame.size(), CV_8U);

        // Loop through each row to find the pixels with the highest intensity and mask the rest
        for (int i = 0; i < height; ++i) {
            double maxIntensity;
            cv::minMaxLoc(grayFrame.row(i), &maxIntensity);

            // Get indices of max intensity pixels
            for (int j = 0; j < width; ++j) {
                if (grayFrame.at<uchar>(i, j) == maxIntensity) {
                    rowIntensityMask.at<uchar>(i, j) = maxIntensity;
                }
            }
        }

        // Create an empty mask to store the CoM
        cv::Mat comMask = cv::Mat::zeros(grayFrame.size(), CV_8U);

        // Find 'A', 'B', and CoM for each row
        for (int i = 0; i < height; ++i) {
            cv::Mat row = rowIntensityMask.row(i);

            // Find indices of all white pixels (non-zero)
            std::vector<int> whiteIndices;
            for (int j = 0; j < width; ++j) {
                if (row.at<uchar>(0, j) > 0) {
                    whiteIndices.push_back(j);
                }
            }

            if (!whiteIndices.empty()) {
                // 'A' is the first white pixel
                int a = whiteIndices[0];

                // 'B' is the last white pixel before the first black pixel after 'A'
                int b = whiteIndices.back();
                for (int idx = a + 1; idx < whiteIndices.size(); ++idx) {
                    if (row.at<uchar>(0, whiteIndices[idx]) == 0) {
                        b = whiteIndices[idx] - 1;
                        break;
                    }
                }

                // Calculate CoM as the midpoint between 'A' and 'B'
                int com = (a + b) / 2;

                // Set the CoM position in the mask to white
                comMask.at<uchar>(i, com) = 255;
            }
        }

        laser = comMask;
        return plotOptimalLine();
    }
};

int main() {
    cv::Mat frame = cv::imread("your_image_path.jpg");
    LaserDetection laserDetection(frame);

    try {
        cv::Mat detectedLaser = laserDetection.laserDetection();
        cv::imshow("Detected Laser", detectedLaser);
        cv::waitKey(0);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
*/