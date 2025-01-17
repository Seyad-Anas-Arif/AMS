/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *mainGuiWindow;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *Default_btn;
    QPushButton *save_btn;
    QPushButton *apply_btn;
    QPushButton *run_btn;
    QPushButton *exit_btn;
    QSlider *X_end_slider;
    QLabel *label_9;
    QPushButton *info_btn;
    QSlider *offset_slider;
    QPushButton *delete_btn;
    QLabel *x_start;
    QPushButton *Developer_btn;
    QLabel *label_8;
    QSlider *x_start_slider;
    QSlider *y_end_slider;
    QLabel *label_7;
    QComboBox *settings_comboBox;
    QLabel *label_4;
    QSlider *y_start_slider;
    QSpinBox *exposure_spinbox;
    QLabel *x_end;
    QLabel *label_5;
    QLabel *Display_lable;
    QLabel *y_end;
    QFrame *line;
    QWidget *resultWindow;
    QLabel *label_17;
    QPushButton *rw_back_btn;
    QWidget *developerWindow;
    QLabel *label_18;
    QPushButton *dw_back_btn;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1132, 637);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setGeometry(QRect(0, -10, 1081, 611));
        mainGuiWindow = new QWidget();
        mainGuiWindow->setObjectName(QString::fromUtf8("mainGuiWindow"));
        horizontalLayoutWidget = new QWidget(mainGuiWindow);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(510, 570, 571, 31));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        Default_btn = new QPushButton(horizontalLayoutWidget);
        Default_btn->setObjectName(QString::fromUtf8("Default_btn"));

        horizontalLayout->addWidget(Default_btn);

        save_btn = new QPushButton(horizontalLayoutWidget);
        save_btn->setObjectName(QString::fromUtf8("save_btn"));

        horizontalLayout->addWidget(save_btn);

        apply_btn = new QPushButton(horizontalLayoutWidget);
        apply_btn->setObjectName(QString::fromUtf8("apply_btn"));

        horizontalLayout->addWidget(apply_btn);

        run_btn = new QPushButton(horizontalLayoutWidget);
        run_btn->setObjectName(QString::fromUtf8("run_btn"));

        horizontalLayout->addWidget(run_btn);

        exit_btn = new QPushButton(horizontalLayoutWidget);
        exit_btn->setObjectName(QString::fromUtf8("exit_btn"));

        horizontalLayout->addWidget(exit_btn);

        X_end_slider = new QSlider(mainGuiWindow);
        X_end_slider->setObjectName(QString::fromUtf8("X_end_slider"));
        X_end_slider->setGeometry(QRect(660, 380, 160, 16));
        // X_end_slider->setValue(50);
        X_end_slider->setOrientation(Qt::Horizontal);
        label_9 = new QLabel(mainGuiWindow);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(510, 510, 131, 20));
        info_btn = new QPushButton(mainGuiWindow);
        info_btn->setObjectName(QString::fromUtf8("info_btn"));
        info_btn->setGeometry(QRect(1050, 40, 20, 20));
        offset_slider = new QSlider(mainGuiWindow);
        offset_slider->setObjectName(QString::fromUtf8("offset_slider"));
        offset_slider->setGeometry(QRect(660, 430, 160, 16));
        offset_slider->setValue(50);
        offset_slider->setOrientation(Qt::Horizontal);
        offset_slider->setTickPosition(QSlider::NoTicks);
        offset_slider->setTickInterval(0);
        delete_btn = new QPushButton(mainGuiWindow);
        delete_btn->setObjectName(QString::fromUtf8("delete_btn"));
        delete_btn->setGeometry(QRect(750, 510, 51, 21));
        QFont font;
        font.setPointSize(9);
        delete_btn->setFont(font);
        x_start = new QLabel(mainGuiWindow);
        x_start->setObjectName(QString::fromUtf8("x_start"));
        x_start->setGeometry(QRect(510, 340, 67, 17));
        Developer_btn = new QPushButton(mainGuiWindow);
        Developer_btn->setObjectName(QString::fromUtf8("Developer_btn"));
        Developer_btn->setGeometry(QRect(1050, 60, 31, 21));
        Developer_btn->setMouseTracking(true);
        label_8 = new QLabel(mainGuiWindow);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(510, 470, 131, 21));
        x_start_slider = new QSlider(mainGuiWindow);
        x_start_slider->setObjectName(QString::fromUtf8("x_start_slider"));
        x_start_slider->setGeometry(QRect(660, 340, 160, 16));
        // x_start_slider->setValue(50);
        x_start_slider->setOrientation(Qt::Horizontal);
        y_end_slider = new QSlider(mainGuiWindow);
        y_end_slider->setObjectName(QString::fromUtf8("y_end_slider"));
        y_end_slider->setGeometry(QRect(790, 140, 16, 121));
        // y_end_slider->setValue(50);
        y_end_slider->setOrientation(Qt::Vertical);
        label_7 = new QLabel(mainGuiWindow);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(510, 430, 91, 20));
        settings_comboBox = new QComboBox(mainGuiWindow);
        settings_comboBox->setObjectName(QString::fromUtf8("settings_comboBox"));
        settings_comboBox->setGeometry(QRect(660, 510, 86, 25));
        // settings_comboBox->setCurrentIndex(0);
        settings_comboBox->addItem("custom");
        label_4 = new QLabel(mainGuiWindow);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(510, 100, 311, 17));
        y_start_slider = new QSlider(mainGuiWindow);
        y_start_slider->setObjectName(QString::fromUtf8("y_start_slider"));
        y_start_slider->setGeometry(QRect(570, 140, 16, 121));
        // y_start_slider->setValue(50);
        y_start_slider->setOrientation(Qt::Vertical);
        exposure_spinbox = new QSpinBox(mainGuiWindow);
        exposure_spinbox->setObjectName(QString::fromUtf8("exposure_spinbox"));
        exposure_spinbox->setGeometry(QRect(660, 470, 50, 26));
        exposure_spinbox->setMaximum(600);
        exposure_spinbox->setSingleStep(50);
        exposure_spinbox->setValue(100);
        x_end = new QLabel(mainGuiWindow);
        x_end->setObjectName(QString::fromUtf8("x_end"));
        x_end->setGeometry(QRect(510, 380, 67, 17));
        label_5 = new QLabel(mainGuiWindow);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(550, 270, 67, 17));
        Display_lable = new QLabel(mainGuiWindow);
        Display_lable->setObjectName(QString::fromUtf8("Display_lable"));
        Display_lable->setGeometry(QRect(40, 40, 421, 511));
        y_end = new QLabel(mainGuiWindow);
        y_end->setObjectName(QString::fromUtf8("y_end"));
        y_end->setGeometry(QRect(770, 270, 67, 17));
        line = new QFrame(mainGuiWindow);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(490, 30, 20, 581));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        stackedWidget->addWidget(mainGuiWindow);
        resultWindow = new QWidget();
        resultWindow->setObjectName(QString::fromUtf8("resultWindow"));
        label_17 = new QLabel(resultWindow);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(30, 60, 471, 511));
        rw_back_btn = new QPushButton(resultWindow);
        rw_back_btn->setObjectName(QString::fromUtf8("rw_back_btn"));
        rw_back_btn->setGeometry(QRect(980, 560, 91, 31));
        stackedWidget->addWidget(resultWindow);
        developerWindow = new QWidget();
        developerWindow->setObjectName(QString::fromUtf8("developerWindow"));
        label_18 = new QLabel(developerWindow);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(320, 260, 451, 21));
        dw_back_btn = new QPushButton(developerWindow);
        dw_back_btn->setObjectName(QString::fromUtf8("dw_back_btn"));
        dw_back_btn->setGeometry(QRect(930, 520, 91, 31));
        stackedWidget->addWidget(developerWindow);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1132, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        // Explain about button purpose when it is hovered
        Default_btn->setToolTip(QCoreApplication::translate("MainWindow", "Reset to default settings", nullptr));
        save_btn->setToolTip(QCoreApplication::translate("MainWindow", "Save current settings", nullptr));
        apply_btn->setToolTip(QCoreApplication::translate("MainWindow", "Apply current settings", nullptr));
        run_btn->setToolTip(QCoreApplication::translate("MainWindow", "to see result", nullptr));
        exit_btn->setToolTip(QCoreApplication::translate("MainWindow", "Exit the application", nullptr));
        info_btn->setToolTip(QCoreApplication::translate("MainWindow", "Show information", nullptr));
        delete_btn->setToolTip(QCoreApplication::translate("MainWindow", "Delete setting", nullptr));
        Developer_btn->setToolTip(QCoreApplication::translate("MainWindow", "Open developer window", nullptr));
        rw_back_btn->setToolTip(QCoreApplication::translate("MainWindow", "Go back to main window", nullptr));
        dw_back_btn->setToolTip(QCoreApplication::translate("MainWindow", "Go back to main window", nullptr));
        x_start_slider->setToolTip(QCoreApplication::translate("MainWindow", "Set the X-Start value", nullptr));
        X_end_slider->setToolTip(QCoreApplication::translate("MainWindow", "Set the X-End value", nullptr));
        y_start_slider->setToolTip(QCoreApplication::translate("MainWindow", "Set the Y-Start value", nullptr));
        y_end_slider->setToolTip(QCoreApplication::translate("MainWindow", "Set the Y-End value", nullptr));
        offset_slider->setToolTip(QCoreApplication::translate("MainWindow", "Set the offset value", nullptr));



        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        Default_btn->setText(QCoreApplication::translate("MainWindow", "Default", nullptr));
        save_btn->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        apply_btn->setText(QCoreApplication::translate("MainWindow", "Apply", nullptr));
        run_btn->setText(QCoreApplication::translate("MainWindow", "Run", nullptr));
        exit_btn->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Settings                  :", nullptr));
        info_btn->setText(QCoreApplication::translate("MainWindow", "?", nullptr));
        delete_btn->setText(QCoreApplication::translate("MainWindow", "Delete", nullptr));
        x_start->setText(QCoreApplication::translate("MainWindow", "X-Start", nullptr));
        Developer_btn->setText(QCoreApplication::translate("MainWindow", "D", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Camera Exposure :", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "OFFSET Line", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Intersection between die and bending sheet", nullptr));
        x_end->setText(QCoreApplication::translate("MainWindow", "X- End", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Y-Start", nullptr));
        Display_lable->setText(QString());
        y_end->setText(QCoreApplication::translate("MainWindow", "Y- End", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "Result_video_Display", nullptr));
        rw_back_btn->setText(QCoreApplication::translate("MainWindow", "Back", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "Welcome To developer window ", nullptr));
        dw_back_btn->setText(QCoreApplication::translate("MainWindow", "Back", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
