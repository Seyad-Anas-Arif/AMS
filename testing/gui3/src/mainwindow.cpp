#include "../inc/mainwindow.h"

using namespace std;

QMap<QString, QMap<QString, int>> saved_configs;
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          cameraThread(new CameraThread)
{
    ui->setupUi(this);

    // Connect signals from CameraThread to slots in MainWindow
    connect(cameraThread, &CameraThread::imageMain, this, &MainWindow::updateMainImage);
    connect(cameraThread, &CameraThread::imageResult, this, &MainWindow::updateMainImage);

    // window switch buttons
    connect(ui->run_btn, &QPushButton::clicked, this, &MainWindow::onRunButtonClicked);
    connect(ui->rw_back_btn, &QPushButton::clicked, this, &MainWindow::rwBackButtonClicked);

    connect(ui->Developer_btn, &QPushButton::clicked, this, &MainWindow::onDeveloperbuttonClicked);
    connect(ui->dw_back_btn, &QPushButton::clicked, this, &MainWindow::dwBackButtonClicked);

    // Actions for the main GUI window
    connect(ui->Default_btn, &QPushButton::clicked, this, &MainWindow::onDefaultButtonClicked);
    connect(ui->save_btn, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    connect(ui->apply_btn, &QPushButton::clicked, this, &MainWindow::onApplyButtontoggled);
    connect(ui->run_btn, &QPushButton::clicked, this, &MainWindow::onRunButtonClicked);
    connect(ui->exit_btn, &QPushButton::clicked, this, &MainWindow::onExitButtonClicked);
    connect(ui->delete_btn, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);

    // Connect sliders or other UI elements to update exposure and offset line values
     connect(ui->settings_comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onSettingsComboBoxChanged);
    connect(ui->exposure_spinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::updateExposure);
    connect(ui->offset_slider, static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged), this, &MainWindow::updateOffsetLineValue);

    // Start the camera thread
    cameraThread->start();
}

MainWindow::~MainWindow()
{
    cameraThread->stop();
    cameraThread->wait();
    delete cameraThread;
    delete ui;
}

void MainWindow::updateMainImage(const QImage &frame)
{
    ui->Display_lable->setPixmap(QPixmap::fromImage(frame).scaled(ui->Display_lable->size(), Qt::KeepAspectRatio));
}

void MainWindow::updateResultImage(const QImage &frame)
{
    // Update the result image label (if you have one)
    // ui->Result_lable->setPixmap(QPixmap::fromImage(frame).scaled(ui->Result_lable->size(), Qt::KeepAspectRatio));
}

//------------------------------------Button Actions------------------------------------//

void MainWindow::onDefaultButtonClicked()
{
    ui->y_start_slider->setValue(image_height - default_line_position_y_start);
    ui->y_end_slider->setValue(image_height - default_line_position_y_end);
    ui->x_start_slider->setValue(default_point_position);
    ui->X_end_slider->setValue(default_bottom_point_position);
    ui->settings_comboBox->setCurrentIndex(0);
}

void MainWindow::onSaveButtonClicked()
{
    // Show an input dialog to get the configuration name
    bool ok;
    QString name = QInputDialog::getText(this, "Save Configuration",
                                         "Enter a name for this configuration:",
                                         QLineEdit::Normal, QString(), &ok);

    if (ok && !name.isEmpty())
    {
        // Save the current configuration
        QMap<QString, int> config;
        // config["line_position_y_start"] = image_height - y_axis_start_slider->value();
        // config["line_position_y_end"] = image_height - y_axis_end_slider->value();
        // config["point_position"] = point_position_slider->value();
        // config["bottom_point_position"] = bottom_point_position_slider->value();

        saved_configs[name] = config;
        ui->settings_comboBox->addItem(name);
    }
}

void MainWindow::onApplyButtontoggled(bool checked)
{
    if (ui->apply_btn->text() == "Apply")
    {
        //after setting are applied
        ui->apply_btn->setText("Unapply");
        qDebug() << "Apply button is PRESSED    !";
        ui->Default_btn->setEnabled(false);
        ui->save_btn->setEnabled(false);
        ui->settings_comboBox->setEnabled(false);
         setSlidersEditable(false);
    }
    else
    {
        qDebug() << "Apply button is RELEASED!";
        ui->apply_btn->setText("Apply");
        ui->Default_btn->setEnabled(true);
        ui->save_btn->setEnabled(true);
        ui->settings_comboBox->setEnabled(true);
    }
}

void MainWindow::onRunButtonClicked()
{
    // Switch to the result window
    ui->stackedWidget->setCurrentWidget(ui->resultWindow);
}

void MainWindow::onDeleteButtonClicked()
{
    // Get the current index of the selected item
    int index = ui->settings_comboBox->currentIndex();

    // Check if the index is valid (non-negative)
    if (index != -1 && index != 0)
    {
        qDebug() << "Removing item at index:" << index;
        ui->settings_comboBox->removeItem(index); // Remove the selected item
        onDefaultButtonClicked();                 // Reset to default settings
    }
    else
    {
        qDebug() << "No item selected!";
    }
}

void MainWindow::onExitButtonClicked()
{
    // Show confirmation dialog
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit Application",
                                  "Are you sure you want to exit?",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    // If the user confirms exit
    if (reply == QMessageBox::Yes)
    {
        // if (camera_thread)
        // {
        //     camera_thread->quit();  // Quit the thread's event loop
        //     camera_thread->wait();  // Wait for the thread to finish
        // }
        this->close(); // Close the application window
    }
}

void MainWindow::onDeveloperbuttonClicked()
{
    // Display a password input dialog
    bool ok;
    QString password = QInputDialog::getText(this, "Developer Options",
                                             "Enter Password:",
                                             QLineEdit::Password,
                                             QString(), &ok);

    // Check if the user clicked OK and entered the correct password
    if (ok && password == "TIS")
    {
        QMessageBox::information(this, "Access Granted", "Welcome to the Developer Options.");

        // Navigate to the Camera Calibration page
        ui->stackedWidget->setCurrentWidget(ui->developerWindow);
    }
    else
    {
        QMessageBox::warning(this, "Access Denied", "Authentication failed.");
    }
    // Switch to the developer window
}

void MainWindow::rwBackButtonClicked()
{
    // Switch back to the main GUI window
    ui->stackedWidget->setCurrentWidget(ui->mainGuiWindow);
}

void MainWindow::dwBackButtonClicked()
{
    // Switch back to the main GUI window
    ui->stackedWidget->setCurrentWidget(ui->mainGuiWindow);
}

//------------------------------------Slider and ComboBox Actions------------------------------------//
void MainWindow::updateExposure(int value)
{
    cameraThread->updateExposure(value);
}

void MainWindow::updateOffsetLineValue(int value)
{
    cameraThread->updateOffsetLineValue(value);
}
void MainWindow::onXStartSliderChanged(int value)
{
    // Set the X-Start value
}

void MainWindow::onXEndSliderChanged(int value)
{
    // Set the X-End value
}

void MainWindow::onYStartSliderChanged(int value)
{
    // Set the Y-Start value
}

void MainWindow::onYEndSliderChanged(int value)
{
    // Set the Y-End value
}

void MainWindow::onOffsetSliderChanged(int value)
{
    // Set the offset value
}

void MainWindow::onSettingsComboBoxChanged(int index)
{
    qDebug() << "Selected index:" << index;
    // if (index == 0)
    //     {
    //         // Reset to "Custom"
    //         setSlidersEditable(true);
    //         save_button->setEnabled(true);
    //     }
    //     else
    //     {
    //         QString config_name = combo_box->itemText(index);
    //         if (saved_configs.contains(config_name))
    //         {
    //             QMap<QString, int> config = saved_configs[config_name];
    //             y_axis_start_slider->setValue(image_height - config["line_position_y_start"]);
    //             y_axis_end_slider->setValue(image_height - config["line_position_y_end"]);
    //             point_position_slider->setValue(config["point_position"]);
    //             bottom_point_position_slider->setValue(config["bottom_point_position"]);

    //             setSlidersEditable(false);
    //             save_button->setEnabled(false);
    //         }
    //     }
}
void MainWindow::restoreDefaultValues()
{
    // y_axis_start_slider->setValue(image_height - default_line_position_y_start);
    // y_axis_end_slider->setValue(image_height - default_line_position_y_end);
    // point_position_slider->setValue(default_point_position);
    // bottom_point_position_slider->setValue(default_bottom_point_position);
    // combo_box->setCurrentIndex(0);
}

void MainWindow::setSlidersEditable(bool editable)
{
    ui->y_start_slider->setEnabled(editable);
    ui->y_end_slider->setEnabled(editable);
    ui->x_start_slider->setEnabled(editable);
    ui->X_end_slider->setEnabled(editable);
}
