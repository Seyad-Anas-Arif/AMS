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
#include <QtGui/QIcon>
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
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1042, 615);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        Display_lable = new QLabel(centralwidget);
        Display_lable->setObjectName(QString::fromUtf8("Display_lable"));
        Display_lable->setGeometry(QRect(20, 30, 421, 511));
        y_start_slider = new QSlider(centralwidget);
        y_start_slider->setObjectName(QString::fromUtf8("y_start_slider"));
        y_start_slider->setGeometry(QRect(530, 130, 16, 121));
        y_start_slider->setOrientation(Qt::Vertical);
        y_end_slider = new QSlider(centralwidget);
        y_end_slider->setObjectName(QString::fromUtf8("y_end_slider"));
        y_end_slider->setGeometry(QRect(750, 130, 16, 121));
        y_end_slider->setOrientation(Qt::Vertical);
        x_start_slider = new QSlider(centralwidget);
        x_start_slider->setObjectName(QString::fromUtf8("x_start_slider"));
        x_start_slider->setGeometry(QRect(620, 330, 160, 16));
        x_start_slider->setOrientation(Qt::Horizontal);
        X_end_slider = new QSlider(centralwidget);
        X_end_slider->setObjectName(QString::fromUtf8("X_end_slider"));
        X_end_slider->setGeometry(QRect(620, 370, 160, 16));
        X_end_slider->setOrientation(Qt::Horizontal);
        offset_slider = new QSlider(centralwidget);
        offset_slider->setObjectName(QString::fromUtf8("offset_slider"));
        offset_slider->setGeometry(QRect(620, 420, 160, 16));
        offset_slider->setOrientation(Qt::Horizontal);
        exposure_spinbox = new QSpinBox(centralwidget);
        exposure_spinbox->setObjectName(QString::fromUtf8("exposure_spinbox"));
        exposure_spinbox->setGeometry(QRect(620, 460, 44, 26));
        FIlter_comboBox = new QComboBox(centralwidget);
        FIlter_comboBox->setObjectName(QString::fromUtf8("FIlter_comboBox"));
        FIlter_comboBox->setGeometry(QRect(620, 500, 86, 25));
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(470, 530, 561, 31));
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

        Developer_btn = new QPushButton(centralwidget);
        Developer_btn->setObjectName(QString::fromUtf8("Developer_btn"));
        Developer_btn->setGeometry(QRect(1010, 0, 31, 21));
        Developer_btn->setMouseTracking(true);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../../Downloads/9042044_user_male_circle_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        Developer_btn->setIcon(icon);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(470, 90, 311, 17));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(510, 260, 67, 17));
        y_end = new QLabel(centralwidget);
        y_end->setObjectName(QString::fromUtf8("y_end"));
        y_end->setGeometry(QRect(730, 260, 67, 17));
        x_start = new QLabel(centralwidget);
        x_start->setObjectName(QString::fromUtf8("x_start"));
        x_start->setGeometry(QRect(470, 330, 67, 17));
        x_end = new QLabel(centralwidget);
        x_end->setObjectName(QString::fromUtf8("x_end"));
        x_end->setGeometry(QRect(470, 370, 67, 17));
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(470, 420, 91, 20));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(470, 460, 131, 21));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(470, 500, 91, 20));
        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(450, 10, 20, 581));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(470, 300, 181, 17));
        label_11 = new QLabel(centralwidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(470, 400, 271, 17));
        result_window = new QStackedWidget(centralwidget);
        result_window->setObjectName(QString::fromUtf8("result_window"));
        result_window->setGeometry(QRect(0, 0, 1041, 591));
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        result_window->addWidget(page_3);
        page_4 = new QWidget();
        page_4->setObjectName(QString::fromUtf8("page_4"));
        result_window->addWidget(page_4);
        MainWindow->setCentralWidget(centralwidget);
        result_window->raise();
        Display_lable->raise();
        y_start_slider->raise();
        y_end_slider->raise();
        x_start_slider->raise();
        X_end_slider->raise();
        offset_slider->raise();
        exposure_spinbox->raise();
        FIlter_comboBox->raise();
        horizontalLayoutWidget->raise();
        Developer_btn->raise();
        label_2->raise();
        label_3->raise();
        y_end->raise();
        x_start->raise();
        x_end->raise();
        label_7->raise();
        label_8->raise();
        label_9->raise();
        line->raise();
        label_10->raise();
        label_11->raise();
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1042, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        Display_lable->setText(QString());
        Default_btn->setText(QCoreApplication::translate("MainWindow", "Default", nullptr));
        save_btn->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        apply_btn->setText(QCoreApplication::translate("MainWindow", "Apply", nullptr));
        run_btn->setText(QCoreApplication::translate("MainWindow", "Run", nullptr));
        exit_btn->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        Developer_btn->setText(QString());
        label_2->setText(QCoreApplication::translate("MainWindow", "Intersection between die and bending sheet", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Y-Start", nullptr));
        y_end->setText(QCoreApplication::translate("MainWindow", "Y- End", nullptr));
        x_start->setText(QCoreApplication::translate("MainWindow", "X-Start", nullptr));
        x_end->setText(QCoreApplication::translate("MainWindow", "X- End", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "OFFSET Line", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Camera Exposure :", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Filter Type :", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "Laser below Intersection", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "OFFSET of the laser in metel sheet", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
