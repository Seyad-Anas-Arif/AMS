import numpy as np
import cv2
import math
import pandas as pd

# Load camera calibration data
excel_path = 'camera_calibration_results.xlsx'  # Update with the correct path
calibration_data = pd.read_excel(excel_path)

# Extract and format camera matrix and distortion coefficients
camera_matrix_str = calibration_data['Camera Matrix'].values[0].replace('[', '').replace(']', '')
dist_coeffs_str = calibration_data['Distortion Coefficients'].values[0].replace('[', '').replace(']', '')

camera_matrix = np.array(camera_matrix_str.split(), dtype=np.float32).reshape(3, 3)
dist_coeffs = np.array(dist_coeffs_str.split(), dtype=np.float32)

# Extract focal length and principal points
fx = camera_matrix[0][0]  # Focal length along the x-axis in pixels
fy = camera_matrix[1][1]  # Focal length along the y-axis in pixels
cx = camera_matrix[0][2]
cy = camera_matrix[1][2]

# Sensor specifications
sensor_width_pixels = 1920
sensor_height_pixels = 1200
pixel_size_mm = 0.003  # Pixel size in mm

# Calculate sensor dimensions
sensor_width_mm = sensor_width_pixels * pixel_size_mm
sensor_height_mm = sensor_height_pixels * pixel_size_mm
focal_length_mm_x = fx * pixel_size_mm
focal_length_mm_y = fy * pixel_size_mm
principal_point_mm_x = cx * pixel_size_mm
principal_point_mm_y = cy * pixel_size_mm

# Define triangulation parameters
baseline = 88.60  # Baseline (mm)
alpha = np.radians(30)  # Camera tilt angle (degrees to radians)

# Bresenham's algorithm to generate pixel coordinates along a line from (x1, y1) to (x2, y2)
def bresenham_line(x1, y1, x2, y2):
    points = []
    dx = abs(x2 - x1)
    dy = abs(y2 - y1)
    sx = 1 if x1 < x2 else -1
    sy = 1 if y1 < y2 else -1
    err = dx - dy

    while True:
        points.append((x1, y1))
        if x1 == x2 and y1 == y2:
            break
        e2 = 2 * err
        if e2 > -dy:
            err -= dy
            x1 += sx
        if e2 < dx:
            err += dx
            y1 += sy

    return points

class XYZ:
    def __init__(self, x1, y1, x2, y2):
        self.x1 = x1
        self.y1 = y1
        self.x2 = x2
        self.y2 = y2

    def depth(self):
        laser_pixel_coordinates = bresenham_line(self.x1, self.y1, self.x2, self.y2)
        laser_points_3D = []

        # Loop through each pixel coordinate along the laser line
        for x_p, y_p in laser_pixel_coordinates:
       
            # Convert pixel coordinates (x', y') to real-world coordinates using provided equations
            x = x_p * pixel_size_mm
            y = y_p * pixel_size_mm
            p = x - principal_point_mm_x  # Calculate distance in mm
            beta = math.atan(p / focal_length_mm_x)  # beta
            Z = math.cos(alpha) * (baseline * math.tan(alpha) + (baseline * (1 / math.tan(alpha)) / math.tan(alpha - beta)))
            Y = Z * (y - principal_point_mm_y) / focal_length_mm_y
            X = Z * (x - principal_point_mm_x) / focal_length_mm_x
        
            point_3D = np.array([X, Y, Z])
            laser_points_3D.append(point_3D)

        return laser_points_3D