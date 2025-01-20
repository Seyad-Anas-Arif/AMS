#include "ControlWindow.h"
#include "roi.h"
#include <QDebug>

ControlWindow::ControlWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Line Controller");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    auto createSliderLayout = [this](const QString &labelText, int min, int max, int value, const char *slot) {
        QVBoxLayout *layout = new QVBoxLayout;
        QLabel *label = new QLabel(labelText, this);
        QSlider *slider = createSlider(min, max, value, slot);
        layout->addWidget(label);
        layout->addWidget(slider);
        return layout;
    };

    mainLayout->addLayout(createSliderLayout("Start Y-Axis", 0, image_height, image_height - line_position_y_start, SLOT(update_y_axis_start(int))));
    mainLayout->addLayout(createSliderLayout("End Y-Axis", 0, image_height, image_height - line_position_y_end, SLOT(update_y_axis_end(int))));
    mainLayout->addLayout(createSliderLayout("Point Position", 0, image_width, point_position, SLOT(update_point_position(int))));
    mainLayout->addLayout(createSliderLayout("Bottom Point Position", 0, image_width, bottom_point_position, SLOT(update_bottom_point_position(int))));

    setLayout(mainLayout);
}

QSlider *ControlWindow::createSlider(int min, int max, int value, const char *slot) {
    QSlider *slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(min, max);
    slider->setValue(value);
   connect(slider, &QAbstractSlider::valueChanged, this, &ControlWindow::updateSliderValue);

    return slider;
}
void ControlWindow::updateSliderValue(int value) {
    // Define the behavior for updating the slider value
    // Example:
    qDebug() << "Slider value updated to:" << value;
}
