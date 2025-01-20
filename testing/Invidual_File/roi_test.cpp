#include <opencv2/opencv.hpp>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>
#include <QTimer>
#include <QDebug>

// Global Parameters
const int imageHeight = 500;
const int imageWidth = 800;
int linePositionYStart = imageHeight / 2;
int linePositionYEnd = imageHeight / 2;
int lineThickness = 2;
int pointPosition = imageWidth / 2;
int bottomLineY = imageHeight;
int bottomPointPosition = imageWidth / 2;

void drawROI(cv::Mat &frame) {
    cv::Mat img = frame.clone();

    // Draw the horizontal white line
    cv::line(img, cv::Point(0, linePositionYStart), cv::Point(imageWidth - 1, linePositionYEnd), cv::Scalar(255, 255, 255), lineThickness);

    // Calculate and draw the point on the first line
    int px = pointPosition;
    int py = linePositionYStart + (pointPosition * (linePositionYEnd - linePositionYStart)) / imageWidth;
    px = std::clamp(px, 0, imageWidth - 1);
    py = std::clamp(py, 0, imageHeight - 1);

    // Calculate and draw the point on the bottom line
    int bottomPx = bottomPointPosition;
    bottomPx = std::clamp(bottomPx, 0, imageWidth - 1);

    // Draw the line connecting both points
    cv::line(img, cv::Point(px, py), cv::Point(bottomPx, bottomLineY), cv::Scalar(0, 0, 0), 3);

    cv::imshow("User Viewpoint", img);
}

class ControlWindow : public QWidget {
    Q_OBJECT

public:
    ControlWindow() {
        this->setWindowTitle("Line Controller");

        QHBoxLayout *mainLayout = new QHBoxLayout();

        // Start Y-Axis Slider
        QVBoxLayout *startYAxisLayout = new QVBoxLayout();
        QLabel *startYAxisLabel = new QLabel("Start Y-Axis");
        startYAxisLayout->addWidget(startYAxisLabel);
        QSlider *startYAxisSlider = new QSlider(Qt::Vertical);
        startYAxisSlider->setRange(0, imageHeight);
        startYAxisSlider->setValue(imageHeight - linePositionYStart);
        connect(startYAxisSlider, &QSlider::valueChanged, [&](int value) {
            linePositionYStart = imageHeight - value;
            startYAxisLabel->setText(QString("Start Y-Axis: %1").arg(linePositionYStart));
        });
        startYAxisLayout->addWidget(startYAxisSlider);

        mainLayout->addLayout(startYAxisLayout);

        // End Y-Axis Slider
        QVBoxLayout *endYAxisLayout = new QVBoxLayout();
        QLabel *endYAxisLabel = new QLabel("End Y-Axis");
        endYAxisLayout->addWidget(endYAxisLabel);
        QSlider *endYAxisSlider = new QSlider(Qt::Vertical);
        endYAxisSlider->setRange(0, imageHeight);
        endYAxisSlider->setValue(imageHeight - linePositionYEnd);
        connect(endYAxisSlider, &QSlider::valueChanged, [&](int value) {
            linePositionYEnd = imageHeight - value;
            endYAxisLabel->setText(QString("End Y-Axis: %1").arg(linePositionYEnd));
        });
        endYAxisLayout->addWidget(endYAxisSlider);

        mainLayout->addLayout(endYAxisLayout);

        // Point Position Slider
        QVBoxLayout *pointPositionLayout = new QVBoxLayout();
        QLabel *pointPositionLabel = new QLabel("Point Position");
        pointPositionLayout->addWidget(pointPositionLabel);
        QSlider *pointPositionSlider = new QSlider(Qt::Horizontal);
        pointPositionSlider->setRange(0, imageWidth);
        pointPositionSlider->setValue(pointPosition);
        connect(pointPositionSlider, &QSlider::valueChanged, [&](int value) {
            pointPosition = value;
            pointPositionLabel->setText(QString("Point Position: %1").arg(pointPosition));
        });
        pointPositionLayout->addWidget(pointPositionSlider);

        mainLayout->addLayout(pointPositionLayout);

        // Bottom Point Position Slider
        QVBoxLayout *bottomPointPositionLayout = new QVBoxLayout();
        QLabel *bottomPointPositionLabel = new QLabel("Bottom Point Position");
        bottomPointPositionLayout->addWidget(bottomPointPositionLabel);
        QSlider *bottomPointPositionSlider = new QSlider(Qt::Horizontal);
        bottomPointPositionSlider->setRange(0, imageWidth);
        bottomPointPositionSlider->setValue(bottomPointPosition);
        connect(bottomPointPositionSlider, &QSlider::valueChanged, [&](int value) {
            bottomPointPosition = value;
            bottomPointPositionLabel->setText(QString("Bottom Point Position: %1").arg(bottomPointPosition));
        });
        bottomPointPositionLayout->addWidget(bottomPointPositionSlider);

        mainLayout->addLayout(bottomPointPositionLayout);

        this->setLayout(mainLayout);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ControlWindow controlWindow;
    controlWindow.show();

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        qDebug() << "Error: Could not open camera.";
        return -1;
    }

    cv::Mat frame;
    QTimer timer;

    QObject::connect(&timer, &QTimer::timeout, [&]() {
        cap >> frame;
        if (frame.empty()) {
            qDebug() << "Error: Could not read frame.";
            timer.stop();
            return;
        }

        cv::resize(frame, frame, cv::Size(imageWidth, imageHeight));
        drawROI(frame);
    });

    timer.start(30); // Refresh every 30 ms

    return app.exec();
}
#include "roi_test.moc"