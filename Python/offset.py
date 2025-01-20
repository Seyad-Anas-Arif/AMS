#opposite to vision components
import cv2
import numpy as np
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel, QHBoxLayout, QSlider
from PyQt5.QtCore import Qt
import sys
 
# Initialize parameters
image_height = 500
image_width = 800
line_position_y_start = image_height // 2
line_position_y_end = image_height // 2
line_thickness = 2
point_position = image_width // 2  # Position along the first line (0 to image_width)
bottom_line_y = image_height  # Y-position for the bottom line
bottom_point_position = image_width // 2  # Position along the bottom line (0 to image_width)
 
def calculate_extended_intersections(start_x, start_y, end_x, end_y):
    intersections = []
    offset = 50  # Move the line 50 pixels to the left
   
    # Calculate slope
    if end_x != start_x:
        slope = (end_y - start_y) / (end_x - start_x)
    else:
        slope = float('inf')
 
    # Calculate intersections with the image boundaries
    if slope != float('inf'):
        # Intersection with the left boundary (x = 0)
        y_at_x0 = start_y - slope * (start_x - offset)
        if 0 <= y_at_x0 < image_height:
            intersections.append((0, int(y_at_x0)))
 
        # Intersection with the right boundary (x = image_width - 1)
        y_at_x_max = start_y + slope * (image_width - (start_x - offset) - 1)
        if 0 <= y_at_x_max < image_height:
            intersections.append((image_width - 1, int(y_at_x_max)))
 
        # Intersection with the top boundary (y = 0)
        x_at_y0 = (start_x - offset) + (0 - start_y) / slope
        if 0 <= x_at_y0 < image_width:
            intersections.append((int(x_at_y0), 0))
 
        # Intersection with the bottom boundary (y = image_height - 1)
        x_at_y_max = (start_x - offset) + (image_height - 1 - start_y) / slope
        if 0 <= x_at_y_max < image_width:
            intersections.append((int(x_at_y_max), image_height - 1))
    else:
        # Vertical line
        intersections = [(start_x - offset, 0), (start_x - offset, image_height - 1)]
 
    # Ensure distinct points
    unique_intersections = []
    for point in intersections:
        if point not in unique_intersections:
            unique_intersections.append(point)
 
    return unique_intersections
 
def find_intersection(point_1, point_2, line_y_start, line_y_end):
    epsilon = 1e-6  # Small value to check for parallel lines
   
    x1, y1 = point_1
    x2, y2 = point_2
   
    if x1 == x2:  # Vertical line case
        m_y = float('inf')
        c_y = None
    else:
        m_y = (y2 - y1) / (x2 - x1)
        c_y = y1 - m_y * x1
 
    m_w = (line_y_end - line_y_start) / (image_width - 1)
    c_w = line_y_start
 
    # Check for parallel or nearly parallel lines
    if abs(m_y - m_w) < epsilon:
        return None
 
    if m_y == float('inf'):
        x_intersect = x1
        y_intersect = m_w * x_intersect + c_w
    elif m_w == float('inf'):
        x_intersect = 0
        y_intersect = m_y * x_intersect + c_y
    else:
        x_intersect = (c_w - c_y) / (m_y - m_w)
        y_intersect = m_y * x_intersect + c_y
 
    if 0 <= x_intersect < image_width and 0 <= y_intersect < image_height:
        return int(x_intersect), int(y_intersect)
    else:
        return None
 
def ROI(frame):
    img = frame.copy()
 
    # Draw the horizontal white line
    cv2.line(img, (0, line_position_y_start), (image_width - 1, line_position_y_end), (255, 255, 255), line_thickness)
 
    # Calculate and draw the point on the first line
    px = int(point_position)
    py = int(line_position_y_start + (point_position / image_width) * (line_position_y_end - line_position_y_start))
    px = np.clip(px, 0, image_width - 1)
    py = np.clip(py, 0, image_height - 1)
 
    # Calculate and draw the point on the bottom line
    bottom_px = int(bottom_point_position)
    bottom_px = np.clip(bottom_px, 0, image_width - 1)
 
    # Draw the line connecting both points
    cv2.line(img, (px, py), (bottom_px, bottom_line_y), (0, 0, 0), 3)
    cv2.imshow("users viewpoint", img)
 
    # Calculate intersections with the image boundaries
    original_intersections = calculate_extended_intersections(px, py, bottom_px, bottom_line_y)
 
    # Filter out invalid intersection points
    original_intersections = [point for point in original_intersections if 0 <= point[0] < image_width and 0 <= point[1] < image_height]
   
    # Create a mask with the green area
    mask = np.zeros_like(img)
    mask_1_points = np.array([
        [0, line_position_y_start],
        [image_width, line_position_y_end],
        [image_width, image_height],
        [0, image_height]
    ], dtype=np.int32)
    cv2.fillPoly(mask, [mask_1_points], (0, 255, 0))
 
    # Only proceed if there are at least two intersection points
    if len(original_intersections) >= 2:
        # Draw the yellow line between intersections
        cv2.line(img, original_intersections[0], original_intersections[1], (0, 255, 255), line_thickness)
 
        upper_endpoint = min(original_intersections, key=lambda point: point[1])
        lower_endpoint = max(original_intersections, key=lambda point: point[1])
 
        cv2.line(img, upper_endpoint, (upper_endpoint[0], 0), (0, 255, 255), line_thickness)
 
        mask_2_points = np.array([
            [0, 0],
            [upper_endpoint[0], 0],
            upper_endpoint,
            lower_endpoint,
            [0, image_height]
        ], dtype=np.int32)
        cv2.fillPoly(mask, [mask_2_points], (0, 255, 0))
 
        # Calculate intersection
        intersection = find_intersection(original_intersections[0], original_intersections[1], line_position_y_start, line_position_y_end)
        if intersection is not None:
            x_intersect, y_intersect = intersection
 
            # Check if intersection is within boundaries
            if 0 <= x_intersect < image_width and 0 <= y_intersect < image_height:
                x = min(upper_endpoint[0], x_intersect)
                y = max(y_intersect, line_position_y_end)
            else:
                # Plot at alternate position if out of bounds
                x = max(0, x_intersect)
                y = max(line_position_y_start, line_position_y_end)
        else:
            # Plot at alternate position if intersection is None
            x = 0
            y = max(line_position_y_start, line_position_y_end)
    else:
        # Plot at alternate position if fewer than two intersections
        x = 0
        y = max(line_position_y_start, line_position_y_end)
       
    img = cv2.addWeighted(img, 1, mask, 0.5, 0)
   
    cv2.circle(img, (x, y), 5, (0, 0, 255), -1)
    cv2.imshow("region of no interest", img)
    if x <= 0:
        x = 1
    if x >= image_width:
        x = image_width-1
    if y <= 0:
        y = 1
    if y >= image_height:
        y = image_height-1
    background = np.zeros_like(img)
    roi = img[0:y, x:image_width]
    background[0:roi.shape[0], background.shape[1] - roi.shape[1]:] = roi
    print(roi.shape)
    cv2.imshow("observation", background)
    return roi
 
def update_y_axis_start(value):
    global line_position_y_start
    line_position_y_start = image_height - value
    y_value_label_start.setText(f"Start Y-Axis: {line_position_y_start}")
 
def update_y_axis_end(value):
    global line_position_y_end
    line_position_y_end = image_height - value
    y_value_label_end.setText(f"End Y-Axis: {line_position_y_end}")
 
def update_point_position(value):
    global point_position
    point_position = value  # Directly use slider value for position
    point_position_label.setText(f"Point Position: {value}")
 
def update_bottom_point_position(value):
    global bottom_point_position
    bottom_point_position = value  # Directly use slider value for position
    bottom_point_position_label.setText(f"Bottom Point Position: {value}")
 
class ControlWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
 
    def initUI(self):
        global y_value_label_start, y_value_label_end, point_position_label, bottom_point_position_label  # Declare global variables here
 
        self.setWindowTitle('Line Controller')
 
        main_layout = QHBoxLayout()
 
        y_axis_start_layout = QVBoxLayout()
        y_axis_label_start = QLabel('Start Y-Axis', self)
        y_axis_start_layout.addWidget(y_axis_label_start)
        self.y_axis_start_slider = QSlider(Qt.Vertical, self)
        self.y_axis_start_slider.setRange(0, image_height)
        self.y_axis_start_slider.setValue(image_height - line_position_y_start)  # Inverted initial value
        self.y_axis_start_slider.valueChanged.connect(update_y_axis_start)
        y_axis_start_layout.addWidget(self.y_axis_start_slider)
 
        y_value_label_start = QLabel(f'Start Y-Axis: {line_position_y_start}', self)
        y_axis_start_layout.addWidget(y_value_label_start)
 
        main_layout.addLayout(y_axis_start_layout)
 
        y_axis_end_layout = QVBoxLayout()
        y_axis_label_end = QLabel('End Y-Axis', self)
        y_axis_end_layout.addWidget(y_axis_label_end)
        self.y_axis_end_slider = QSlider(Qt.Vertical, self)
        self.y_axis_end_slider.setRange(0, image_height)
        self.y_axis_end_slider.setValue(image_height - line_position_y_end)  # Inverted initial value
        self.y_axis_end_slider.valueChanged.connect(update_y_axis_end)
        y_axis_end_layout.addWidget(self.y_axis_end_slider)
 
        y_value_label_end = QLabel(f'End Y-Axis: {line_position_y_end}', self)
        y_axis_end_layout.addWidget(y_value_label_end)
 
        main_layout.addLayout(y_axis_end_layout)
 
        point_position_layout = QVBoxLayout()
        point_position_label_widget = QLabel('Point Position', self)
        point_position_layout.addWidget(point_position_label_widget)
        self.point_position_slider = QSlider(Qt.Horizontal, self)
        self.point_position_slider.setRange(0, image_width)
        self.point_position_slider.setValue(point_position)
        self.point_position_slider.valueChanged.connect(update_point_position)
        point_position_layout.addWidget(self.point_position_slider)
 
        point_position_label = QLabel(f'Point Position: {point_position}', self)
        point_position_layout.addWidget(point_position_label)
 
        main_layout.addLayout(point_position_layout)
 
        bottom_point_position_layout = QVBoxLayout()
        bottom_point_position_label_widget = QLabel('Bottom Point Position', self)
        bottom_point_position_layout.addWidget(bottom_point_position_label_widget)
        self.bottom_point_position_slider = QSlider(Qt.Horizontal, self)
        self.bottom_point_position_slider.setRange(0, image_width)
        self.bottom_point_position_slider.setValue(bottom_point_position)
        self.bottom_point_position_slider.valueChanged.connect(update_bottom_point_position)
        bottom_point_position_layout.addWidget(self.bottom_point_position_slider)
 
        bottom_point_position_label = QLabel(f'Bottom Point Position: {bottom_point_position}', self)
        bottom_point_position_layout.addWidget(bottom_point_position_label)
 
        main_layout.addLayout(bottom_point_position_layout)
 
        self.setLayout(main_layout)
        self.show()
 
if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = ControlWindow()
 
    cap = cv2.VideoCapture(0)
 
    if not cap.isOpened():
        print("Error: Could not open camera.")
        sys.exit()
 
    while True:
        ret, frame = cap.read()
 
        if not ret:
            print("Error: Could not read frame.")
            break
 
        frame = cv2.resize(frame, (image_width, image_height))
        frame = ROI(frame)
        cv2.imshow('after ROI', frame)
 
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
 
    cap.release()
    cv2.destroyAllWindows()
 
    try:
        sys.exit(app.exec_())
    except SystemExit:
        pass
    cv2.destroyAllWindows()
 