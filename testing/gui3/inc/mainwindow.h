#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMap>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <tuple>
#include <QSlider>

#include "CameraThread.h"
#include "ui_mainwindow.h" 

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
    void configureSlider(QSlider *slider, int min, int max, int initialValue);
    

private slots:
    void updateMainImage(const QImage &frame);
    void updateResultImage(const QImage &frame);
    void updateExposure(int value);
    void updateOffsetLineValue(int value);
    //Buttions Actions
    void onDefaultButtonClicked();
    void onSaveButtonClicked();
    void onApplyButtontoggled();
    void onRunButtonClicked();
    void onDeveloperbuttonClicked();
    void rwBackButtonClicked();
    void dwBackButtonClicked();
    void onExitButtonClicked();
    void onDeleteButtonClicked();
    //change the value of 0the sliders and combobox
    
    void onXStartSliderChanged(int value);
    void onXEndSliderChanged(int value);
    void onYStartSliderChanged(int value);
    void onYEndSliderChanged(int value);
    void onOffsetSliderChanged(int value);
    void onSettingsComboBoxChanged(int index);
    void restoreDefaultValues();
    void setSlidersEditable(bool editable);
    void enableoffsetview();
    void disableoffsetview();
private:
    Ui::MainWindow *ui;
    CameraThread *cameraThread;
        
  
   
};

std::tuple<cv::Mat, int, int, int, int, int, int, int>UsersLiveFeed(const cv::Mat &frame);

#ifdef ENABLE_MAIN_DEBUG
    #define MAIN_FILE_DEBUG qDebug()
#else
    #define MAIN_FILE_DEBUG if (false) qDebug()
#endif


#endif // MAINWINDOW_H