#ifndef CAMERAAPP_H
#define CAMERAAPP_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QStackedWidget>
#include <QThread>
#include <opencv2/opencv.hpp>

class CameraApp : public QWidget {
    Q_OBJECT

public:
    explicit CameraApp(QWidget *parent = nullptr);
    ~CameraApp();

private slots:
    void setSettings();
    void snapImage();
    void goToInspectPage();
    void goToMainPage();
    void closeApp();
    void updateVideoFrame(const QImage &frame);
    void updateDistortedFrame(const QImage &frame);
    void updateUndistortedFrame(const QImage &frame);
    void saveCalibrationData();

private:
    void createMainPage();
    void createInspectionPage();

    QStackedWidget *stackedWidget;
    QWidget *page1, *page2;

    QLabel *videoLabel;
    QLabel *distortedLabel, *undistortedLabel;

    QSpinBox *gridRowsInput, *gridColsInput, *maxSnapsInput;
    QDoubleSpinBox *gridPitchInput;
    QLabel *maxSnapsStatus;

    QPushButton *setButton, *snapButton, *inspectButton, *saveButton, *backButton;

    // Camera thread and control flag
    std::thread cameraThread;
    std::atomic<bool> stopFlag;

    void captureFrames();  // Method to capture frames in the background thread
};

#endif // CAMERAAPP_H
