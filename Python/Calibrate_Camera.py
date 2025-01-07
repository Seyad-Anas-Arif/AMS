# plot without cropping
import numpy as np
import cv2
import pandas as pd

class calibrate_feed:
    def __init__(self, frame):
        self.frame = frame

    def calibrate(self):
        # Load camera calibration data (camera matrix and distortion coefficients)
        excel_path = 'camera_calibration_results.xlsx'
        calibration_data = pd.read_excel(excel_path)

        # Clean up and extract camera matrix and distortion coefficients
        camera_matrix_str = calibration_data['Camera Matrix'].values[0].replace('[', '').replace(']', '')
        dist_coeffs_str = calibration_data['Distortion Coefficients'].values[0].replace('[', '').replace(']', '')

        # Convert strings to NumPy arrays
        camera_matrix = np.array(camera_matrix_str.split(), dtype=np.float32).reshape(3, 3)
        dist_coeffs = np.array(dist_coeffs_str.split(), dtype=np.float32)

        h, w = self.frame.shape[:2]
        # Get the optimal camera matrix for undistortion
        new_camera_matrix, _ = cv2.getOptimalNewCameraMatrix(camera_matrix, dist_coeffs, (w, h), 1, (w, h))
        # Undistort the current frame without cropping
        undistorted_frame = cv2.undistort(self.frame, camera_matrix, dist_coeffs, None, new_camera_matrix)
        return undistorted_frame
