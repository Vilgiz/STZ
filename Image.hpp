#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <utility>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

class Image {
public:
    explicit Image(const cv::Mat& frame) : frame(frame) {}

    inline cv::Mat transformZone(const cv::Mat& frame) {
        std::ifstream file("transformation_data.json");
        if (!file.is_open()) {
            throw std::runtime_error("Error opening transformation_data.json");
        }

        json data;
        file >> data;

        auto M_data = data["M"].get<std::vector<std::vector<double>>>();
        cv::Mat M(M_data.size(), M_data[0].size(), CV_64F);

        for (size_t i = 0; i < M_data.size(); ++i) {
            for (size_t j = 0; j < M_data[i].size(); ++j) {
                M.at<double>(i, j) = M_data[i][j];
            }
        }

        int maxWidth = data["maxWidth"];
        int maxHeight = data["maxHeight"];

        cv::Mat output;
        cv::warpPerspective(frame, output, M, cv::Size(maxWidth, maxHeight));

        return output;
    }

    inline cv::Mat transformChees(const cv::Mat& frame) {
    std::ifstream file("calibration_result.json");
    if (!file.is_open()) {
        throw std::runtime_error("Error opening calibration_result.json");
    }

    json data;
    file >> data;

    // Чтение матрицы камеры
    auto camera_matrix_data = data["camera_matrix"].get<std::vector<std::vector<double>>>();
    cv::Mat cameraMatrix = cv::Mat(camera_matrix_data.size(), camera_matrix_data[0].size(), CV_64F);

    for (size_t i = 0; i < camera_matrix_data.size(); ++i) {
        for (size_t j = 0; j < camera_matrix_data[i].size(); ++j) {
            cameraMatrix.at<double>(i, j) = camera_matrix_data[i][j];
        }
    }

    // Чтение коэффициентов дисторсии
    auto dist_coefficients_data = data["dist_coefficients"].get<std::vector<double>>();
    cv::Mat distCoefficients = cv::Mat(1, dist_coefficients_data.size(), CV_64F);
    for (size_t i = 0; i < dist_coefficients_data.size(); ++i) {
        distCoefficients.at<double>(0, i) = dist_coefficients_data[i];
    }

    cv::Mat output;
    cv::undistort(frame, output, cameraMatrix, distCoefficients);

    return output;
}


    inline cv::Mat imageCorrection(const cv::Mat& frame) {
        cv::Mat corrected;
        frame.convertTo(corrected, -1, brightnessFactor, 0);

        if (blur % 2 == 0) blur += 1;
        if (threshold2 % 2 == 0) threshold2 += 1;
        if (threshold3 == 1) threshold3 += 2;

        cv::medianBlur(corrected, corrected, 3);

        return corrected;
    }

    inline cv::Mat detectContours(cv::Mat& frame, std::vector<cv::Point>& coordinates, std::vector<std::string>& angels) {
        cv::Mat grayFrame, binaryFrame, dilatedFrame;

        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::adaptiveThreshold(grayFrame, binaryFrame, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                              cv::THRESH_BINARY_INV, threshold2, threshold3);

        cv::medianBlur(binaryFrame, binaryFrame, blur);

        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilate, dilate));
        cv::dilate(binaryFrame, dilatedFrame, kernel);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(dilatedFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours) {
            double area = cv::contourArea(contour);
            if (area > 50 && area < 400) {
                cv::Moments M = cv::moments(contour);
                int cX = static_cast<int>(M.m10 / M.m00);
                int cY = static_cast<int>(M.m01 / M.m00);

                std::string angle;
                cv::Mat roi;
                std::tie(roi, angle) = orientationDetection(frame, contour);

                if (!angle.empty()) {
                    coordinates.push_back(cv::Point(cX, cY));
                    angels.push_back(angle);
                }
            }
        }

        return frame;
    }

    inline cv::Mat drawContours(const cv::Mat& frame) {
        cv::Mat result = frame.clone();
        // Реализуйте логику рисования контуров
        return result;
    }

private:
    cv::Mat frame;

    // Переменные для обработки изображения
    double brightnessFactor = 2.0;
    int threshold3 = 17;
    int threshold2 = 65;
    int blur = 1;
    int dilate = 9;

    inline std::pair<cv::Mat, std::string> orientationDetection(cv::Mat& frame, const std::vector<cv::Point>& contour) {
        cv::Rect boundingBox = cv::boundingRect(contour);
        cv::Mat roi = frame(boundingBox);

        int centerY = boundingBox.y + boundingBox.height / 2;

        if (centerY > frame.rows / 2) {
            return {roi, "above"};
        } else {
            return {roi, "under"};
        }
    }
};

#endif // IMAGE_HPP
