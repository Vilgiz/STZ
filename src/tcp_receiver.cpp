#include "tcp_receiver.hpp"
#include "logger.hpp"

TCPReceiver::TCPReceiver(unsigned short port)
    : acceptor_(ioContext_, tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      socket_(ioContext_),
      isConnected_(false) {
    Logger::getInstance().log("TCPReceiver initialized on port " + std::to_string(port));
}


void TCPReceiver::start() {
    Logger::getInstance().log("TCPReceiver started.");

    try {
        Logger::getInstance().log("Waiting for incoming connection...");
        acceptor_.accept(socket_);
        {
            std::lock_guard<std::mutex> lock(connectionMutex_);
            isConnected_ = true;
        }
        connectionCondVar_.notify_all();
        Logger::getInstance().log("TCP connection accepted.");
    } catch (const std::exception& e) {
        Logger::getInstance().log(std::string("TCPReceiver start error: ") + e.what());
    }
}


std::vector<unsigned char> TCPReceiver::receive() {
    {
        std::unique_lock<std::mutex> lock(connectionMutex_);
        if (!isConnected_) {
            Logger::getInstance().log("TCPReceiver is waiting for a connection...");
            connectionCondVar_.wait(lock, [this]() { return isConnected_; });
        }
    }

    std::vector<unsigned char> buffer(65536);
    try {
        size_t length = socket_.read_some(boost::asio::buffer(buffer));
        buffer.resize(length);

        auto remoteEndpoint = socket_.remote_endpoint();

    } catch (const std::exception& e) {
      Logger::getInstance().log("Critical TCP receive error: " + std::string(e.what()));
      throw;
    }
    return buffer;
}
