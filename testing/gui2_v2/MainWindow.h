#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    // Central widget and layouts
    QWidget *centralWidget;
    
    // Main page Componanents
    QLabel *Display_lable;
    QSlider *y_start_slider;
    QSlider *y_end_slider;
    QSlider *x_start_slider;
    QSlider *X_end_slider;
    QSlider *offset_slider;
    QSpinBox *exposure_spinbox;
    QComboBox *FIlter_comboBox;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *Default_btn;
    QPushButton *save_btn;
    QPushButton *apply_btn;
    QPushButton *run_btn;
    QPushButton *exit_btn;
    QPushButton *Developer_btn;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *y_end;
    QLabel *x_start;
    QLabel *x_end;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QFrame *line;
    QLabel *label_10;
    QLabel *label_11;
    QStackedWidget *result_window;
    QWidget *page_3;
    QWidget *page_4;

    // Stacked widget and pages
    QStackedWidget *stackedWidget;
    QWidget *widget1;   // Mainwindow
    QWidget *widget2;   // Result window
    QWidget *widget3;   // Developer Window

    // Functions to switch between widgets
    void setupUI();
    void createConnections();
    void setupPages();

private slots:
    void showWidget1();
    void showWidget2();
    void showWidget3();
};

#endif // MAINWINDOW_H
