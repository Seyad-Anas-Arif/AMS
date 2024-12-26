#ifndef CAMERA_APP_H
#define CAMERA_APP_H

#include <QMainWindow>
#include <QThread>
#include <QImage>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QStackedWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class CameraThread : public QThread {
    Q_OBJECT

public:
    CameraThread();
    void run() override;
    void stop();
    void updateExposure(int value);
    void updateOffsetLineValue(int value);

signals:
    void imageMain(const QImage &image);
    void imageResult(const QImage &image);

private:
    bool running;
    int targetHeight;
    int targetWidth;
    int exposureValue;
    int offsetLineValue;
    bool plotPoint;
};

class ControlCameraWindow : public QMainWindow {
    Q_OBJECT

public:
    ControlCameraWindow();

private slots:
    void updateImage(const QImage &image);
    void updateYAxisStartValue(int value);
    void updateYAxisEndValue(int value);
    void updatePointPositionValue(int value);
    void updateBottomPointPositionValue(int value);
    void onExposureChange(int value);
    void updateOffsetLineValue(int value);
    void saveCurrentConfig();
    void comboBoxChanged(int index);
    void unsaveConfiguration();
    void restoreDefaultValues();
    void applyUnapplyChanges();
    void runAction();
    void resultLiveFeed(const QImage &image);
    void backToMainPage();
    void exitAction();
    void showHelpDialog();
    void showPasswordDialog();
    void enablePlotPoint();
    void disablePlotPoint();

private:
    void setupMainPage();
    void setupResultPage();
    void setupCameraCalibrationPage1();
    void loadSavedConfig(const std::map<std::string, int> &config);
    void updateSavedConfigsComboBox();
    void setSlidersEditable(bool editable);
    void setControlsEnabled(bool enabled);

    QLabel *label;
    QLabel *liveFeedLabel;
    QLabel *positionLabel;
    QSlider *yAxisStartSlider;
    QSlider *yAxisEndSlider;
    QSlider *pointPositionSlider;
    QSlider *bottomPointPositionSlider;
    QSlider *offsetSlider;
    QLabel *yAxisStartValueLabel;
    QLabel *yAxisEndValueLabel;
    QLabel *pointPositionValueLabel;
    QLabel *bottomPointPositionValueLabel;
    QLabel *offsetValueLabel;
    QSpinBox *exposureSpinBox;
    QComboBox *comboBox;
    QPushButton *defaultButton;
    QPushButton *saveButton;
    QPushButton *applyButton;
    QPushButton *runButton;
    QPushButton *exitButton;
    QPushButton *unsaveButton;
    QStackedWidget *stackedWidget;
    QWidget *mainPage;
    QWidget *resultPage;
    QWidget *cameraCalibrationPage1;
    CameraThread *cameraThread;
    std::string positionText;
    std::map<std::string, std::map<std::string, int>> savedConfigs;
};

#endif // CAMERA_APP_H