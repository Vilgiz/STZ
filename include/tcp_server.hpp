#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <boost/asio.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "logger.hpp"

/**
 * Класс для работы с TCP сервером.
 * Обеспечивает прием подключений и отправку данных клиентам.
 */
class TCPServer {
public:
    /**
     * Конструктор.
     * @param port Порт для приема подключений.
     */
    explicit TCPServer(unsigned short port);

    /**
     * Запуск сервера.
     */
    void start();

    /**
     * Остановка сервера.
     */
    void stop();

    /**
     * Отправка данных всем подключенным клиентам.
     * @param data Вектор байтов для отправки.
     */
    void sendData(const std::vector<unsigned char>& data);

private:
    boost::asio::io_context ioContext;                           ///< Контекст ввода-вывода Boost.Asio
    boost::asio::ip::tcp::acceptor acceptor_;                    ///< Аcceptor для подключения клиентов
    std::vector<boost::asio::ip::tcp::socket> clients_;          ///< Список подключенных клиентов
    std::mutex queueMutex;                                       ///< Мьютекс для синхронизации доступа к списку клиентов
    bool stopServer = false;                                     ///< Флаг остановки сервера
};

#endif // TCP_SERVER_HPP
