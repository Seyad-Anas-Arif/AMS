from ROI import RegionOfInterest
from Blue_Laser_Detection import LaserDetection
from depth_detection import XYZ
from Calibrate_Camera import calibrate_feed
import sys
import plotly.graph_objects as go
import pandas as pd
import cv2
import numpy as np
from PyQt5.QtCore import Qt, QThread, pyqtSignal
from PyQt5.QtGui import QImage, QPixmap, QFont
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QLabel, QWidget, QVBoxLayout, QSpinBox, QFormLayout,
    QHBoxLayout, QSlider, QPushButton, QLineEdit, QComboBox, QInputDialog, 
    QMessageBox, QStackedWidget
)

# Initialize parameters
cap = cv2.VideoCapture(0)
image_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))  # Get actual frame height
image_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))    # Get actual frame width
line_position_y_start = image_height // 2
line_position_y_end = image_height // 2
line_thickness = 2
point_position = image_width // 2
bottom_line_y = image_height
bottom_point_position = image_width // 2

# Define default values (initialized to initial values)
default_line_position_y_start = line_position_y_start
default_line_position_y_end = line_position_y_end
default_point_position = point_position
default_bottom_point_position = bottom_point_position

# Dictionary to store saved configurations
saved_configs = {}

# Function to fit a plane to 3D points
def fit_plane(points):
    # Points: Nx3 numpy array of 3D points
    centroid = np.mean(points, axis=0)
    points_centered = points - centroid
    _, _, vh = np.linalg.svd(points_centered)
    normal = vh[-1]  # Last row of V^T is the normal to the plane
    return normal / np.linalg.norm(normal)  # Normalize the vector

# Function to calculate angle between two planes
def angle_between_planes(normal1, normal2):
    dot_product = np.dot(normal1, normal2)
    angle = np.arccos(np.clip(dot_product, -1.0, 1.0))
    return np.degrees(angle)

# def calculate_angle(vector1, vector2):
#     # Normalize the vectors
#     unit_vector1 = vector1 / np.linalg.norm(vector1)
#     unit_vector2 = vector2 / np.linalg.norm(vector2)

#     # Calculate the dot product
#     dot_product = np.dot(unit_vector1, unit_vector2)
    
#     # Clamp the dot product to avoid numerical errors
#     dot_product = np.clip(dot_product, -1.0, 1.0)
    
#     # Calculate the angle in radians
#     angle_radians = np.arccos(dot_product)
    
#     # Convert the angle to degrees
#     angle_degrees = np.degrees(angle_radians)
    
#     return angle_degrees

def Users_live_feed(frame):
    global line_position_y_start, line_position_y_end, point_position, bottom_point_position
    img = frame.copy()

    # Draw horizontal line
    cv2.line(img, (0, line_position_y_start), (image_width - 1, line_position_y_end), (255, 255, 255), line_thickness)

    # Calculate and draw vertical line from point to bottom
    px = int(point_position)
    py = int(line_position_y_start + (point_position / image_width) * (line_position_y_end - line_position_y_start))
    px = np.clip(px, 0, image_width - 1)
    py = np.clip(py, 0, image_height - 1)

    bottom_px = int(bottom_point_position)
    bottom_px = np.clip(bottom_px, 0, image_width - 1)

    cv2.line(img, (px, py), (bottom_px, bottom_line_y), (255, 255, 255), line_thickness)
    return img, line_position_y_start, line_position_y_end, point_position, bottom_point_position, px, py, bottom_px

def update_y_axis_start(value):
    global line_position_y_start
    line_position_y_start = image_height - value

def update_y_axis_end(value):
    global line_position_y_end
    line_position_y_end = image_height - value

def update_point_position(value):
    global point_position
    point_position = value

def update_bottom_point_position(value):
    global bottom_point_position
    bottom_point_position = value
    


class ControlCameraWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Camera Application with Controls")
        
        # Disable maximize button and make the window non-resizable
        self.setWindowFlags(Qt.Window | Qt.WindowMinimizeButtonHint | Qt.WindowCloseButtonHint)
        self.setFixedSize(700, 400)  # Set fixed size to prevent resizing
        
        self.position_text = ""
        
        # Flag to manage plotting
        self.plot_point = False  # Initialize plot_point flag to False
        
        # Create a stacked widget to manage different pages
        self.stacked_widget = QStackedWidget()

        # Create the main page and result page
        self.main_page = QWidget()
        self.result_page = QWidget()
        self.camera_calibration_page_1 = QWidget()
        self.camera_calibration_page_2 = QWidget()
        
        self.setCentralWidget(self.stacked_widget)
        
        self.stacked_widget.addWidget(self.main_page)
        self.stacked_widget.addWidget(self.result_page)
        self.stacked_widget.addWidget(self.camera_calibration_page_1)  # Add Camera Calibration page to the stack
        self.stacked_widget.addWidget(self.camera_calibration_page_2)  # Add Camera Calibration page to the stack

        # Initialize camera
        self.camera_thread = CameraThread()
        self.camera_thread.image_main.connect(self.update_image)
        self.camera_thread.start()
        
        # Setup the pages
        self.setup_main_page()
        self.setup_result_page()
        self.setup_camera_calibration_page_1() 
        

    def setup_main_page(self):
        """Setup the main page with camera controls and sliders."""
        main_layout = QHBoxLayout(self.main_page)

        # Camera feed label
        self.label = QLabel()
        self.label.setAlignment(Qt.AlignCenter)
        main_layout.addWidget(self.label)

        # Control layout for sliders and buttons
        control_layout = QVBoxLayout()

        # Monospaced font for labels
        font = QFont("Times", 10)

        # Fixed width for labels to ensure visibility
        label_width = 200  # Adjusted width for larger text labels

        # Slider dimensions
        slider_width = 80
        slider_height = 10
        
        help_button_layout = QHBoxLayout()
        # Help Button (?)
        help_button = QPushButton("?", self)
        help_button.setFixedSize(20, 20)
        help_button.clicked.connect(self.show_help_dialog)
        help_button_layout.addWidget(help_button, alignment=Qt.AlignRight)
        control_layout.addLayout(help_button_layout)
        
        # Developer Options Button
        dev_button_layout = QHBoxLayout()
        dev_options_button = QPushButton("Developer Options", self)
        dev_options_button.setFixedSize(150, 20)
        dev_options_button.clicked.connect(self.show_password_dialog)
        dev_button_layout.addWidget(dev_options_button, alignment=Qt.AlignRight)
        control_layout.addLayout(dev_button_layout)
        
        y_axis_head = QHBoxLayout()  # Changed from QVBoxLayout to QHBoxLayout for horizontal arrangement
        # Y-Axis Heading
        y_axis_heading = QLabel('Intersection between die and the bending sheet', self)
        y_axis_heading.setFont(font)
        y_axis_head.addWidget(y_axis_heading, alignment=Qt.AlignLeft)
        control_layout.addLayout(y_axis_head)
        
        # Y-Axis Sliders Side by Side
        y_axis_layout = QHBoxLayout()  # Changed from QVBoxLayout to QHBoxLayout for horizontal arrangement
        
        # Y-axis Start Slider Layout
        y_axis_start_layout = QHBoxLayout()  # Changed to QVBoxLayout for vertical alignment of label and slider
        
        y_axis_label_start = QLabel('y_start', self)
        y_axis_label_start.setFont(font)
        y_axis_label_start.setFixedWidth(label_width)
        y_axis_start_layout.addWidget(y_axis_label_start, alignment=Qt.AlignLeft)
        self.y_axis_start_slider = QSlider(Qt.Vertical, self)
        self.y_axis_start_slider.setRange(0, image_height)
        self.y_axis_start_slider.setValue(image_height - line_position_y_start)
        self.y_axis_start_slider.setFixedSize(slider_height, slider_width)
        self.y_axis_start_slider.valueChanged.connect(self.update_y_axis_start_value)
        y_axis_start_layout.addWidget(self.y_axis_start_slider, alignment=Qt.AlignCenter)
        self.y_axis_start_value_label = QLabel(f"{image_height - line_position_y_start}", self)
        self.y_axis_start_value_label.setFont(font)
        self.y_axis_start_value_label.setFixedWidth(label_width)
        y_axis_start_layout.addWidget(self.y_axis_start_value_label, alignment=Qt.AlignRight)

        # Y-axis End Slider Layout
        y_axis_end_layout = QHBoxLayout()  # Changed to QVBoxLayout for vertical alignment of label and slider
      
        y_axis_label_end = QLabel('y_end', self)
        y_axis_label_end.setFont(font)
        y_axis_label_end.setFixedWidth(label_width)
        y_axis_end_layout.addWidget(y_axis_label_end, alignment=Qt.AlignLeft)
        self.y_axis_end_slider = QSlider(Qt.Vertical, self)
        self.y_axis_end_slider.setRange(0, image_height)
        self.y_axis_end_slider.setValue(image_height - line_position_y_end)
        self.y_axis_end_slider.setFixedSize(slider_height, slider_width)
        self.y_axis_end_slider.valueChanged.connect(self.update_y_axis_end_value)
        y_axis_end_layout.addWidget(self.y_axis_end_slider, alignment=Qt.AlignCenter)
        self.y_axis_end_value_label = QLabel(f"{image_height - line_position_y_end}", self)
        self.y_axis_end_value_label.setFont(font)
        self.y_axis_end_value_label.setFixedWidth(label_width)
        y_axis_end_layout.addWidget(self.y_axis_end_value_label, alignment=Qt.AlignRight)

        # Add the Start and End layouts side by side
        y_axis_layout.addLayout(y_axis_start_layout)
        y_axis_layout.addLayout(y_axis_end_layout)

        # Add the combined Y-axis layout to the control_layout
        control_layout.addLayout(y_axis_layout)
        
        x_axis_head = QHBoxLayout()  # Changed from QVBoxLayout to QHBoxLayout for horizontal arrangement
        # X-Axis Heading
        x_axis_heading = QLabel('Laser below the Intersection', self)
        x_axis_heading.setFont(font)
        x_axis_head.addWidget(x_axis_heading, alignment=Qt.AlignLeft)
        control_layout.addLayout(x_axis_head)

        # X-Axis Sliders Side by Side
        x_axis_layout = QVBoxLayout()
        # Point Position Slider
        point_position_layout = QHBoxLayout()
        
        point_position_label = QLabel('x_start', self)
        point_position_label.setFont(font)
        point_position_label.setFixedWidth(label_width)
        point_position_layout.addWidget(point_position_label)
        self.point_position_slider = QSlider(Qt.Horizontal, self)
        self.point_position_slider.setRange(0, image_width)
        self.point_position_slider.setValue(point_position)
        self.point_position_slider.setFixedSize(slider_width, slider_height)
        self.point_position_slider.valueChanged.connect(self.update_point_position_value)
        point_position_layout.addWidget(self.point_position_slider)
        self.point_position_value_label = QLabel(f"{point_position}", self)
        self.point_position_value_label.setFont(font)
        self.point_position_value_label.setFixedWidth(label_width)
        point_position_layout.addWidget(self.point_position_value_label)
        x_axis_layout.addLayout(point_position_layout)

        # Bottom Point Position Slider
        bottom_point_position_layout = QHBoxLayout()
        
        bottom_point_position_label = QLabel('x_end', self)
        bottom_point_position_label.setFont(font)
        bottom_point_position_label.setFixedWidth(label_width)
        bottom_point_position_layout.addWidget(bottom_point_position_label)
        self.bottom_point_position_slider = QSlider(Qt.Horizontal, self)
        self.bottom_point_position_slider.setRange(0, image_width)
        self.bottom_point_position_slider.setValue(bottom_point_position)
        self.bottom_point_position_slider.setFixedSize(slider_width, slider_height)
        self.bottom_point_position_slider.valueChanged.connect(self.update_bottom_point_position_value)
        bottom_point_position_layout.addWidget(self.bottom_point_position_slider)
        self.bottom_point_position_value_label = QLabel(f"{bottom_point_position}", self)
        self.bottom_point_position_value_label.setFont(font)
        self.bottom_point_position_value_label.setFixedWidth(label_width)
        bottom_point_position_layout.addWidget(self.bottom_point_position_value_label)
        x_axis_layout.addLayout(bottom_point_position_layout)
        
        # Add the combined X-axis layout to the control_layout
        control_layout.addLayout(x_axis_layout)
        
        head = QHBoxLayout() 
        heading = QLabel('Offset of the laser in the metal sheet', self)
        heading.setFont(font)
        head.addWidget(heading, alignment=Qt.AlignLeft)  # Use 'heading' instead of 'y_axis_heading'
        control_layout.addLayout(head)

        # Slider for adjusting the offset line value
        self.offset_slider = QSlider(Qt.Horizontal)
        self.offset_slider.setRange(0, image_width)  # Adjust range as needed
        self.offset_slider.setValue(self.camera_thread.offset_line_value)  # Set initial value
        self.offset_slider.setFixedSize(slider_width, slider_height)
      
        # Label to show the current slider value
        self.offset_value_label = QLabel(f"{self.camera_thread.offset_line_value}")
        self.offset_slider.valueChanged.connect(self.update_offset_line_value)  # Connect to update function
        self.offset_slider.valueChanged.connect(self.update_offset_label)  # Update the label with slider value
        self.offset_slider.sliderPressed.connect(self.enable_plot_point)
        self.offset_slider.sliderReleased.connect(self.disable_plot_point)
  
        # Horizontal layout to hold the slider and the value label
        slider_layout = QHBoxLayout()
        slider_layout.addWidget(QLabel("Offset Line"))  # Label for the slider
        slider_layout.addWidget(self.offset_slider)
        slider_layout.addWidget(self.offset_value_label)  # Label to show the value
        # Add the slider layout to the right side layout
        control_layout.addLayout(slider_layout)
      
        self.exposure_spinbox = QSpinBox(self)
        self.exposure_spinbox.setMinimum(-13)  # Adjust the min value based on your camera
        self.exposure_spinbox.setMaximum(0)  # Adjust the max value based on your camera
        self.exposure_spinbox.setValue(-5)  # Default exposure value
        self.exposure_spinbox.setFixedSize(60, 25)
        self.exposure_spinbox.setSingleStep(1)  # Step size when pressing up or down
        self.exposure_spinbox.setKeyboardTracking(False)  # Disable typing input, allow only step changes
        self.exposure_spinbox.valueChanged.connect(self.on_exposure_change)
        grid_layout = QFormLayout()
        grid_layout.addRow("camera exposure:", self.exposure_spinbox)
        control_layout.addLayout(grid_layout)

        # Combo box and Save button layout
        combo_layout = QHBoxLayout()
        
        # Combo box for saved configurations
        self.combo_box = QComboBox(self)
        self.combo_box.setFixedWidth(200)
        self.combo_box.addItem("Custom")
        self.combo_box.currentIndexChanged.connect(self.combo_box_changed)
        combo_layout.addWidget(self.combo_box, alignment=Qt.AlignLeft)

        self.unsave_button = QPushButton("-", self)
        self.unsave_button.setFixedWidth(30)
        self.unsave_button.clicked.connect(self.unsave_configuration)
        combo_layout.addWidget(self.unsave_button, alignment=Qt.AlignLeft)
        
        control_layout.addLayout(combo_layout)

        # Button Layout
        buttons_layout = QHBoxLayout()  # Changed from QVBoxLayout to QHBoxLayout for horizontal arrangement

        # Add Default Button
        self.default_button = QPushButton("Default", self)
        self.default_button.clicked.connect(self.restore_default_values)
        buttons_layout.addWidget(self.default_button)

        # Add Save Button
        self.save_button = QPushButton("Save", self)
        self.save_button.clicked.connect(self.save_current_config)
        buttons_layout.addWidget(self.save_button)

        self.apply_button = QPushButton("Apply", self)
        self.apply_button.clicked.connect(self.apply_unapply_changes)
        buttons_layout.addWidget(self.apply_button)

        # Run Button
        self.run_button = QPushButton("Run", self)
        self.run_button.clicked.connect(self.run_action)
        self.run_button.setEnabled(False)
        buttons_layout.addWidget(self.run_button)

        # Exit Button
        self.exit_button = QPushButton("Exit", self)
        self.exit_button.clicked.connect(self.exit_action)
        buttons_layout.addWidget(self.exit_button)
        control_layout.addLayout(buttons_layout)

        # Add the control_layout to the main_layout
        main_layout.addLayout(control_layout)
        
    # Method to update the offset value label
    def update_offset_label(self, value):
        self.offset_value_label.setText(str(value))
        
    # Inside the ControlCameraWindow class
    def enable_plot_point(self):
        self.camera_thread.plot_point = True

    def disable_plot_point(self):
        self.camera_thread.plot_point = False
        
    def show_help_dialog(self):
        """Display a help dialog when the help button is clicked."""
        help_text = (
            "Camera Application Controls:\n\n"
            "- Use the sliders to adjust the intersection and laser position.\n"
            "- Save: Save the current settings under a custom name.\n"
            "- Apply: Apply the changes made by the sliders.\n"
            "- Run: View the processed image result.\n"
            "- Default: Restore all settings to their default values.\n"
            "- Exit: Close the application."
        )
        QMessageBox.information(self, "Help", help_text)
        
    def show_password_dialog(self):
        """Display a password dialog for developer options."""
        password, ok = QInputDialog.getText(self, 'Developer Options', 'Enter Password:', QLineEdit.Password)
        
        if ok and password == 'teckinsoft':
            QMessageBox.information(self, "Access Granted", "Welcome to the Developer Options.")
            # Navigate to the Camera Calibration page
            self.stacked_widget.setCurrentWidget(self.camera_calibration_page_1)
        else:
            QMessageBox.warning(self, "Access Denied", "Authentication failed.")
        
    def run_action(self):
        """Handle the Run button action to switch to the result page."""
        # Update the position text
        self.position_text = (
            f"y_start {image_height - self.y_axis_start_slider.value()}\n"
            f"y_end {image_height - self.y_axis_end_slider.value()}\n"
            f"x_start {self.point_position_slider.value()}\n"
            f"x_end {self.bottom_point_position_slider.value()}"
        )
        
        # Update the position label on the result page
        self.position_label.setText(self.position_text)
        # Switch to the result page
        self.stacked_widget.setCurrentWidget(self.result_page)
        # Start updating the live feed on the result page
        self.camera_thread.image_result.connect(self.result_live_feed)

    def result_live_feed(self, qt_image):
        """Update the displayed live feed image in the label on the result page."""
        self.live_feed_label.setPixmap(QPixmap.fromImage(qt_image))
        
    def setup_camera_calibration_page_1(self):
        """Setup the camera calibration page."""
        layout = QVBoxLayout(self.camera_calibration_page_1)
        label = QLabel("Camera Calibration Page", self)
        label.setAlignment(Qt.AlignCenter)
        layout.addWidget(label)

    def setup_result_page(self):
        """Setup the result page where the processed image is displayed."""
        result_layout = QHBoxLayout(self.result_page)  # Changed to QHBoxLayout to have side-by-side layout

        # Add a QLabel for live feed on the left side
        self.live_feed_label = QLabel(self.result_page)
        self.live_feed_label.setFixedSize(image_width, image_height)
        self.live_feed_label.setAlignment(Qt.AlignTop)
        result_layout.addWidget(self.live_feed_label, alignment=Qt.AlignLeft)

        # Create a vertical layout on the right side for text and back button
        right_side_layout = QVBoxLayout()

        # Text label to display processed image data
        self.position_label = QLabel(self.position_text)  # Use self.position_text
        right_side_layout.addWidget(self.position_label, alignment=Qt.AlignTop)

        # Back button to return to main page
        back_button = QPushButton("Back", self.result_page)
        back_button.clicked.connect(self.back_to_main_page)
        right_side_layout.addWidget(back_button, alignment=Qt.AlignBottom)

        # Add the right side layout to the result layout
        result_layout.addLayout(right_side_layout)
    
    def back_to_main_page(self):

        """Handle the action to return to the main page."""
        self.stacked_widget.setCurrentWidget(self.main_page)

    def exit_action(self):
        """Handle the Exit button action with a confirmation dialog."""
        reply = QMessageBox.question(self, 'Exit Application',
                                    "Are you sure you want to exit?",
                                    QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
        if reply == QMessageBox.Yes:
            self.camera_thread.stop()  # Correct method to stop the thread
            self.camera_thread.quit()
            self.close()

    def update_image(self, qt_image):
        """Update the displayed image in the label."""
        self.label.setPixmap(QPixmap.fromImage(qt_image))

    def update_y_axis_start_value(self, value):
        update_y_axis_start(value)
        self.y_axis_start_value_label.setText(f"{value}")

    def update_y_axis_end_value(self, value):
        update_y_axis_end(value)
        self.y_axis_end_value_label.setText(f"{value}")

    def update_point_position_value(self, value):
        update_point_position(value)
        self.point_position_value_label.setText(f"{value}")

    def update_bottom_point_position_value(self, value):
        update_bottom_point_position(value)
        self.bottom_point_position_value_label.setText(f"{value}")
        
    def on_exposure_change(self, value):
        self.camera_thread.update_exposure(value)
        
    def update_offset_line_value(self, value):
        """Update the offset line value in CameraThread."""
        self.camera_thread.update_offset_line_value(value)

    def save_current_config(self):
        global saved_configs
        
        name, ok = QInputDialog.getText(self, "Save Configuration", "Enter a name for this configuration:")
        if ok and name:
            saved_configs[name] = {
                "line_position_y_start": image_height - self.y_axis_start_slider.value(),
                "line_position_y_end": image_height - self.y_axis_end_slider.value(),
                "point_position": self.point_position_slider.value(),
                "bottom_point_position": self.bottom_point_position_slider.value()
            }
            self.combo_box.addItem(name)

    def combo_box_changed(self, index):
        if index == 0:
            # Reset to "Custom"
            self.set_sliders_editable(True)
            self.save_button.setEnabled(True)
        else:
            # Handle selected configuration
            config_name = self.combo_box.itemText(index)
            config = saved_configs.get(config_name, None)
            if config:
                self.y_axis_start_slider.setValue(image_height - config["line_position_y_start"])
                self.y_axis_end_slider.setValue(image_height - config["line_position_y_end"])
                self.point_position_slider.setValue(config["point_position"])
                self.bottom_point_position_slider.setValue(config["bottom_point_position"])
                self.set_sliders_editable(False)
                self.save_button.setEnabled(False)

    def load_saved_config(self, config):
        """Load the selected saved configuration."""
        global line_position_y_start, line_position_y_end, point_position, bottom_point_position

        line_position_y_start = config['line_position_y_start']
        line_position_y_end = config['line_position_y_end']
        point_position = config['point_position']
        bottom_point_position = config['bottom_point_position']

        self.y_axis_start_slider.setValue(image_height - line_position_y_start)
        self.y_axis_end_slider.setValue(image_height - line_position_y_end)
        self.point_position_slider.setValue(point_position)
        self.bottom_point_position_slider.setValue(bottom_point_position)
        self.set_sliders_editable(False)
        self.save_button.setEnabled(False)
        
    def unsave_configuration(self):
        """Remove the currently selected configuration from saved configurations."""
        selected = self.combo_box.currentText()
        if selected in saved_configs:
            del saved_configs[selected]
            self.update_saved_configs_combobox()
    
    def update_saved_configs_combobox(self):
        """Update the combo box with saved configurations."""
        self.combo_box.clear()
        for name in saved_configs:
            self.combo_box.addItem(name)
        # Optionally, you could set a default or empty text if there are no configs
        if not saved_configs:
            if not saved_configs:
                self.combo_box.addItem("Custom")

    def restore_default_values(self):
        """Restore the default slider values."""
        global line_position_y_start, line_position_y_end, point_position, bottom_point_position

        line_position_y_start = default_line_position_y_start
        line_position_y_end = default_line_position_y_end
        point_position = default_point_position
        bottom_point_position = default_bottom_point_position

        self.y_axis_start_slider.setValue(image_height - line_position_y_start)
        self.y_axis_end_slider.setValue(image_height - line_position_y_end)
        self.point_position_slider.setValue(point_position)
        self.bottom_point_position_slider.setValue(bottom_point_position)
        self.combo_box.setCurrentIndex(0)

    def apply_unapply_changes(self):
        """Toggle the state of the sliders and Apply button."""
        if self.apply_button.text() == "Apply":
            self.apply_button.setText('Unapply')
            self.run_button.setEnabled(True)
            self.set_sliders_editable(False)
            self.set_controls_enabled(False)
        else:
            self.apply_button.setText('Apply')
            self.run_button.setEnabled(False)
            self.set_sliders_editable(True)
            self.set_controls_enabled(True)
                
    def set_sliders_editable(self, editable):
        self.y_axis_start_slider.setEnabled(editable)
        self.y_axis_end_slider.setEnabled(editable)
        self.point_position_slider.setEnabled(editable)
        self.bottom_point_position_slider.setEnabled(editable)
    
    def set_controls_enabled(self, enabled):
        # Enable or disable controls based on the apply/unapply state
        self.default_button.setEnabled(enabled)
        self.save_button.setEnabled(enabled)
        self.combo_box.setEnabled(enabled)
        self.unsave_button.setEnabled(enabled)

class CameraThread(QThread):
    image_main = pyqtSignal(QImage)
    image_result = pyqtSignal(QImage)

    def __init__(self):
        super().__init__()
    
        self.running = True
        self.target_height = 240  # Set the target width
        self.target_width = 320  # Set the target height
        self.exposure_value = -5  # Initial exposure value
        self.offset_line_value = 130 #initial offset value
        self.plot_point = False  # Flag to control point plotting
        
    def run(self):
        while self.running:
            cap.set(cv2.CAP_PROP_EXPOSURE, self.exposure_value)  # Set the exposure
            
            ret, frame = cap.read()
            if not ret:
                break
            # Create an instance of calibrate_feed with the frame
            calibrator = calibrate_feed(frame)
            # Call the calibrate method on the instance
            frame = calibrator.calibrate()
            feed = frame.copy()
            # Process the frame using Users_live_feed
            user_feed, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px = Users_live_feed(frame)
            roi,not_roi,l,u = RegionOfInterest(frame, y_start, y_end, p_position, bottom_p_position, px, py, bottom_px, self.offset_line_value).ROI()
            if self.plot_point:
                if u is not None and l is not None:
                    not_mask = np.array([
                        [image_width, 0],
                        [u[0], 0],
                        u,
                        l,
                        [image_width, image_height]
                    ], dtype=np.int32)
                    cv2.fillPoly(user_feed, [not_mask], (0, 0, 0))

                    not_mask = np.array([
                        [0, line_position_y_start],
                        [image_width, line_position_y_end],
                        [image_width, image_height],
                        [0, image_height]
                    ], dtype=np.int32)
                    cv2.fillPoly(user_feed, [not_mask], (0, 0, 0))
                    
            # Resize the frame to the target resolution
            resized_user_feed = cv2.resize(user_feed, (self.target_width, self.target_height))
            h, w, ch = resized_user_feed.shape
            bytes_per_line = ch * w
            qimage = QImage(resized_user_feed.data, w, h, bytes_per_line, QImage.Format_RGB888)
            self.image_main.emit(qimage)

            laser_detector = LaserDetection(roi)
            laser_detected, x1, y1, x2, y2 = laser_detector.laser_detection()
            feed[0:roi.shape[0],0:roi.shape[1]] = laser_detected
            
            # Create a mask where the overlay image is black (black pixels are 0, 0, 0)
            mask = cv2.inRange(feed, np.array([0, 0, 0]), np.array([0, 0, 0]))
            # Invert the mask to get the non-black areas of the overlay
            mask_inv = cv2.bitwise_not(mask)
            # Use the mask to extract the background only from the background image
            background_part = cv2.bitwise_and(frame, frame, mask=mask)
            # Use the inverse mask to extract the non-black parts of the overlay image
            overlay_part = cv2.bitwise_and(feed, feed, mask=mask_inv)
            # Combine both parts
            result = cv2.add(background_part, overlay_part)
            # Extract the non-black area from the original frame
            frame_region = cv2.bitwise_and(frame, frame, mask=not_roi)
            # Inverse the mask to make the region outside the polygon black in the background image
            inverse_not_roi = cv2.bitwise_not(not_roi)
            # Apply the inverse mask to the background to retain only the background where the polygon is not
            background_region = cv2.bitwise_and(result, result, mask=inverse_not_roi)
            # Combine the two images (overlay the non-black region of the original image on top of the background)
            result_feed = cv2.add(frame_region, background_region)
            cv2.line(result_feed, (px, py), (bottom_px, bottom_line_y), (0, 0, 150), 3)

            if None in (x1, y1, x2, y2, px, py, bottom_px, bottom_line_y):
                cv2.putText(result_feed, "Not enough lines detected", (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 1.5, (0, 255, 255), 4, cv2.LINE_AA)
                # Handle error or skip processing
                resized_result_feed = cv2.resize(result_feed, (self.target_width, self.target_height))
                h, w, ch = resized_result_feed.shape
                bytes_per_line = ch * w
                result_qimage = QImage(resized_result_feed.data, w, h, bytes_per_line, QImage.Format_RGB888)
                self.image_result.emit(result_qimage)
            else:
                # Assuming you have the frame and coordinates of two lines
                xyz_above = XYZ(x1, y1, x2, y2).depth()
                xyz_below = XYZ(px, py, bottom_px, bottom_line_y).depth()

                # Fit planes to the 3D points
                normal1 = fit_plane(np.array(xyz_above))
                normal2 = fit_plane(np.array(xyz_below))

                # Calculate the angle between the planes
                angle = angle_between_planes(normal1, normal2)
                print(f"The angle between the two planes is {angle:.2f} degrees.")


            # Assuming xyz_above and xyz_below are lists of 3D points from the depth calculation
            # Convert them to numpy arrays for easier indexing
            xyz_above = np.array(xyz_above)  # Shape should be (N, 3) where N is the number of points
            xyz_below = np.array(xyz_below)  # Shape should be (M, 3) where M is the number of points

            # Separate x, y, z coordinates for each set of points
            x_above, y_above, z_above = xyz_above[:, 0], xyz_above[:, 1], xyz_above[:, 2]
            x_below, y_below, z_below = xyz_below[:, 0], xyz_below[:, 1], xyz_below[:, 2]
            
            

            # Prepare the data for the DataFrame
            above_data = {
                'X (mm)': x_above,
                'Y (mm)': y_above,
                'Z (mm)': z_above
            }

            # Create a DataFrame
            df = pd.DataFrame(above_data)

            # Save to a CSV file
            csv_file_path = 'above.csv'  # Specify your desired file path
            df.to_csv(csv_file_path, index=False)

            print(f"3D coordinates saved to {csv_file_path}")
            
            # Prepare the data for the DataFrame
            below_data = {
                'X (mm)': x_below,
                'Y (mm)': y_below,
                'Z (mm)': z_below
            }

            # Create a DataFrame
            df = pd.DataFrame(below_data)

            # Save to a CSV file
            csv_file_path = 'below.csv'  # Specify your desired file path
            df.to_csv(csv_file_path, index=False)

            print(f"3D coordinates saved to {csv_file_path}")
            

            # Create the 3D scatter plot
            fig = go.Figure()

            # Add points for xyz_above (metal sheet 1)
            fig.add_trace(go.Scatter3d(
                x=x_above, y=y_above, z=z_above,
                mode='markers',
                marker=dict(size=4, color='blue'),  # Blue color for metal sheet 1
                name='Metal Sheet above'
            ))

            # Add points for xyz_below (metal sheet 2)
            fig.add_trace(go.Scatter3d(
                x=x_below, y=y_below, z=z_below,
                mode='markers',
                marker=dict(size=4, color='red'),  # Red color for metal sheet 2
                name='Metal Sheet below'
            ))

            # Set plot layout
            fig.update_layout(
                scene=dict(
                    xaxis_title='X (mm)',
                    yaxis_title='Y (mm)',
                    zaxis_title='Z (mm)'
                ),
                title="3D Plot of Metal Sheets' Laser Points (Extended along Y-axis)",
            )

            # Show the plot
            fig.show()

        
            
            resized_result_feed = cv2.resize(result_feed, (self.target_width, self.target_height))
            h, w, ch = resized_result_feed.shape
            bytes_per_line = ch * w
            result_qimage = QImage(resized_result_feed.data, w, h, bytes_per_line, QImage.Format_RGB888)
            self.image_result.emit(result_qimage)

    def stop(self):
        self.running = False
        if cap:
            cap.release()
            
    def update_exposure(self, value):
        self.exposure_value = value
        
    def update_offset_line_value(self, value):
        self.offset_line_value = value
        # self.plot_point = True  # Set flag to plot the point

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ControlCameraWindow()
    window.show()
    
    sys.exit(app.exec_())