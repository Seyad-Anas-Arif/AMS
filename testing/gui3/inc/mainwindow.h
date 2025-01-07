#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include "CameraThread.h"

class CameraThread; // Forward declaration

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateMainImage(const QImage &frame);
    void updateResultImage(const QImage &frame);
    void onRunButtonClicked();
    void onDeveloperbuttonClicked();
    void rwBackButtonClicked();
    void dwBackButtonClicked();
    void updateExposure(int value);
    void updateOffsetLineValue(int value);

private:
    Ui::MainWindow *ui;
    
    CameraThread *cameraThread;
};

std::tuple<cv::Mat, int, int, int, int, int, int, int> UsersLiveFeed(const cv::Mat& frame, int line_position_y_start, int line_position_y_end, int point_position, int bottom_point_position, int px, int py, int bottom_px);

#endif // MAINWINDOW_H