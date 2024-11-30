#ifndef VIDEO_SENDER_HPP
#define VIDEO_SENDER_HPP

#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <condition_variable>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <memory>
#include "logger.hpp"
#include "tcp_sender.hpp"

class VideoSender {
public:
    VideoSender(const std::string& address,
                unsigned short port,
                unsigned short cameraIndex);

    void start();
    void stop();

private:
    void captureFrame();
    void sendFrame();
    nlohmann::json generateMetadata();

    std::string address_;
    unsigned short port_;
    unsigned short cameraIndex_;
    std::atomic<bool> stopFlag{false};
    TCPSender sender_;

    std::vector<int> compression_params_ = {cv::IMWRITE_JPEG_QUALITY, 90};

    std::queue<std::shared_ptr<cv::Mat>> frameQueue_;
    size_t maxQueueSize_ = 10;

    std::mutex frameQueueMutex_;
    std::condition_variable frameCondVar_;

    std::mutex connectionMutex_;                  // Мьютекс для ожидания подключения
    std::condition_variable connectionCondVar_;   // Условная переменная для ожидания подключения
    bool isConnectionReady_ = false;              // Флаг состояния подключения
};

#endif // VIDEO_SENDER_HPP
