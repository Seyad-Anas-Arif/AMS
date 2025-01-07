import cv2
import numpy as np
import math

class LaserDetection:
    BORDER_THRESHOLD = 1

    def __init__(self, frame):
        self.frame = frame
        if self.frame is None:
            raise ValueError("Image not found or unable to load.")
        self.optimal_line = None
        self.laser = None

    def select_optimal_line(self, lines):
        if lines is None:
            return None
        
        height, width = self.frame.shape[:2]
        angles_lengths = []
        
        for line in lines:
            for x1, y1, x2, y2 in line:
                # Ignore lines near the border
                if (x1 < self.BORDER_THRESHOLD or x2 < self.BORDER_THRESHOLD or 
                    x1 > width - self.BORDER_THRESHOLD or x2 > width - self.BORDER_THRESHOLD or
                    y1 < self.BORDER_THRESHOLD or y2 < self.BORDER_THRESHOLD or
                    y1 > height - self.BORDER_THRESHOLD or y2 > height - self.BORDER_THRESHOLD):
                    continue
                
                # Calculate angle and length of the line
                angle = math.atan2(y2 - y1, x2 - x1)
                length = math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)
                angles_lengths.append((line, angle, length))
        
        if not angles_lengths:
            return None
        
        # Find the most common angle
        angle_counts = {}
        for _, angle, _ in angles_lengths:
            if angle not in angle_counts:
                angle_counts[angle] = 0
            angle_counts[angle] += 1
        
        common_angle = max(angle_counts, key=angle_counts.get)
        
        # Find the longest line with the most common angle
        longest_line = None
        max_length = 0
        for line, angle, length in angles_lengths:
            if angle == common_angle and length > max_length:
                longest_line = line
                max_length = length
        
        return longest_line
  
    def extend_line_to_boundaries(self, line, image_shape, midpoint=0):
        if line is None:
            return None
        x1, y1, x2, y2 = line[0]
        height, width = image_shape[:2]
        
        if y1 == y2:
            new_y1, new_y2 = y1, y2
            new_x1, new_x2 = 0, width
        elif x1 == x2:
            new_x1, new_x2 = x1, x2
            new_y1, new_y2 = 0, height
        else:
            slope = (y2 - y1) / (x2 - x1)
            intercept = y1 - slope * x1
            new_x1 = 0
            new_y1 = int(slope * new_x1 + intercept)
            new_x2 = width
            new_y2 = int(slope * new_x2 + intercept)
        
        return new_x1, new_y1 + midpoint, new_x2, new_y2 + midpoint

    def plot_optimal_line(self, midpoint=0):
        lines = cv2.HoughLinesP(self.laser, 1, np.pi / 180, threshold=50, minLineLength = 0, maxLineGap=100)
        if lines is None:
            return self.frame, None, None, None, None  # Return None for coordinates if no line is detected
            
        optimal_line = self.select_optimal_line(lines)
        self.optimal_line = optimal_line
        extended_line = self.extend_line_to_boundaries(optimal_line, self.frame.shape, midpoint)
        
        # if extended_line:
            # x1, y1, x2, y2 = extended_line
        if extended_line:
            x1, y1, x2, y2 = optimal_line[0]
            cv2.line(self.frame, (x1, y1), (x2, y2), (0, 0, 150), 3)
            return self.frame, x1, y1, x2, y2  # Return frame and coordinates
        return self.frame, None, None, None, None  # Return None for coordinates if no line is extended

    def laser_detection(self):
        # Convert the frame from BGR to HSV
        hsv_frame = cv2.cvtColor(self.frame, cv2.COLOR_BGR2HSV)
        
        # Apply a bilateral filter to smooth the image while preserving edges
        hsv_blur = cv2.bilateralFilter(hsv_frame, 2, 90, 90)
        
        # Define the range for detecting 450 nm blue laser in HSV
        lower_blue = np.array([90, 90, 40])
        upper_blue = np.array([125, 255, 255])
        
        # Create a mask for the blue color
        blue_mask = cv2.inRange(hsv_blur, lower_blue, upper_blue)
        
        # Apply the mask to get the blue regions in the image
        blue_regions = cv2.bitwise_and(self.frame, self.frame, mask=blue_mask)
        
        # Split channels and use the blue channel as the grayscale frame
        b, g, r = cv2.split(blue_regions)
        gray_frame = b  # Assuming blue laser has highest intensity in blue channel
        
        # Get the dimensions of the grayscale frame
        height, width = gray_frame.shape
        
        # Create an empty mask to store the result
        row_intensity_mask = np.zeros_like(gray_frame)
        
        # Loop through each row to find the pixels with the highest intensity and mask the rest
        for i in range(height):
            max_intensity = np.max(gray_frame[i, :])  # Get max intensity in the row
            max_intensity_indices = np.where(gray_frame[i, :] == max_intensity)  # Get indices of max intensity pixels
            
            # Set only the pixels with the maximum intensity to their original value in the mask
            row_intensity_mask[i, max_intensity_indices] = gray_frame[i, max_intensity_indices]
        
        # Create an empty mask to store the CoM
        com_mask = np.zeros_like(gray_frame)
        
        # Find 'A', 'B', and CoM for each row
        for i in range(height):
            row = row_intensity_mask[i, :]
            
            # Find indices of all white pixels (non-zero)
            white_indices = np.where(row > 0)[0]
            
            if len(white_indices) > 0:
                # 'A' is the first white pixel
                a = white_indices[0]
                
                # 'B' is the last white pixel before the first black pixel after 'A'
                for idx in range(a + 1, len(row)):
                    if row[idx] == 0:
                        b = idx - 1
                        break
                else:
                    b = white_indices[-1]  # If no black pixel is found, 'B' is the last white pixel
                
                # Calculate CoM as the midpoint between 'A' and 'B'
                com = (a + b) // 2
                
                # Set the CoM position in the mask to white
                com_mask[i, com] = 255
                
        self.laser = com_mask
        detected_laser, x1, y1, x2, y2 = self.plot_optimal_line()
        return detected_laser, x1, y1, x2, y2  # Return frame and coordinates