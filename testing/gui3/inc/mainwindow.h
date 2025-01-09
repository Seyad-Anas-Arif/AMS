#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include "CameraThread.h"
#include "../inc/ui_mainwindow.h" 
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <tuple>

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

extern int image_height;
extern int image_width;

#endif // MAINWINDOW_H