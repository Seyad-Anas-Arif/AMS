#ifndef CAMERAAPP_H
#define CAMERAAPP_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <QImage>
#include <QPixmap>

class CameraApp : public QWidget
{
    Q_OBJECT

public:
    explicit CameraApp(QWidget *parent = nullptr);
    ~CameraApp();

private slots:
    void updateFrame();
    void setSettings();
    void snapImage();
    void goToInspectPage();
    void saveCalibrationData();
    void updateInspectionFrame();
    void goToMainPage();
    void closeApp();

private:
    void initUI();
    void createMainPage();
    void createInspectionPage();
    void displayImage(const cv::Mat &img, QLabel *label);
    
    bool calibrationDone;
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Mat newCameraMtx;
    
    int gridRows;
    int gridCols;
    int gridPitch;
    int maxSnaps;
    int snapsTaken;
    
    std::vector<std::vector<cv::Point3f>> objPoints;
    std::vector<std::vector<cv::Point2f>> imgPoints;

    QLabel *videoLabel;
    QLabel *distortedLabel;
    QLabel *undistortedLabel;
    QLabel *maxSnapsStatus;
    QSpinBox *gridRowsInput;
    QSpinBox *gridColsInput;
    QDoubleSpinBox *gridPitchInput;
    QSpinBox *maxSnapsInput;
    
    QStackedWidget *stackedWidget;
    QWidget *page1;
    QWidget *page2;
    
    QPushButton *setButton;
    QPushButton *snapButton;
    QPushButton *inspectButton;
    QPushButton *saveButton;
    QPushButton *backButton;
    QPushButton *backToMainButton;
    
    QTimer *timer;
    cv::VideoCapture cap;
};

#endif // CAMERAAPP_H
