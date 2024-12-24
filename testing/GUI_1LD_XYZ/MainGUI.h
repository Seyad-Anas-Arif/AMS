#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QFormLayout>
#include <QComboBox>
#include <QFont>

class ControlCameraWindow : public QMainWindow {
    Q_OBJECT

public:
    ControlCameraWindow(QWidget *parent = nullptr);

private:
    void setupMainPage();
    void setupResultPage();
    void setupCameraCalibrationPage1();
    void showHelpDialog();
    void showPasswordDialog();
    void restoreDefaultValues();
    void saveCurrentConfig();
    void applyUnapplyChanges();
    void runAction();
    void exitAction();
    void updateYAxisStartValue(int value);
    void updateYAxisEndValue(int value);
    void updatePointPositionValue(int value);
    void updateBottomPointPositionValue(int value);
    void updateOffsetLineValue(int value);
    void updateOffsetLabel(int value);
    void enablePlotPoint();
    void disablePlotPoint();
    void onExposureChange(int value);
    void comboBoxChanged(int index);
    void unsaveConfiguration();

    QStackedWidget *stackedWidget;
    QWidget *mainPage;
    QWidget *resultPage;
    QWidget *cameraCalibrationPage1;
    QWidget *cameraCalibrationPage2;

    QLabel *cameraFeedLabel;
    QSlider *yAxisStartSlider;
    QSlider *yAxisEndSlider;
    QSlider *pointPositionSlider;
    QSlider *bottomPointPositionSlider;
    QSlider *offsetSlider;
    QSpinBox *exposureSpinBox;
    QComboBox *comboBox;
    QPushButton *saveButton;
    QPushButton *defaultButton;
    QPushButton *applyButton;
    QPushButton *runButton;
    QPushButton *exitButton;
    QLabel *offsetValueLabel;
    QLabel *yAxisStartValueLabel;
    QLabel *yAxisEndValueLabel;
    QLabel *pointPositionValueLabel;
    QLabel *bottomPointPositionValueLabel;

    int imageWidth = 700;
    int imageHeight = 400;
    int linePositionYStart = 50;
    int linePositionYEnd = 100;
    int pointPosition = 200;
    int bottomPointPosition = 300;
};

#endif // MAINGUI_H
