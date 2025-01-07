import numpy as np
import cv2

class RegionOfInterest:
    def __init__(self, frame, line_position_y_start, line_position_y_end, point_position, bottom_point_position, px_run, py_run, bottom_px_run, offset):
        self.frame = frame
        self.offset = offset
        self.line_position_y_start = line_position_y_start
        self.line_position_y_end = line_position_y_end
        self.point_position = point_position
        self.bottom_point_position = bottom_point_position
        self.px_run = px_run
        self.py_run = py_run
        self.bottom_px_run = bottom_px_run
        self.image_height, self.image_width = frame.shape[:2]
        self.bottom_py_run = self.image_height

    def calculate_extended_intersections(self, start_x, start_y, end_x, end_y):
        intersections = []
        # Calculate slope
        if end_x != start_x:
            slope = (end_y - start_y) / (end_x - start_x)
        else:
            slope = float('inf')

        # Calculate intersections with the image boundaries
        if slope != float('inf'):
            # Intersection with the left boundary (x = 0)
            y_at_x0 = start_y - slope * (start_x + self.offset)
            if 0 <= y_at_x0 < self.image_height:
                intersections.append((0, int(y_at_x0)))

            # Intersection with the right boundary (x = self.image_width - 1)
            y_at_x_max = start_y + slope * (self.image_width - (start_x + self.offset) - 1)
            if 0 <= y_at_x_max < self.image_height:
                intersections.append((self.image_width - 1, int(y_at_x_max)))

            # Intersection with the top boundary (y = 0)
            x_at_y0 = (start_x + self.offset) + (0 - start_y) / slope
            if 0 <= x_at_y0 < self.image_width:
                intersections.append((int(x_at_y0), 0))

            # Intersection with the bottom boundary (y = self.image_height - 1)
            x_at_y_max = (start_x + self.offset) + (self.image_height - 1 - start_y) / slope
            if 0 <= x_at_y_max < self.image_width:
                intersections.append((int(x_at_y_max), self.image_height - 1))
        else:
            # Vertical line
            intersections = [(start_x + self.offset, 0), (start_x + self.offset, self.image_height - 1)]

        # Ensure distinct points
        unique_intersections = []
        for point in intersections:
            if point not in unique_intersections:
                unique_intersections.append(point)

        return unique_intersections

    def find_intersection(self, point_1, point_2):
        epsilon = 1e-6  # Small value to check for parallel lines
        
        x1, y1 = point_1
        x2, y2 = point_2
        
        if x1 == x2:  # Vertical line case
            m_y = float('inf')
            c_y = None
        else:
            m_y = (y2 - y1) / (x2 - x1)
            c_y = y1 - m_y * x1

        m_w = (self.line_position_y_end - self.line_position_y_start) / (self.image_width - 1)
        c_w = self.line_position_y_start

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

        if 0 <= x_intersect < self.image_width and 0 <= y_intersect < self.image_height:
            return int(x_intersect), int(y_intersect)
        else:
            return None

    def ROI(self):
        img = self.frame.copy()
        # Calculate intersections with the image boundaries
        original_intersections = self.calculate_extended_intersections(self.px_run, self.py_run, 
                                                                    self.bottom_px_run, self.bottom_py_run)

        # Filter out invalid intersection points
        original_intersections = [point for point in original_intersections if 0 <= point[0] < self.image_width and 0 <= point[1] < self.image_height]
        
        # Create a mask with the green area
        mask_1_points = np.array([
            [0, self.line_position_y_start],
            [self.image_width, self.line_position_y_end],
            [self.image_width, self.image_height],
            [0, self.image_height]
        ], dtype=np.int32)
        cv2.fillPoly(img, [mask_1_points], (0, 0, 0))
        mask = np.zeros((self.image_height, self.image_width), dtype=np.uint8)
        # Fill the mask with white in the region defined by mask_1_points
        cv2.fillPoly(mask, [mask_1_points], 255)
        
        # Initialize default values for endpoints
        upper_endpoint = (self.image_width, 0)
        lower_endpoint = (self.image_width, self.image_height)

        # Only proceed if there are at least two intersection points
        if len(original_intersections) >= 2:
            upper_endpoint = min(original_intersections, key=lambda point: point[1])
            lower_endpoint = max(original_intersections, key=lambda point: point[1])

            mask_2_points = np.array([
                [self.image_width, 0],
                [upper_endpoint[0], 0],
                upper_endpoint,
                lower_endpoint,
                [self.image_width, self.image_height]
            ], dtype=np.int32)
            cv2.fillPoly(img, [mask_2_points], (0, 0, 0))

            # Calculate intersection
            intersection = self.find_intersection(original_intersections[0], original_intersections[1])
            if intersection is not None:
                x_intersect, y_intersect = intersection

                # Check if intersection is within boundaries
                if 0 <= x_intersect < self.image_width and 0 <= y_intersect < self.image_height:
                    x = max(upper_endpoint[0], x_intersect)
                    y = max(y_intersect, self.line_position_y_start)
                else:
                    # Plot at alternate position if out of bounds
                    x = max(self.image_width, x_intersect)
                    y = max(self.line_position_y_start, self.line_position_y_end)
            else:
                # Plot at alternate position if intersection is None
                x = self.image_width
                y = max(self.line_position_y_start, self.line_position_y_end)
        else:
            # Plot at alternate position if fewer than two intersections
            x = self.image_width
            y = max(self.line_position_y_start, self.line_position_y_end)

        if x <= 0:
            x = 1
        if x >= self.image_width:
            x = self.image_width - 1
        if y <= 0:
            y = 1
        if y >= self.image_height:
            y = self.image_height - 1
        roi = img[0:y, 0:x]
        return roi, mask, lower_endpoint, upper_endpoint
