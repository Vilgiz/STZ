#include "camera.hpp"
#include <iostream>
#include <stdexcept>

Camera::Camera() {
    if (!capture_.open(0)) { // Open the default camera (ID 0)
        Logger::getInstance().log("Unable to open the default camera.");
        throw std::runtime_error("Unable to open the default camera.");
    }
    Logger::getInstance().log("Camera initialized successfully.");
}

Camera::~Camera() {
    end();
}

cv::Mat Camera::getImage() {
    cv::Mat frame;
    if (capture_.isOpened()) {
        capture_ >> frame;
        if (frame.empty()) {
            Logger::getInstance().log("Captured frame is empty.");
        } else {
            Logger::getInstance().log("Captured frame successfully.");
        }
    } else {
        Logger::getInstance().log("Camera is not opened.");
        std::cerr << "Camera is not opened." << std::endl;
    }
    return frame;
}

void Camera::show(const cv::Mat& frame) {
    if (!frame.empty()) {
        cv::imshow("Camera Feed", frame);
        Logger::getInstance().log("Displayed frame in window.");
    } else {
        Logger::getInstance().log("Attempted to display an empty frame.");
    }
}

void Camera::end() {
    if (capture_.isOpened()) {
        capture_.release(); // Release the camera
        Logger::getInstance().log("Camera released successfully.");
    }
    cv::destroyAllWindows(); // Close all OpenCV windows
    Logger::getInstance().log("Closed all OpenCV windows.");
}
