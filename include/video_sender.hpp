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

    // Запуск видеопередачи
    void start();

    // Завершение работы
    void stop();

private:
    // Поток захвата кадров
    void captureFrame();

    // Поток отправки кадров
    void sendFrame();

    // Генерация метаданных для кадра
    nlohmann::json generateMetadata();

    // Поля для настройки и управления
    
    std::string address_;                 // IP-адрес получателя
    unsigned short port_;                 // Порт получателя
    unsigned short cameraIndex_;          // Индекс камеры
    std::atomic<bool> stopFlag{false};    // Флаг завершения потоков

    TCPSender sender_;

    // Параметры компрессии
    std::vector<int> compression_params_ = {cv::IMWRITE_JPEG_QUALITY, 90};

    // Очередь для кадров
    std::queue<std::shared_ptr<cv::Mat>> frameQueue_; // Очередь для хранения кадров
    size_t maxQueueSize_ = 10;            // Максимальный размер очереди

    // Синхронизация потоков
    std::mutex frameQueueMutex_;          // Мьютекс для очереди
    std::condition_variable frameCondVar_; // Условная переменная для синхронизации

    // Вспомогательные данные
    std::vector<unsigned char> buffer_;  // Буфер для кодированного изображения
};

#endif // VIDEO_SENDER_HPP
