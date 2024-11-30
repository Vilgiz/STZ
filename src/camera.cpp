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
    return frame;
    }
}

void Camera::show(const cv::Mat& frame) {
    if (!frame.empty()) {
        cv::imshow("Camera Feed", frame);
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
