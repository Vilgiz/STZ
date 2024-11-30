#include "tcp_server.hpp"

TCPServer::TCPServer(unsigned short port)
    : acceptor_(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    Logger::getInstance().log("TCPServer initialized on port " + std::to_string(port));
}

void TCPServer::start() {
    std::thread([this]() {
        while (!stopServer) {
            boost::asio::ip::tcp::socket socket(ioContext);
            acceptor_.accept(socket);
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                clients_.push_back(std::move(socket));
            }
            Logger::getInstance().log("Client connected to server on port " +
                                      std::to_string(acceptor_.local_endpoint().port()));
        }
    }).detach();
}

void TCPServer::stop() {
    stopServer = true;
    ioContext.stop();
    Logger::getInstance().log("TCPServer stopped.");
}

void TCPServer::sendData(const std::vector<unsigned char>& data) {
    std::lock_guard<std::mutex> lock(queueMutex);
    for (auto it = clients_.begin(); it != clients_.end();) {
        boost::system::error_code ec;
        boost::asio::write(*it, boost::asio::buffer(data), ec);
        if (ec) {
            Logger::getInstance().log("Error sending data: " + ec.message());
            it = clients_.erase(it);
        } else {
            ++it;
        }
    }
    
}
