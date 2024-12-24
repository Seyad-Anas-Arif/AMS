// #include "MainGUI.h"
// #include <QApplication>

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     ControlCameraWindow window;
//     window.show();

//     return app.exec();
// }
//------------------------------------------------------
// #include <QApplication>
// #include <QMainWindow>
// #include <QStackedWidget>
// #include <QWidget>
// #include <QVBoxLayout>
// #include <QPushButton>
// #include <QLabel>
// #include <QHBoxLayout>

// class MainWindow : public QMainWindow {
//     Q_OBJECT

// public:
//     MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
//         // Create central widget
//         QWidget *centralWidget = new QWidget(this);
//         setCentralWidget(centralWidget);

//         // Create QStackedWidget
//         QStackedWidget *stackedWidget = new QStackedWidget(this);

//         // Page 1
//         QWidget *page1 = new QWidget();
//         QVBoxLayout *layout1 = new QVBoxLayout(page1);
//         QLabel *label1 = new QLabel("Welcome to Page 1");
//         QPushButton *goToPage2 = new QPushButton("Go to Page 2");
//         layout1->addWidget(label1);
//         layout1->addWidget(goToPage2);
//         stackedWidget->addWidget(page1);

//         // Page 2
//         QWidget *page2 = new QWidget();
//         QVBoxLayout *layout2 = new QVBoxLayout(page2);
//         QLabel *label2 = new QLabel("This is Page 2");
//         QPushButton *goToPage3 = new QPushButton("Go to Page 3");
//         QPushButton *goToPage1 = new QPushButton("Back to Page 1");
//         layout2->addWidget(label2);
//         layout2->addWidget(goToPage3);
//         layout2->addWidget(goToPage1);
//         stackedWidget->addWidget(page2);

//         // Page 3
//         QWidget *page3 = new QWidget();
//         QVBoxLayout *layout3 = new QVBoxLayout(page3);
//         QLabel *label3 = new QLabel("You are now on Page 3");
//         QPushButton *goToPage2From3 = new QPushButton("Back to Page 2");
//         layout3->addWidget(label3);
//         layout3->addWidget(goToPage2From3);
//         stackedWidget->addWidget(page3);

//         // Layout for the main window
//         QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
//         mainLayout->addWidget(stackedWidget);

//         // Connect buttons to switch between pages
//         connect(goToPage2, &QPushButton::clicked, [stackedWidget]() {
//             stackedWidget->setCurrentIndex(1);
//         });

//         connect(goToPage3, &QPushButton::clicked, [stackedWidget]() {
//             stackedWidget->setCurrentIndex(2);
//         });

//         connect(goToPage1, &QPushButton::clicked, [stackedWidget]() {
//             stackedWidget->setCurrentIndex(0);
//         });

//         connect(goToPage2From3, &QPushButton::clicked, [stackedWidget]() {
//             stackedWidget->setCurrentIndex(1);
//         });
//     }
// };

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     MainWindow window;
//     window.setWindowTitle("QStackedWidget Example");
//     window.resize(400, 300);
//     window.show();

//     return app.exec();
// }
//----------------------------------------------------------------------------------------------------
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
#include <QCoreApplication>
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <cmath>
#include <numeric>

// Include necessary headers for custom classes
#include "ROI.h"
#include "Blue_Laser_Detection.h"
#include "depth_detection.h"
#include "Calibrate_Camera.h"

// Initialize parameters
cv::VideoCapture cap(0);
int image_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));  // Get actual frame height
int image_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));    // Get actual frame width
int line_position_y_start = image_height / 2;
int line_position_y_end = image_height / 2;
int line_thickness = 2;
int point_position = image_width / 2;
int bottom_line_y = image_height;
int bottom_point_position = image_width / 2;

// Define default values (initialized to initial values)
int default_line_position_y_start = line_position_y_start;
int default_line_position_y_end = line_position_y_end;
int default_point_position = point_position;
int default_bottom_point_position = bottom_point_position;

// Dictionary to store saved configurations
std::map<std::string, std::vector<int>> saved_configs;

// Function to fit a plane to 3D points
std::vector<double> fit_plane(const std::vector<std::vector<double>>& points) {
    // Points: Nx3 vector of 3D points
    std::vector<double> centroid(3, 0.0);
    for (const auto& point : points) {
        for (size_t i = 0; i < 3; ++i) {
            centroid[i] += point[i];
        }
    }
    for (auto& val : centroid) {
        val /= points.size();
    }

    std::vector<std::vector<double>> points_centered(points.size(), std::vector<double>(3));
    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            points_centered[i][j] = points[i][j] - centroid[j];
        }
    }

    cv::Mat mat_points = cv::Mat(points_centered).reshape(1);
    cv::Mat w, u, vt;
    cv::SVD::compute(mat_points, w, u, vt);
    std::vector<double> normal = { vt.at<double>(2, 0), vt.at<double>(2, 1), vt.at<double>(2, 2) }; // Last row of V^T is the normal to the plane
    double norm = std::sqrt(std::inner_product(normal.begin(), normal.end(), normal.begin(), 0.0));
    for (auto& val : normal) {
        val /= norm;  // Normalize the vector
    }
    return normal;
}

// Function to calculate angle between two planes
double angle_between_planes(const std::vector<double>& normal1, const std::vector<double>& normal2) {
    double dot_product = std::inner_product(normal1.begin(), normal1.end(), normal2.begin(), 0.0);
    double angle = std::acos(std::clamp(dot_product, -1.0, 1.0));
    return angle * (180.0 / M_PI); // Convert to degrees
}

void Users_live_feed(cv::Mat& frame, int& line_position_y_start, int& line_position_y_end, int& point_position, int& bottom_point_position, 
                     int& px, int& py, int& bottom_px) {
    cv::Mat img = frame.clone();

    // Draw horizontal line
    cv::line(img, cv::Point(0, line_position_y_start), cv::Point(image_width - 1, line_position_y_end), cv::Scalar(255, 255, 255), line_thickness);

    // Calculate and draw vertical line from point to bottom
    px = static_cast<int>(point_position);
    py = static_cast<int>(line_position_y_start + (point_position / static_cast<double>(image_width)) * (line_position_y_end - line_position_y_start));
    px = std::clamp(px, 0, image_width - 1);
    py = std::clamp(py, 0, image_height - 1);

    bottom_px = static_cast<int>(bottom_point_position);
    bottom_px = std::clamp(bottom_px, 0, image_width - 1);

    cv::line(img, cv::Point(px, py), cv::Point(bottom_px, bottom_line_y), cv::Scalar(255, 255, 255), line_thickness);
}

void update_y_axis_start(int value) {
    line_position_y_start = image_height - value;
}

void update_y_axis_end(int value) {
    line_position_y_end = image_height - value;
}

void update_point_position(int value) {
    point_position = value;
}

void update_bottom_point_position(int value) {
    bottom_point_position = value;
}

class CameraThread : public QThread {
    Q_OBJECT
public:
    // Signal to send the image to the main thread
    signals:
        void image_main(const QImage& image);
        void image_result(const QImage& image);
    
    // Other members and methods for camera handling
};

class ControlCameraWindow : public QMainWindow {
    Q_OBJECT
public:
    ControlCameraWindow() {
        setWindowTitle("Camera Application with Controls");
        
        // Disable maximize button and make the window non-resizable
        setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
        setFixedSize(700, 400);  // Set fixed size to prevent resizing
        
        position_text = "";
        
        // Flag to manage plotting
        plot_point = false;  // Initialize plot_point flag to False
        
        // Create a stacked widget to manage different pages
        stacked_widget = new QStackedWidget(this);

        // Create the main page and result page
        main_page = new QWidget(this);
        result_page = new QWidget(this);
        camera_calibration_page_1 = new QWidget(this);
        camera_calibration_page_2 = new QWidget(this);
        
        setCentralWidget(stacked_widget);
        
        stacked_widget->addWidget(main_page);
        stacked_widget->addWidget(result_page);
        stacked_widget->addWidget(camera_calibration_page_1);  // Add Camera Calibration page to the stack
        stacked_widget->addWidget(camera_calibration_page_2);  // Add Camera Calibration page to the stack

        // Initialize camera
        camera_thread = new CameraThread();
        connect(camera_thread, &CameraThread::image_main, this, &ControlCameraWindow::update_image);
        camera_thread->start();
        
        // Setup the pages
        setup_main_page();
        setup_result_page();
        setup_camera_calibration_page_1(); 
    }

    void setup_main_page() {
        // Setup the main page with camera controls and sliders.
        QHBoxLayout* main_layout = new QHBoxLayout(main_page);

        // Camera feed label
        label = new QLabel(this);
        label->setAlignment(Qt::AlignCenter);
        main_layout->addWidget(label);

        // Control layout for sliders and buttons
        QVBoxLayout* control_layout = new QVBoxLayout();

        // Monospaced font for labels
        QFont font("Times", 10);

        // Fixed width for labels to ensure visibility
        int label_width = 200;  // Adjusted width for larger text labels

        // Slider dimensions
        int slider_width = 80;
        int slider_height = 10;
        
        QHBoxLayout* help_button_layout = new QHBoxLayout();
        // Help Button (?)
        QPushButton* help_button = new QPushButton("?", this);
        help_button->setFixedSize(20, 20);
        connect(help_button, &QPushButton::clicked, this, &ControlCameraWindow::show_help_dialog);
        help_button_layout->addWidget(help_button, Qt::AlignRight);
        control_layout->addLayout(help_button_layout);
        
        // Developer Options Button
        QHBoxLayout* dev_button_layout = new QHBoxLayout();
        QPushButton* dev_options_button = new QPushButton("Developer Options", this);
        dev_options_button->setFixedSize(150, 20);
        connect(dev_options_button, &QPushButton::clicked, this, &ControlCameraWindow::show_password_dialog);
        dev_button_layout->addWidget(dev_options_button, Qt::AlignRight);
        control_layout->addLayout(dev_button_layout);
        
        QHBoxLayout* y_axis_head = new QHBoxLayout();  // Changed from QVBoxLayout to QHBoxLayout for horizontal arrangement
        // Y-Axis Heading
        QLabel* y_axis_heading = new QLabel("Intersection between die and the bending sheet", this);
        y_axis_heading->setFont(font);
        y_axis_head->addWidget(y_axis_heading, Qt::AlignLeft);
        control_layout->addLayout(y_axis_head);
        
        // Y-Axis Sliders Side by Side
        QHBoxLayout* y_axis_layout = new QHBoxLayout();  // Changed from QVBoxLayout to QHBoxLayout for horizontal arrangement
        
        // Y-axis Start Slider Layout
        QHBoxLayout* y_axis_start_layout = new QHBoxLayout();  // Changed to QVBoxLayout for vertical alignment of label and slider
        
        QLabel* y_axis_label_start = new QLabel("y_start", this);
        y_axis_label_start->setFont(font);
        y_axis_label_start->setFixedWidth(label_width);
        y_axis_start_layout->addWidget(y_axis_label_start, Qt::AlignLeft);
        y_axis_start_slider = new QSlider(Qt::Vertical, this);
        y_axis_start_slider->setRange(0, image_height);
        y_axis_start_slider->setValue(image_height - line_position_y_start);
        y_axis_start_slider->setFixedSize(slider_height, slider_width);
        connect(y_axis_start_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_y_axis_start_value);
        y_axis_start_layout->addWidget(y_axis_start_slider, Qt::AlignCenter);
        y_axis_start_value_label = new QLabel(QString::number(image_height - line_position_y_start), this);
        y_axis_start_value_label->setFont(font);
        y_axis_start_value_label->setFixedWidth(label_width);
        y_axis_start_layout->addWidget(y_axis_start_value_label, Qt::AlignRight);

        // Y-axis End Slider Layout
        QHBoxLayout* y_axis_end_layout = new QHBoxLayout();  // Changed to QVBoxLayout for vertical alignment of label and slider
      
        QLabel* y_axis_label_end = new QLabel("y_end", this);
        y_axis_label_end->setFont(font);
        y_axis_label_end->setFixedWidth(label_width);
        y_axis_end_layout->addWidget(y_axis_label_end, Qt::AlignLeft);
        y_axis_end_slider = new QSlider(Qt::Vertical, this);
        y_axis_end_slider->setRange(0, image_height);
        y_axis_end_slider->setValue(image_height - line_position_y_end);
        y_axis_end_slider->setFixedSize(slider_height, slider_width);
        connect(y_axis_end_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_y_axis_end_value);
        y_axis_end_layout->addWidget(y_axis_end_slider, Qt::AlignCenter);
        y_axis_end_value_label = new QLabel(QString::number(image_height - line_position_y_end), this);
        y_axis_end_value_label->setFont(font);
        y_axis_end_value_label->setFixedWidth(label_width);
        y_axis_end_layout->addWidget(y_axis_end_value_label, Qt::AlignRight);

        // Add the Start and End layouts side by side
        y_axis_layout->addLayout(y_axis_start_layout);
        y_axis_layout->addLayout(y_axis_end_layout);

        // Add the combined Y-axis layout to the control_layout
        control_layout->addLayout(y_axis_layout);
        
        QHBoxLayout* x_axis_head = new QHBoxLayout();  // Changed from QVBoxLayout to QHBoxLayout for horizontal arrangement
        // X-Axis Heading
        QLabel* x_axis_heading = new QLabel("Laser below the Intersection", this);
        x_axis_heading->setFont(font);
        x_axis_head->addWidget(x_axis_heading, Qt::AlignLeft);
        control_layout->addLayout(x_axis_head);

        // X-Axis Sliders Side by Side
        QVBoxLayout* x_axis_layout = new QVBoxLayout();
        // Point Position Slider
        QHBoxLayout* point_position_layout = new QHBoxLayout();
        
        QLabel* point_position_label = new QLabel("x_start", this);
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

        // Bottom Point Position Slider
        QHBoxLayout* bottom_point_position_layout = new QHBoxLayout();
        
        QLabel* bottom_point_position_label = new QLabel("x_end", this);
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
        
        // Add the combined X-axis layout to the control_layout
        control_layout->addLayout(x_axis_layout);
        
        QHBoxLayout* head = new QHBoxLayout(); 
        QLabel* heading = new QLabel("Offset of the laser in the metal sheet", this);
        heading->setFont(font);
        head->addWidget(heading, Qt::AlignLeft);  // Use 'heading' instead of 'y_axis_heading'
        control_layout->addLayout(head);

        // Slider for adjusting the offset line value
        offset_slider = new QSlider(Qt::Horizontal, this);
        offset_slider->setRange(0, image_width);  // Adjust range as needed
        offset_slider->setValue(camera_thread->offset_line_value);  // Set initial value
        offset_slider->setFixedSize(slider_width, slider_height);
      
        // Label to show the current slider value
        offset_value_label = new QLabel(QString::number(camera_thread->offset_line_value), this);
        connect(offset_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_offset_line_value);  // Connect to update function
        connect(offset_slider, &QSlider::valueChanged, this, &ControlCameraWindow::update_offset_label);  // Update the label with slider value
        connect(offset_slider, &QSlider::sliderPressed, this, &ControlCameraWindow::enable_plot_point);
        connect(offset_slider, &QSlider::sliderReleased, this, &ControlCameraWindow::disable_plot_point);
  
        // Horizontal layout to hold the slider and the value label
        QHBoxLayout* slider_layout = new QHBoxLayout();
        slider_layout->addWidget(new QLabel("Offset Line", this));  // Label for the slider
        slider_layout->addWidget(offset_slider);
        slider_layout->addWidget(offset_value_label);  // Label to show the value
        // Add the slider layout to the right side layout
        control_layout->addLayout(slider_layout);
      
        exposure_spinbox = new QSpinBox(this);
        exposure_spinbox->setMinimum(-13);  // Adjust the min value based on your camera
        exposure_spinbox->setMaximum(0);  // Adjust the max value based on your camera
        exposure_spinbox->setValue(-5);  // Default exposure value
        exposure_spinbox->setFixedSize(60, 25);
        exposure_spinbox->setSingleStep(1);  // Step size when pressing up or down
        exposure_spinbox->setKeyboardTracking(false);  // Disable typing input, allow only step changes
        connect(exposure_spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ControlCameraWindow::on_exposure_change);
        QFormLayout* grid_layout = new QFormLayout();
        grid_layout->addRow("camera exposure:", exposure_spinbox);
        control_layout->addLayout(grid_layout);

        // Combo box and Save button layout
        QHBoxLayout* combo_layout = new QHBoxLayout();
        
        // Combo box for saved configurations
        combo_box = new QComboBox(this);
        combo_box->setFixedWidth(200);
        combo_box->addItem("Custom");
        connect(combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ControlCameraWindow::combo_box_changed);
        combo_layout->addWidget(combo_box, Qt::AlignLeft);

        unsave_button = new QPushButton("-", this);
        unsave_button->setFixedWidth(30);
        connect(unsave_button, &QPushButton::clicked, this, &ControlCameraWindow::unsave_configuration);
        combo_layout->addWidget(unsave_button, Qt::AlignLeft);
        
        control_layout->addLayout(combo_layout);

        // Button Layout
        QHBoxLayout* buttons_layout = new QHBoxLayout();  // Changed from QVBoxLayout to QHBoxLayout for horizontal arrangement

        // Add Default Button
        default_button = new QPushButton("Default", this);
        connect(default_button, &QPushButton::clicked, this, &ControlCameraWindow::restore_default_values);
        buttons_layout->addWidget(default_button);

        // Add Save Button
        save_button = new QPushButton("Save", this);
        connect(save_button, &QPushButton::clicked, this, &ControlCameraWindow::save_current_config);
        buttons_layout->addWidget(save_button);

        apply_button = new QPushButton("Apply", this);
        connect(apply_button, &QPushButton::clicked, this, &ControlCameraWindow::apply_unapply_changes);
        buttons_layout->addWidget(apply_button);

        // Run Button
        run_button = new QPushButton("Run", this);
        connect(run_button, &QPushButton::clicked, this, &ControlCameraWindow::run_action);
        run_button->setEnabled(false);
        buttons_layout->addWidget(run_button);

        // Exit Button
        exit_button = new QPushButton("Exit", this);
        connect(exit_button, &QPushButton::clicked, this, &ControlCameraWindow::exit_action);
        buttons_layout->addWidget(exit_button);
        control_layout->addLayout(buttons_layout);

        // Add the control_layout to the main_layout
        main_layout->addLayout(control_layout);
    }

    // Method to update the offset value label
    void update_offset_label(int value) {
        offset_value_label->setText(QString::number(value));
    }
        
    // Inside the ControlCameraWindow class
    void enable_plot_point() {
        camera_thread->plot_point = true;
    }

    void disable_plot_point() {
        camera_thread->plot_point = false;
    }
        
    void show_help_dialog() {
        // Display a help dialog when the help button is clicked.
        QString help_text = 
            "Camera Application Controls:\n\n"
            "- Use the sliders to adjust the intersection and laser position.\n"
            "- Save: Save the current settings under a custom name.\n"
            "- Apply: Apply the changes made by the sliders.\n"
            "- Run: View the processed image result.\n"
            "- Default: Restore all settings to their default values.\n"
            "- Exit: Close the application.";
        QMessageBox::information(this, "Help", help_text);
    }
        
    void show_password_dialog() {
        // Display a password dialog for developer options.
        bool ok;
        QString password = QInputDialog::getText(this, "Developer Options", "Enter Password:", QLineEdit::Password, "", &ok);
        
        if (ok && password == "teckinsoft") {
            QMessageBox::information(this, "Access Granted", "Welcome to the Developer Options.");
            // Navigate to the Camera Calibration page
            stacked_widget->setCurrentWidget(camera_calibration_page_1);
        } else {
            QMessageBox::warning(this, "Access Denied", "Authentication failed.");
        }
    }
        
    void run_action() {
        // Handle the Run button action to switch to the result page.
        // Update the position text
        position_text = QString("y_start %1\n").arg(image_height - y_axis_start_slider->value()) +
                        QString("y_end %1\n").arg(image_height - y_axis_end_slider->value()) +
                        QString("x_start %1\n").arg(point_position_slider->value()) +
                        QString("x_end %1").arg(bottom_point_position_slider->value());
        
        // Update the position label on the result page
        position_label->setText(position_text);
        // Switch to the result page
        stacked_widget->setCurrentWidget(result_page);
        // Start updating the live feed on the result page
        connect(camera_thread, &CameraThread::image_result, this, &ControlCameraWindow::result_live_feed);
    }

    void result_live_feed(const QImage& qt_image) {
        // Update the displayed live feed image in the label on the result page.
        live_feed_label->setPixmap(QPixmap::fromImage(qt_image));
    }

    void setup_camera_calibration_page_1() {
        // Setup the camera calibration page.
        QVBoxLayout* layout = new QVBoxLayout(camera_calibration_page_1);
        QLabel* label = new QLabel("Camera Calibration Page", this);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
    }

    void setup_result_page() {
        // Implementation for result page setup
    }

private:
    QWidget* main_page;
    QWidget* result_page;
    QWidget* camera_calibration_page_1;
    QWidget* camera_calibration_page_2;
    QStackedWidget* stacked_widget;
    QLabel* label;
    QSlider* y_axis_start_slider;
    QSlider* y_axis_end_slider;
    QSlider* point_position_slider;
    QSlider* bottom_point_position_slider;
    QSlider* offset_slider;
    QSpinBox* exposure_spinbox;
    QComboBox* combo_box;
    QPushButton* unsave_button;
    QPushButton* default_button;
    QPushButton* save_button;
    QPushButton* apply_button;
    QPushButton* run_button;
    QPushButton* exit_button;
    QLabel* y_axis_start_value_label;
    QLabel* y_axis_end_value_label;
    QLabel* point_position_value_label;
    QLabel* bottom_point_position_value_label;
    QLabel* offset_value_label;
    QLabel* position_label;
    QString position_text;
    bool plot_point;
    CameraThread* camera_thread;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ControlCameraWindow window;
    window.show();
    return app.exec();
}
