#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"  // Include the generated header for Ui::MainWindow
#include <QDebug>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void setDefaultSettings();
    void saveSettings();
    void applySettings();
    void runOperation();

private:
    Ui::MainWindow *ui;  // Pointer to the Ui_MainWindow object
};

#endif // MAINWINDOW_H
