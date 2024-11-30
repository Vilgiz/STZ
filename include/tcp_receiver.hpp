#ifndef TCP_RECEIVER_HPP
#define TCP_RECEIVER_HPP


#include <boost/asio.hpp>
#include <condition_variable>
using boost::asio::ip::tcp;

class TCPReceiver {
public:
    explicit TCPReceiver(unsigned short port);
    void start();
    std::vector<unsigned char> receive();

private:
    boost::asio::io_context ioContext_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    std::condition_variable connectionCondVar_;
    std::mutex connectionMutex_;
    bool isConnected_;
};

#endif // TCP_RECEIVER_HPP
