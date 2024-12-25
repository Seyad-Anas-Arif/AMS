#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QStackedWidget>
#include <QThread>
#include <QImage>
#include <QPixmap>
#include <QFont>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <Eigen/Dense>

using namespace cv;
using namespace std;
using namespace Eigen;

int image_height, image_width;
int line_position_y_start, line_position_y_end, point_position, bottom_point_position;
int bottom_line_y;
int line_thickness = 2;
int default_line_position_y_start, default_line_position_y_end, default_point_position, default_bottom_point_position;
map<string, map<string, int>> saved_configs;

class CameraThread;

class ControlCameraWindow : public QMainWindow {
    Q_OBJECT

public:
    ControlCameraWindow(QWidget *parent = nullptr);
    ~ControlCameraWindow();

private slots:
    void update_image(const QImage &qt_image);
    void update_y_axis_start_value(int value);
    void update_y_axis_end_value(int value);
    void update_point_position_value(int value);
    void update_bottom_point_position_value(int value);
    void on_exposure_change(int value);
    void update_offset_line_value(int value);
    void save_current_config();
    void combo_box_changed(int index);
    void unsave_configuration();
    void restore_default_values();
    void apply_unapply_changes();
    void run_action();
    void result_live_feed(const QImage &qt_image);
    void back_to_main_page();
    void exit_action();
    void show_help_dialog();
    void show_password_dialog();
    void enable_plot_point();
    void disable_plot_point();
    void update_offset_label(int value);

private:
    void setup_main_page();
    void setup_result_page();
    void setup_camera_calibration_page_1();
    void set_sliders_editable(bool editable);
    void set_controls_enabled(bool enabled);
    void load_saved_config(const map<string, int> &config);
    void update_saved_configs_combobox();

    QLabel *label;
    QLabel *live_feed_label;
    QLabel *position_label;
    QLabel *y_axis_start_value_label;
    QLabel *y_axis_end_value_label;
    QLabel *point_position_value_label;
    QLabel *bottom_point_position_value_label;
    QLabel *offset_value_label;
    QSlider *y_axis_start_slider;
    QSlider *y_axis_end_slider;
    QSlider *point_position_slider;
    QSlider *bottom_point_position_slider;
    QSlider *offset_slider;
    QSpinBox *exposure_spinbox;
    QComboBox *combo_box;
    QPushButton *default_button;
    QPushButton *save_button;
    QPushButton *apply_button;
    QPushButton *run_button;
    QPushButton *exit_button;
    QPushButton *unsave_button;
    QStackedWidget *stacked_widget;
    QWidget *main_page;
    QWidget *result_page;
    QWidget *camera_calibration_page_1;
    CameraThread *camera_thread;
    QString position_text;
};

class CameraThread : public QThread {
    Q_OBJECT

public:
    CameraThread();
    ~CameraThread();
    void stop();
    void update_exposure(int value);
    void update_offset_line_value(int value);

signals:
    void image_main(const QImage &qt_image);
    void image_result(const QImage &qt_image);

protected:
    void run() override;

private:
    bool running;
    int target_height;
    int target_width;
    int exposure_value;
    int offset_line_value;
    bool plot_point;
};

ControlCameraWindow::ControlCameraWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Camera Application with Controls");
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    setFixedSize(700, 400);

    position_text = "";

    stacked_widget = new QStackedWidget();
    main_page = new QWidget();
    result_page = new QWidget();
    camera_calibration_page_1 = new QWidget();

    setCentralWidget(stacked_widget);
    stacked_widget->addWidget(main_page);
    stacked_widget->addWidget(result_page);
    stacked_widget->addWidget(camera_calibration_page_1);

    camera_thread = new CameraThread();
    connect(camera_thread, &CameraThread::image_main, this, &ControlCameraWindow::update_image);
    camera_thread->start();

    setup_main_page();
    setup_result_page();
    setup_camera_calibration_page_1();
}

ControlCameraWindow::~ControlCameraWindow() {
    camera_thread->stop();
    camera_thread->quit();
    camera_thread->wait();
    delete camera_thread;
}

void ControlCameraWindow::setup_main_page() {
    QHBoxLayout *main_layout = new QHBoxLayout(main_page);

    label = new QLabel();
    label->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(label);

    QVBoxLayout *control_layout = new QVBoxLayout();

    QFont font("Times", 10);
    int label_width = 200;
    int slider_width = 80;
    int slider_height = 10;

    QHBoxLayout *help_button_layout = new QHBoxLayout();
    QPushButton *help_button = new QPushButton("?", this);
    help_button->setFixedSize(20, 20);
    connect(help_button, &QPushButton::clicked, this, &ControlCameraWindow::show_help_dialog);
    help_button_layout->addWidget(help_button, 0, Qt::AlignRight);
    control_layout->addLayout(help_button_layout);

    QHBoxLayout *dev_button_layout = new QHBoxLayout();
    QPushButton *dev_options_button = new QPushButton("Developer Options", this);
    dev_options_button->setFixedSize(150, 20);
    connect(dev_options_button, &QPushButton::clicked, this, &ControlCameraWindow::show_password_dialog);
    dev_button_layout->addWidget(dev_options_button, 0, Qt::AlignRight);
    control_layout->addLayout(dev_button_layout);

    QHBoxLayout *y_axis_head = new QHBoxLayout();
    QLabel *y_axis_heading = new QLabel("Intersection between die and the bending sheet", this);
    y_axis_heading->setFont(font);
    y_axis_head->addWidget(y_axis_heading, 0, Qt::AlignLeft);
    control_layout->addLayout(y_axis_head);

    QHBoxLayout *y_axis_layout = new QHBoxLayout();

    QHBoxLayout *y_axis_start_layout = new QHBoxLayout();
    QLabel *y_axis_label_start = new QLabel("y_start", this);
    y_axis_label_start->setFont(font);
    y_axis_label_start->setFixedWidth(label_width);
    y_axis_start_layout->addWidget(y_axis_label_start, 0, Qt::AlignLeft);

    y_axis_start_slider = new QSlider(Qt::Vertical, this);
    y_axis_start_slider->setRange(0, image_height);
    y_axis_start_slider->setValue(image_height - line_position_y_start);
    y_axis_start_slider->setFixedSize(slider_height, slider_width);
    connect(y_axis_start_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_y_axis_start_value);
    y_axis_start_layout->addWidget(y_axis_start_slider, 0, Qt::AlignCenter);

    y_axis_start_value_label = new QLabel(QString::number(image_height - line_position_y_start), this);
    y_axis_start_value_label->setFont(font);
    y_axis_start_value_label->setFixedWidth(label_width);
    y_axis_start_layout->addWidget(y_axis_start_value_label, 0, Qt::AlignRight);

    QHBoxLayout *y_axis_end_layout = new QHBoxLayout();
    QLabel *y_axis_label_end = new QLabel("y_end", this);
    y_axis_label_end->setFont(font);
    y_axis_label_end->setFixedWidth(label_width);
    y_axis_end_layout->addWidget(y_axis_label_end, 0, Qt::AlignLeft);

    y_axis_end_slider = new QSlider(Qt::Vertical, this);
    y_axis_end_slider->setRange(0, image_height);
    y_axis_end_slider->setValue(image_height - line_position_y_end);
    y_axis_end_slider->setFixedSize(slider_height, slider_width);
    connect(y_axis_end_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_y_axis_end_value);
    y_axis_end_layout->addWidget(y_axis_end_slider, 0, Qt::AlignCenter);

    y_axis_end_value_label = new QLabel(QString::number(image_height - line_position_y_end), this);
    y_axis_end_value_label->setFont(font);
    y_axis_end_value_label->setFixedWidth(label_width);
    y_axis_end_layout->addWidget(y_axis_end_value_label, 0, Qt::AlignRight);

    y_axis_layout->addLayout(y_axis_start_layout);
    y_axis_layout->addLayout(y_axis_end_layout);
    control_layout->addLayout(y_axis_layout);

    QHBoxLayout *x_axis_head = new QHBoxLayout();
    QLabel *x_axis_heading = new QLabel("Laser below the Intersection", this);
    x_axis_heading->setFont(font);
    x_axis_head->addWidget(x_axis_heading, 0, Qt::AlignLeft);
    control_layout->addLayout(x_axis_head);

    QVBoxLayout *x_axis_layout = new QVBoxLayout();

    QHBoxLayout *point_position_layout = new QHBoxLayout();
    QLabel *point_position_label = new QLabel("x_start", this);
    point_position_label->setFont(font);
    point_position_label->setFixedWidth(label_width);
    point_position_layout->addWidget(point_position_label);

    point_position_slider = new QSlider(Qt::Horizontal, this);
    point_position_slider->setRange(0, image_width);
    point_position_slider->setValue(point_position);
    point_position_slider->setFixedSize(slider_width, slider_height);
    connect(point_position_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_point_position_value);
    point_position_layout->addWidget(point_position_slider);

    point_position_value_label = new QLabel(QString::number(point_position), this);
    point_position_value_label->setFont(font);
    point_position_value_label->setFixedWidth(label_width);
    point_position_layout->addWidget(point_position_value_label);
    x_axis_layout->addLayout(point_position_layout);

    QHBoxLayout *bottom_point_position_layout = new QHBoxLayout();
    QLabel *bottom_point_position_label = new QLabel("x_end", this);
    bottom_point_position_label->setFont(font);
    bottom_point_position_label->setFixedWidth(label_width);
    bottom_point_position_layout->addWidget(bottom_point_position_label);

    bottom_point_position_slider = new QSlider(Qt::Horizontal, this);
    bottom_point_position_slider->setRange(0, image_width);
    bottom_point_position_slider->setValue(bottom_point_position);
    bottom_point_position_slider->setFixedSize(slider_width, slider_height);
    connect(bottom_point_position_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_bottom_point_position_value);
    bottom_point_position_layout->addWidget(bottom_point_position_slider);

    bottom_point_position_value_label = new QLabel(QString::number(bottom_point_position), this);
    bottom_point_position_value_label->setFont(font);
    bottom_point_position_value_label->setFixedWidth(label_width);
    bottom_point_position_layout->addWidget(bottom_point_position_value_label);
    x_axis_layout->addLayout(bottom_point_position_layout);

    control_layout->addLayout(x_axis_layout);

    QHBoxLayout *head = new QHBoxLayout();
    QLabel *heading = new QLabel("Offset of the laser in the metal sheet", this);
    heading->setFont(font);
    head->addWidget(heading, 0, Qt::AlignLeft);
    control_layout->addLayout(head);

    offset_slider = new QSlider(Qt::Horizontal);
    offset_slider->setRange(0, image_width);
    offset_slider->setValue(camera_thread->offset_line_value);
    offset_slider->setFixedSize(slider_width, slider_height);
    connect(offset_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_offset_line_value);
    connect(offset_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_offset_label);
    connect(offset_slider, &QSlider::sliderPressed, this, &ControlCameraWindow::enable_plot_point);
    connect(offset_slider, &QSlider::sliderReleased, this, &ControlCameraWindow::disable_plot_point);

    offset_value_label = new QLabel(QString::number(camera_thread->offset_line_value));
    QHBoxLayout *slider_layout = new QHBoxLayout();
    slider_layout->addWidget(new QLabel("Offset Line"));
    slider_layout->addWidget(offset_slider);
    slider_layout->addWidget(offset_value_label);
    control_layout->addLayout(slider_layout);

    exposure_spinbox = new QSpinBox(this);
    exposure_spinbox->setMinimum(-13);
    exposure_spinbox->setMaximum(0);
    exposure_spinbox->setValue(-5);
    exposure_spinbox->setFixedSize(60, 25);
    exposure_spinbox->setSingleStep(1);
    exposure_spinbox->setKeyboardTracking(false);
    connect(exposure_spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ControlCameraWindow::on_exposure_change);
    QFormLayout *grid_layout = new QFormLayout();
    grid_layout->addRow("camera exposure:", exposure_spinbox);
    control_layout->addLayout(grid_layout);

    QHBoxLayout *combo_layout = new QHBoxLayout();
    combo_box = new QComboBox(this);
    combo_box->setFixedWidth(200);
    combo_box->addItem("Custom");
    connect(combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ControlCameraWindow::combo_box_changed);
    combo_layout->addWidget(combo_box, 0, Qt::AlignLeft);

    unsave_button = new QPushButton("-", this);
    unsave_button->setFixedWidth(30);
    connect(unsave_button, &QPushButton::clicked, this, &ControlCameraWindow::unsave_configuration);
    combo_layout->addWidget(unsave_button, 0, Qt::AlignLeft);
    control_layout->addLayout(combo_layout);

    QHBoxLayout *buttons_layout = new QHBoxLayout();
    default_button = new QPushButton("Default", this);
    connect(default_button, &QPushButton::clicked, this, &ControlCameraWindow::restore_default_values);
    buttons_layout->addWidget(default_button);

    save_button = new QPushButton("Save", this);
    connect(save_button, &QPushButton::clicked, this, &ControlCameraWindow::save_current_config);
    buttons_layout->addWidget(save_button);

    apply_button = new QPushButton("Apply", this);
    connect(apply_button, &QPushButton::clicked, this, &ControlCameraWindow::apply_unapply_changes);
    buttons_layout->addWidget(apply_button);

    run_button = new QPushButton("Run", this);
    connect(run_button, &QPushButton::clicked, this, &ControlCameraWindow::run_action);
    run_button->setEnabled(false);
    buttons_layout->addWidget(run_button);

    exit_button = new QPushButton("Exit", this);
    connect(exit_button, &QPushButton::clicked, this, &ControlCameraWindow::exit_action);
    buttons_layout->addWidget(exit_button);
    control_layout->addLayout(buttons_layout);

    main_layout->addLayout(control_layout);
}

void ControlCameraWindow::setup_result_page() {
    QHBoxLayout *result_layout = new QHBoxLayout(result_page);

    live_feed_label = new QLabel(result_page);
    live_feed_label->setFixedSize(image_width, image_height);
    live_feed_label->setAlignment(Qt::AlignTop);
    result_layout->addWidget(live_feed_label, 0, Qt::AlignLeft);

    QVBoxLayout *right_side_layout = new QVBoxLayout();
    position_label = new QLabel(position_text);
    right_side_layout->addWidget(position_label, 0, Qt::AlignTop);

    QPushButton *back_button = new QPushButton("Back", result_page);
    connect(back_button, &QPushButton::clicked, this, &ControlCameraWindow::back_to_main_page);
    right_side_layout->addWidget(back_button, 0, Qt::AlignBottom);

    result_layout->addLayout(right_side_layout);
}

void ControlCameraWindow::setup_camera_calibration_page_1() {
    QVBoxLayout *layout = new QVBoxLayout(camera_calibration_page_1);
    QLabel *label = new QLabel("Camera Calibration Page", this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}

void ControlCameraWindow::update_image(const QImage &qt_image) {
    label->setPixmap(QPixmap::fromImage(qt_image));
}

void ControlCameraWindow::update_y_axis_start_value(int value) {
    line_position_y_start = image_height - value;
    y_axis_start_value_label->setText(QString::number(value));
}

void ControlCameraWindow::update_y_axis_end_value(int value) {
    line_position_y_end = image_height - value;
    y_axis_end_value_label->setText(QString::number(value));
}

void ControlCameraWindow::update_point_position_value(int value) {
    point_position = value;
    point_position_value_label->setText(QString::number(value));
}

void ControlCameraWindow::update_bottom_point_position_value(int value) {
    bottom_point_position = value;
    bottom_point_position_value_label->setText(QString::number(value));
}

void ControlCameraWindow::on_exposure_change(int value) {
    camera_thread->update_exposure(value);
}

void ControlCameraWindow::update_offset_line_value(int value) {
    camera_thread->update_offset_line_value(value);
}

void ControlCameraWindow::save_current_config() {
    bool ok;
    QString name = QInputDialog::getText(this, "Save Configuration", "Enter a name for this configuration:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        saved_configs[name.toStdString()] = {
            {"line_position_y_start", image_height - y_axis_start_slider->value()},
            {"line_position_y_end", image_height - y_axis_end_slider->value()},
            {"point_position", point_position_slider->value()},
            {"bottom_point_position", bottom_point_position_slider->value()}
        };
        combo_box->addItem(name);
    }
}

void ControlCameraWindow::combo_box_changed(int index) {
    if (index == 0) {
        set_sliders_editable(true);
        save_button->setEnabled(true);
    } else {
        string config_name = combo_box->itemText(index).toStdString();
        auto config = saved_configs.find(config_name);
        if (config != saved_configs.end()) {
            load_saved_config(config->second);
        }
    }
}

class CameraThread : public QThread {
    Q_OBJECT

public:
    CameraThread(QObject *parent = nullptr) : QThread(parent), running(true), target_height(240), target_width(320), exposure_value(-5), offset_line_value(130), plot_point(false) {}

    void run() override {
        while (running) {
            cap.set(CAP_PROP_EXPOSURE, exposure_value);  // Set the exposure

            Mat frame;
            bool ret = cap.read(frame);
            if (!ret) {
                break;
            }

            // Create an instance of calibrate_feed with the frame
            calibrate_feed calibrator(frame);
            // Call the calibrate method on the instance
            frame = calibrator.calibrate();
            Mat feed = frame.clone();

            // Process the frame using Users_live_feed
            Mat user_feed;
            int y_start, y_end, p_position, bottom_p_position, px, py, bottom_px;
            tie(user_feed, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px) = Users_live_feed(frame);

            Mat roi, not_roi;
            Point l, u;
            tie(roi, not_roi, l, u) = RegionOfInterest(frame, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px, offset_line_value).ROI();

            if (plot_point) {
                if (!u.empty() && !l.empty()) {
                    vector<Point> not_mask = { Point(image_width, 0), u, l, Point(image_width, image_height) };
                    fillPoly(user_feed, not_mask, Scalar(0, 0, 0));

                    not_mask = { Point(0, line_position_y_start), Point(image_width, line_position_y_end), Point(image_width, image_height), Point(0, image_height) };
                    fillPoly(user_feed, not_mask, Scalar(0, 0, 0));
                }
            }

            // Resize the frame to the target resolution
            Mat resized_user_feed;
            resize(user_feed, resized_user_feed, Size(target_width, target_height));
            QImage qimage(resized_user_feed.data, resized_user_feed.cols, resized_user_feed.rows, resized_user_feed.step, QImage::Format_RGB888);
            emit image_main(qimage);

            LaserDetection laser_detector(roi);
            Mat laser_detected;
            int x1, y1, x2, y2;
            tie(laser_detected, x1, y1, x2, y2) = laser_detector.laser_detection();
            laser_detected.copyTo(feed(Rect(0, 0, roi.cols, roi.rows)));

            // Create a mask where the overlay image is black (black pixels are 0, 0, 0)
            Mat mask;
            inRange(feed, Scalar(0, 0, 0), Scalar(0, 0, 0), mask);
            // Invert the mask to get the non-black areas of the overlay
            Mat mask_inv;
            bitwise_not(mask, mask_inv);
            // Use the mask to extract the background only from the background image
            Mat background_part;
            bitwise_and(frame, frame, background_part, mask);
            // Use the inverse mask to extract the non-black parts of the overlay image
            Mat overlay_part;
            bitwise_and(feed, feed, overlay_part, mask_inv);
            // Combine both parts
            Mat result;
            add(background_part, overlay_part, result);
            // Extract the non-black area from the original frame
            Mat frame_region;
            bitwise_and(frame, frame, frame_region, not_roi);
            // Inverse the mask to make the region outside the polygon black in the background image
            Mat inverse_not_roi;
            bitwise_not(not_roi, inverse_not_roi);
            // Apply the inverse mask to the background to retain only the background where the polygon is not
            Mat background_region;
            bitwise_and(result, result, background_region, inverse_not_roi);
            // Combine the two images (overlay the non-black region of the original image on top of the background)
            Mat result_feed;
            add(frame_region, background_region, result_feed);
            line(result_feed, Point(px, py), Point(bottom_px, bottom_line_y), Scalar(0, 0, 150), 3);

            if (x1 == -1 || y1 == -1 || x2 == -1 || y2 == -1 || px == -1 || py == -1 || bottom_px == -1 || bottom_line_y == -1) {
                putText(result_feed, "Not enough lines detected", Point(10, 40), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255, 255), 4, LINE_AA);
                // Handle error or skip processing
                Mat resized_result_feed;
                resize(result_feed, resized_result_feed, Size(target_width, target_height));
                QImage result_qimage(resized_result_feed.data, resized_result_feed.cols, resized_result_feed.rows, resized_result_feed.step, QImage::Format_RGB888);
                emit image_result(result_qimage);
            } else {
                // Assuming you have the frame and coordinates of two lines
                vector<Vector3d> xyz_above = XYZ(x1, y1, x2, y2).depth();
                vector<Vector3d> xyz_below = XYZ(px, py, bottom_px, bottom_line_y).depth();

                // Fit planes to the 3D points
                Vector3d normal1 = fit_plane(xyz_above);
                Vector3d normal2 = fit_plane(xyz_below);

                // Calculate the angle between the planes
                double angle = angle_between_planes(normal1, normal2);
                cout << "The angle between the two planes is " << angle << " degrees." << endl;

                // Assuming xyz_above and xyz_below are lists of 3D points from the depth calculation
                // Convert them to Eigen matrices for easier indexing
                MatrixXd xyz_above_mat = Map<MatrixXd>(xyz_above[0].data(), 3, xyz_above.size());
                MatrixXd xyz_below_mat = Map<MatrixXd>(xyz_below[0].data(), 3, xyz_below.size());

                // Separate x, y, z coordinates for each set of points
                VectorXd x_above = xyz_above_mat.row(0);
                VectorXd y_above = xyz_above_mat.row(1);
                VectorXd z_above = xyz_above_mat.row(2);
                VectorXd x_below = xyz_below_mat.row(0);
                VectorXd y_below = xyz_below_mat.row(1);
                VectorXd z_below = xyz_below_mat.row(2);

                // Prepare the data for the DataFrame
                ofstream above_file("above.csv");
                above_file << "X (mm),Y (mm),Z (mm)\n";
                for (int i = 0; i < x_above.size(); ++i) {
                    above_file << x_above[i] << "," << y_above[i] << "," << z_above[i] << "\n";
                }
                above_file.close();
                cout << "3D coordinates saved to above.csv" << endl;

                ofstream below_file("below.csv");
                below_file << "X (mm),Y (mm),Z (mm)\n";
                for (int i = 0; i < x_below.size(); ++i) {
                    below_file << x_below[i] << "," << y_below[i] << "," << z_below[i] << "\n";
                }
                below_file.close();
                cout << "3D coordinates saved to below.csv" << endl;

                // Create the 3D scatter plot
                // Note: Plotly is not directly available in C++, you might need to use a different library or export data for plotting in Python
            }

            Mat resized_result_feed;
            resize(result_feed, resized_result_feed, Size(target_width, target_height));
            QImage result_qimage(resized_result_feed.data, resized_result_feed.cols, resized_result_feed.rows, resized_result_feed.step, QImage::Format_RGB888);
            emit image_result(result_qimage);
        }
    }

    void stop() {
        running = false;
        if (cap.isOpened()) {
            cap.release();
        }
    }

    void update_exposure(int value) {
        exposure_value = value;
    }

    void update_offset_line_value(int value) {
        offset_line_value = value;
    }



//------------------------------------------------------------------------------------------------------------------------------------------
