#include "image.hpp"

Image::Image(const cv::Mat& frame) : frame(frame) {}

cv::Mat Image::transformZone(const cv::Mat& frame) {
    std::ifstream file("transformation_data.json");
    if (!file.is_open()) {
        Logger::getInstance().log("Error opening transformation_data.json");
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

cv::Mat Image::transformChees(const cv::Mat& frame) {
    std::ifstream file("calibration_result.json");
    if (!file.is_open()) {
        Logger::getInstance().log("Error opening calibration_result.json");
        throw std::runtime_error("Error opening calibration_result.json");
    }

    json data;
    file >> data;

    auto camera_matrix_data = data["camera_matrix"].get<std::vector<std::vector<double>>>();
    cv::Mat cameraMatrix = cv::Mat(camera_matrix_data.size(), camera_matrix_data[0].size(), CV_64F);

    for (size_t i = 0; i < camera_matrix_data.size(); ++i) {
        for (size_t j = 0; j < camera_matrix_data[i].size(); ++j) {
            cameraMatrix.at<double>(i, j) = camera_matrix_data[i][j];
        }
    }

    auto dist_coefficients_data = data["dist_coefficients"].get<std::vector<double>>();
    cv::Mat distCoefficients = cv::Mat(1, dist_coefficients_data.size(), CV_64F);
    for (size_t i = 0; i < dist_coefficients_data.size(); ++i) {
        distCoefficients.at<double>(0, i) = dist_coefficients_data[i];
    }

    cv::Mat output;
    cv::undistort(frame, output, cameraMatrix, distCoefficients);
    return output;
}

cv::Mat Image::imageCorrection(const cv::Mat& frame) {
    cv::Mat corrected;
    frame.convertTo(corrected, -1, brightnessFactor, 0);

    if (blur % 2 == 0) blur += 1;
    if (threshold2 % 2 == 0) threshold2 += 1;
    if (threshold3 == 1) threshold3 += 2;

    cv::medianBlur(corrected, corrected, 3);

    return corrected;
}

cv::Mat Image::detectContours(cv::Mat& frame, std::vector<cv::Point>& coordinates, std::vector<std::string>& angles) {
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
                angles.push_back(angle);
            }
        }
    }
    return frame;
}

cv::Mat Image::drawContours(cv::Mat& frame, const std::vector<std::vector<cv::Point>>& contours,
                            const std::vector<cv::Point>& coordinates, const std::vector<std::string>& angles) {
    for (const auto& contour : contours) {
        cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), 1);
    }

    for (size_t i = 0; i < coordinates.size(); ++i) {
        const auto& coord = coordinates[i];
        const auto& angle = angles[i];
        if (angle == "above") {
            cv::circle(frame, coord, 2, cv::Scalar(0, 0, 255), -1);
        } else if (angle == "under") {
            cv::circle(frame, coord, 2, cv::Scalar(255, 0, 0), -1);
        }
    }

    return frame;
}

std::pair<cv::Mat, std::string> Image::orientationDetection(cv::Mat& frame, const std::vector<cv::Point>& contour) {
    cv::Rect boundingBox = cv::boundingRect(contour);
    cv::Mat roi = frame(boundingBox);

    int centerY = boundingBox.y + boundingBox.height / 2;

    if (centerY > frame.rows / 2) {
        return {roi, "above"};
    } else {
        return {roi, "under"};
    }
}
