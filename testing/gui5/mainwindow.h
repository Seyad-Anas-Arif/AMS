#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include "ui_mainwindow.h"

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
    void onRunButtonClicked();
    void onDeveloperbuttonClicked();
    void rwBackButtonClicked();
    void dwBackButtonClicked();
    void updateExposure(int value);
    void updateOffsetLineValue(int value);

    void updateFrame(const QImage &frame);

private:
    Ui::MainWindow *ui;
    std::thread cameraThread;
    std::atomic<bool> stopFlag;
    int targetWidth = 320;
    int targetHeight = 240;
    int exposureValue = 100;
    int offsetLineValue = 130;
    bool plotPoint = false;

    void captureFrames();  // Method to capture frames in the background thread
};

#endif // MAINWINDOW_H