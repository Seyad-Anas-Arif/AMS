#include "../inc/mainwindow.h"

using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cameraThread(new CameraThread)
{
    ui->setupUi(this);
    
    // Connect signals from CameraThread to slots in MainWindow
    connect(cameraThread, &CameraThread::imageMain, this, &MainWindow::updateMainImage);
    connect(cameraThread, &CameraThread::imageResult, this, &MainWindow::updateMainImage);

    // Connect the run button to the slot
    connect(ui->run_btn, &QPushButton::clicked, this, &MainWindow::onRunButtonClicked);

    connect(ui->Developer_btn, &QPushButton::clicked, this, &MainWindow::onDeveloperbuttonClicked);

    // Connect the back button to the slot
    connect(ui->rw_back_btn, &QPushButton::clicked, this, &MainWindow::rwBackButtonClicked);

    connect(ui->dw_back_btn, &QPushButton::clicked, this, &MainWindow::dwBackButtonClicked);

    connect(ui->exit_btn, &QPushButton::clicked, qApp, &QApplication::quit);

    // Connect sliders or other UI elements to update exposure and offset line values
    connect(ui->exposure_spinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::updateExposure);
    connect(ui->offset_slider, static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged), this, &MainWindow::updateOffsetLineValue);

    // Start the camera thread
    cameraThread->start();
}

MainWindow::~MainWindow()
{
    cameraThread->stop();
    cameraThread->wait();
    delete cameraThread;
    delete ui;
}

void MainWindow::updateMainImage(const QImage &frame)
{
    ui->Display_lable->setPixmap(QPixmap::fromImage(frame).scaled(ui->Display_lable->size(), Qt::KeepAspectRatio));
}

void MainWindow::updateResultImage(const QImage &frame)
{
    // Update the result image label (if you have one)
    // ui->Result_lable->setPixmap(QPixmap::fromImage(frame).scaled(ui->Result_lable->size(), Qt::KeepAspectRatio));
}

void MainWindow::onRunButtonClicked()
{
    // Switch to the result window
    ui->stackedWidget->setCurrentWidget(ui->resultWindow);
}

void MainWindow::onDeveloperbuttonClicked()
{
    // Switch to the developer window
    ui->stackedWidget->setCurrentWidget(ui->developerWindow);
}

void MainWindow::rwBackButtonClicked()
{
    // Switch back to the main GUI window
    ui->stackedWidget->setCurrentWidget(ui->mainGuiWindow);
}

void MainWindow::dwBackButtonClicked()
{
    // Switch back to the main GUI window
    ui->stackedWidget->setCurrentWidget(ui->mainGuiWindow);
}

void MainWindow::updateExposure(int value)
{
    cameraThread->updateExposure(value);
}

void MainWindow::updateOffsetLineValue(int value)
{
    cameraThread->updateOffsetLineValue(value);
}