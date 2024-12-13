#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

using namespace cv;
using namespace std;

class CameraCalibration {
private:
    vector<vector<Point3f>> objpoints;
    vector<vector<Point2f>> imgpoints;
    Mat cameraMatrix;
    Mat distCoeffs;
    Size boardSize;
    float squareSize;
    int maxSnaps;
    int snapsTaken;
    bool calibrationDone;

public:
    CameraCalibration(int rows, int cols, float squareSizeMM, int maxSnaps)
        : boardSize(cols, rows), squareSize(squareSizeMM), maxSnaps(maxSnaps), snapsTaken(0), calibrationDone(false) {}

    void addSnap(Mat& frame) {
        vector<Point2f> corners;
        bool found = findChessboardCorners(frame, boardSize, corners, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

        if (found) {
            Mat gray;
            cvtColor(frame, gray, COLOR_BGR2GRAY);
            cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                         TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 30, 0.001));

            vector<Point3f> objp;
            for (int i = 0; i < boardSize.height; ++i) {
                for (int j = 0; j < boardSize.width; ++j) {
                    objp.push_back(Point3f(j * squareSize, i * squareSize, 0));
                }
            }

            objpoints.push_back(objp);
            imgpoints.push_back(corners);
            snapsTaken++;

            drawChessboardCorners(frame, boardSize, corners, found);
            cout << "Snap " << snapsTaken << " captured successfully." << endl;

            if (snapsTaken >= maxSnaps) {
                cout << "Maximum snaps reached. Ready for calibration." << endl;
            }
        } else {
            cout << "Checkerboard not detected in the frame." << endl;
        }
    }

    void calibrate(Size imageSize) {
        if (snapsTaken < maxSnaps) {
            cout << "Not enough snaps to calibrate. Please take more snaps." << endl;
            return;
        }

        vector<Mat> rvecs, tvecs;
        double rms = calibrateCamera(objpoints, imgpoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);

        if (rms > 0) {
            calibrationDone = true;
            cout << "Calibration successful. RMS error: " << rms << endl;
            cout << "Camera Matrix:\n" << cameraMatrix << endl;
            cout << "Distortion Coefficients:\n" << distCoeffs << endl;
        } else {
            cout << "Calibration failed. Please retake snaps." << endl;
        }
    }

    void saveCalibrationData(const string& filename) {
        if (!calibrationDone) {
            cout << "Calibration not done. Cannot save data." << endl;
            return;
        }

        ofstream file(filename);
        if (file.is_open()) {
            file << fixed << setprecision(6);
            file << "Camera Matrix:\n" << cameraMatrix << "\n\n";
            file << "Distortion Coefficients:\n" << distCoeffs << "\n";
            file.close();
            cout << "Calibration data saved to " << filename << endl;
        } else {
            cout << "Failed to open file for writing." << endl;
        }
    }

    void showUndistortedFeed(VideoCapture& cap) {
        if (!calibrationDone) {
            cout << "Calibration not completed. Cannot show undistorted feed." << endl;
            return;
        }

        Mat frame, undistorted;
        while (true) {
            cap >> frame;
            if (frame.empty()) {
                break;
            }

            undistort(frame, undistorted, cameraMatrix, distCoeffs);

            imshow("Original Feed", frame);
            imshow("Undistorted Feed", undistorted);

            if (waitKey(30) == 27) {
                break; // Exit on ESC key
            }
        }
    }
};

int main() {
    int rows = 6, cols = 9;
    float squareSize = 29.0; // mm
    int maxSnaps = 15;

    CameraCalibration calibrator(rows, cols, squareSize, maxSnaps);

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Error: Unable to open the camera." << endl;
        return -1;
    }

    cout << "Press 's' to capture a snap, 'c' to calibrate, 'u' to show undistorted feed, 'q' to quit." << endl;

    Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            break;
        }

        imshow("Camera Feed", frame);
        char key = waitKey(30);
        if (key == 's') {
            calibrator.addSnap(frame);
        } else if (key == 'c') {
            calibrator.calibrate(frame.size());
        } else if (key == 'u') {
            calibrator.showUndistortedFeed(cap);
        } else if (key == 'q') {
            break;
        }
    }

    cap.release();
    destroyAllWindows();

    calibrator.saveCalibrationData("calibration_data.txt");
    return 0;
}


-----------------------------------------snap image  function 0-------------------
#include <opencv2/opencv.hpp>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <vector>

class CameraCalibration {
public:
    CameraCalibration(QLabel* statusLabel, QPushButton* snapButton, QPushButton* inspectButton, QPushButton* saveButton, 
                      QSpinBox* maxSnapsInput, QSpinBox* gridColsInput, QSpinBox* gridRowsInput, QSpinBox* gridPitchInput)
        : maxSnapsStatus(statusLabel), snapButton(snapButton), inspectButton(inspectButton), saveButton(saveButton), 
          maxSnapsInput(maxSnapsInput), gridColsInput(gridColsInput), gridRowsInput(gridRowsInput), gridPitchInput(gridPitchInput),
          snapsTaken(0) {
        // Initialize the camera capture object (assuming default camera)
        cap.open(0);
    }

    void snapImage() {
        if (snapsTaken >= maxSnapsInput->value()) {
            maxSnapsStatus->setText("You have reached the maximum number of snaps.");
            inspectButton->setEnabled(true);  // Enable the Inspect button
            saveButton->setEnabled(true);
            snapButton->setEnabled(false);  // Disable the Snap button
            return;
        }

        cv::Mat frame;
        bool ret = cap.read(frame);
        if (ret) {
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

            std::vector<cv::Point2f> corners;
            bool found = cv::findChessboardCorners(gray, cv::Size(gridColsInput->value(), gridRowsInput->value()), corners);
            if (found) {
                // Create object points (3D points for the chessboard)
                std::vector<cv::Point3f> objp;
                objp.resize(gridColsInput->value() * gridRowsInput->value());
                for (int i = 0; i < gridRowsInput->value(); ++i) {
                    for (int j = 0; j < gridColsInput->value(); ++j) {
                        objp[i * gridColsInput->value() + j] = cv::Point3f(j * gridPitchInput->value(), i * gridPitchInput->value(), 0.0f);
                    }
                }

                objpoints.push_back(objp);
                cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001));
                imgpoints.push_back(corners);

                snapsTaken++;
                cv::drawChessboardCorners(frame, cv::Size(gridColsInput->value(), gridRowsInput->value()), corners, found);
                maxSnapsStatus->setText(QString("Snap %1 captured successfully.").arg(snapsTaken));

                if (snapsTaken < maxSnapsInput->value()) {
                    inspectButton->setEnabled(false);  // Disable the Inspect button
                    saveButton->setEnabled(false);
                }
            } else {
                maxSnapsStatus->setText("Checkerboard not detected in the frame.");
            }
        } else {
            maxSnapsStatus->setText("Failed to capture image from the camera.");
        }
    }

private:
    cv::VideoCapture cap;
    QLabel* maxSnapsStatus;
    QPushButton* snapButton;
    QPushButton* inspectButton;
    QPushButton* saveButton;
    QSpinBox* maxSnapsInput;
    QSpinBox* gridColsInput;
    QSpinBox* gridRowsInput;
    QSpinBox* gridPitchInput;

    int snapsTaken;
    std::vector<std::vector<cv::Point3f>> objpoints;  // 3D object points
    std::vector<std::vector<cv::Point2f>> imgpoints;  // 2D image points
};

