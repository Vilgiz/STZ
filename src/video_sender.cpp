#include "video_sender.hpp"
#include "camera.hpp"
#include "image.hpp"

using json = nlohmann::json;

VideoSender::VideoSender(const std::string& address,
                         unsigned short port,
                         unsigned short cameraIndex)
    : address_(address), port_(port), cameraIndex_(cameraIndex), sender_(address, port) {}

void VideoSender::start() {
    Logger::getInstance().log("Starting VideoSender");
    stopFlag = false;

    std::thread connectionThread([this]() {
        sender_.start();
        {
            std::lock_guard<std::mutex> lock(connectionMutex_);
            isConnectionReady_ = true;
        }
        connectionCondVar_.notify_all();
    });

    std::thread captureThread(&VideoSender::captureFrame, this);
    std::thread sendThread(&VideoSender::sendFrame, this);

    connectionThread.join();
    captureThread.join();
    sendThread.join();
}

void VideoSender::stop() {
    stopFlag = true;
    frameCondVar_.notify_all();
    connectionCondVar_.notify_all();
}

void VideoSender::captureFrame() {
    {
        std::unique_lock<std::mutex> lock(connectionMutex_);
        connectionCondVar_.wait(lock, [this]() { return isConnectionReady_ || stopFlag; });
    }

    Camera camera;

    while (!stopFlag) {
        cv::Mat frame = camera.getImage();

        Image image(frame);
        frame = image.imageCorrection(frame);
        std::vector<cv::Point> coordinates;
        std::vector<std::string> angles;
        std::vector<std::vector<cv::Point>> contours;
        frame = image.detectContours(frame, coordinates, angles);

        frame = image.drawContours(frame, contours, coordinates, angles);

        auto framePtr = std::make_shared<cv::Mat>(std::move(frame));
        {
            std::lock_guard<std::mutex> lock(frameQueueMutex_);
            if (frameQueue_.size() < maxQueueSize_) {
                frameQueue_.push(framePtr);
                frameCondVar_.notify_one();
            }
        }
    }
}

void VideoSender::sendFrame() {
    {
        std::unique_lock<std::mutex> lock(connectionMutex_);
        connectionCondVar_.wait(lock, [this]() { return isConnectionReady_ || stopFlag; });
    }

    while (!stopFlag) {
        std::shared_ptr<cv::Mat> frameToSend;

        {
            std::unique_lock<std::mutex> lock(frameQueueMutex_);
            frameCondVar_.wait(lock, [this]() { return !frameQueue_.empty() || stopFlag; });

            if (stopFlag && frameQueue_.empty()) break;

            frameToSend = frameQueue_.front();
            frameQueue_.pop();
        }

        if (frameToSend) {
            std::vector<unsigned char> encodedBuffer;
            if (!cv::imencode(".jpg", *frameToSend, encodedBuffer, compression_params_)) {
                Logger::getInstance().log("Error: Failed to compress the image!");
                continue;
            }

            nlohmann::json metadata = generateMetadata();
            std::string metadataStr = metadata.dump();

            std::vector<unsigned char> dataToSend(metadataStr.begin(), metadataStr.end());
            dataToSend.push_back(0);
            dataToSend.insert(dataToSend.end(), encodedBuffer.begin(), encodedBuffer.end());

            sender_.send(dataToSend);
        }
    }
}

nlohmann::json VideoSender::generateMetadata() {
    return {
        {"timestamp", static_cast<long>(time(nullptr))},
        {"frame_size", 10}
    };
}
