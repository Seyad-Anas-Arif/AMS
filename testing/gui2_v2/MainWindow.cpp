#include "MainWindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), stackedWidget(new QStackedWidget) {
    // Central Widget and Layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // UI Components
    QLabel *mainLabel = new QLabel("Main Page: Adjust sliders and click buttons", this);
    QSlider *slider1 = new QSlider(Qt::Horizontal, this);
    QSlider *slider2 = new QSlider(Qt::Horizontal, this);
    QPushButton *defaultButton = new QPushButton("Default", this);
    QPushButton *saveButton = new QPushButton("Save", this);
    QPushButton *applyButton = new QPushButton("Apply", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(defaultButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(applyButton);

    mainLayout->addWidget(mainLabel);
    mainLayout->addWidget(slider1);
    mainLayout->addWidget(slider2);
    mainLayout->addLayout(buttonLayout);

    QPushButton *btnWidget1 = new QPushButton("Show Widget 1", this);
    QPushButton *btnWidget2 = new QPushButton("Show Widget 2", this);
    QPushButton *btnWidget3 = new QPushButton("Show Widget 3", this);

    mainLayout->addWidget(btnWidget1);
    mainLayout->addWidget(btnWidget2);
    mainLayout->addWidget(btnWidget3);

    // Stacked Widget Pages
    QWidget *widget1 = new QWidget;
    QLabel *label1 = new QLabel("Widget 1: Label", widget1);
    QVBoxLayout *layout1 = new QVBoxLayout(widget1);
    layout1->addWidget(label1);

    QWidget *widget2 = new QWidget;
    QPushButton *button2 = new QPushButton("Widget 2: Button", widget2);
    QVBoxLayout *layout2 = new QVBoxLayout(widget2);
    layout2->addWidget(button2);

    QWidget *widget3 = new QWidget;
    QLineEdit *lineEdit3 = new QLineEdit("Widget 3: Line Edit", widget3);
    QVBoxLayout *layout3 = new QVBoxLayout(widget3);
    layout3->addWidget(lineEdit3);

    stackedWidget->addWidget(widget1);
    stackedWidget->addWidget(widget2);
    stackedWidget->addWidget(widget3);

    mainLayout->addWidget(stackedWidget);

    connect(btnWidget1, &QPushButton::clicked, this, &MainWindow::showWidget1);
    connect(btnWidget2, &QPushButton::clicked, this, &MainWindow::showWidget2);
    connect(btnWidget3, &QPushButton::clicked, this, &MainWindow::showWidget3);

    setCentralWidget(centralWidget);
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::showWidget1() {
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::showWidget2() {
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::showWidget3() {
    stackedWidget->setCurrentIndex(2);
}
