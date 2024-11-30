#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include "logger.hpp"

class Camera {
public:
    /**
     * Constructor to initialize the Camera object.
     * Opens the default camera (ID 0).
     */
    Camera();

    /**
     * Destructor to release resources.
     */
    ~Camera();

    /**
     * Get an image from the camera.
     * 
     * @return cv::Mat The captured image, or an empty matrix if an error occurs.
     */
    cv::Mat getImage();

    /**
     * Display the image in a window.
     * 
     * @param frame The image to display.
     */
    void show(const cv::Mat& frame);

    /**
     * End the camera session and close all OpenCV windows.
     */
    void end();

private:
    cv::VideoCapture capture_;
};

#endif // CAMERA_HPP
