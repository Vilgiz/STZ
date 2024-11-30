#include "tcp_sender.hpp"
#include "logger.hpp"
#include <thread>
#include <chrono>

TCPSender::TCPSender(const std::string& address, unsigned short port)
    : socket_(ioContext_),
      endpoint_(boost::asio::ip::make_address(address), port),
      isConnected_(false) {
    Logger::getInstance().log("TCPSender initialized for " + address + ":" + std::to_string(port));
}

TCPSender::~TCPSender() {
    try {
        if (socket_.is_open()) {
            socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            socket_.close();
            Logger::getInstance().log("TCP socket closed.");
        }
    } catch (const std::exception& e) {
        Logger::getInstance().log("Error closing socket: " + std::string(e.what()));
    }
}

void TCPSender::start() {
    try {
        socket_.connect(endpoint_);
        isConnected_ = true;
        Logger::getInstance().log("TCP connection established with " +
                                   endpoint_.address().to_string() + ":" +
                                   std::to_string(endpoint_.port()));
    } catch (const boost::system::system_error& e) {
        Logger::getInstance().log("TCP connection error: " + std::string(e.what()));
        throw;
    }
}

void TCPSender::send(const std::vector<unsigned char>& data) {
    if (!isConnected_) {
        throw std::runtime_error("Cannot send data: socket is not connected.");
    }

    try {
        boost::asio::write(socket_, boost::asio::buffer(data));
    } catch (const boost::system::system_error& e) {
        Logger::getInstance().log("Error in TCPSender::send: " + std::string(e.what()));
        isConnected_ = false;
        throw;
    }
}

bool TCPSender::isConnected() const {
    return isConnected_;
}
