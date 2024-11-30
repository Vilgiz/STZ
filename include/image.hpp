#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <utility>
#include <nlohmann/json.hpp>
#include <fstream>
#include "logger.hpp"

using json = nlohmann::json;

class Image {
public:
    explicit Image(const cv::Mat& frame);

    cv::Mat transformZone(const cv::Mat& frame);
    cv::Mat transformChees(const cv::Mat& frame);
    cv::Mat imageCorrection(const cv::Mat& frame);
    cv::Mat detectContours(cv::Mat& frame, std::vector<cv::Point>& coordinates, std::vector<std::string>& angles);
    cv::Mat drawContours(cv::Mat& frame, const std::vector<std::vector<cv::Point>>& contours,
                         const std::vector<cv::Point>& coordinates, const std::vector<std::string>& angles);

private:
    cv::Mat frame;

    double brightnessFactor = 2.0;
    int threshold3 = 17;
    int threshold2 = 65;
    int blur = 1;
    int dilate = 9;

    std::pair<cv::Mat, std::string> orientationDetection(cv::Mat& frame, const std::vector<cv::Point>& contour);
};

#endif // IMAGE_HPP
