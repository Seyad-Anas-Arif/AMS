#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ControlWindow : public QWidget {
    Q_OBJECT

public:
    ControlWindow(QWidget *parent = nullptr);

public slots:
    void updateSliderValue(int value);

private:
    QSlider *createSlider(int min, int max, int value, const char *slot);
};

#endif // CONTROLWINDOW_H
