#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

class Camera {
public:
    /**
     * Constructor to initialize the Camera object.
     * Opens the default camera (ID 0).
     */
    Camera() {
        if (!capture.open(0)) { // Open the default camera (ID 0)
            throw std::runtime_error("Unable to open the default camera.");
        }
        std::cout << "Camera initialized successfully." << std::endl;
    }

    ~Camera() {
        end();
    }

    /**
     * Get an image from the camera.
     * 
     * @return cv::Mat The captured image, or an empty matrix if an error occurs.
     */
    cv::Mat getImage() {
        cv::Mat frame;
        if (capture.isOpened()) {
            capture >> frame; // Capture the frame
            if (frame.empty()) {
                std::cerr << "Captured frame is empty!" << std::endl;
            }
        } else {
            std::cerr << "Camera is not opened." << std::endl;
        }
        return frame;
    }

    /**
     * Display the image in a window.
     * 
     * @param frame The image to display.
     */
    void show(const cv::Mat& frame) {
        if (!frame.empty()) {
            cv::imshow("Camera Feed", frame);
        }
    }

    /**
     * End the camera session and close all OpenCV windows.
     */
    void end() {
        if (capture.isOpened()) {
            capture.release(); // Release the camera
        }
        cv::destroyAllWindows(); // Close all OpenCV windows
    }

private:
    cv::VideoCapture capture;
};

