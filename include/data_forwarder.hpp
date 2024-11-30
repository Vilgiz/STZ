#ifndef DATA_FORWARDER_HPP
#define DATA_FORWARDER_HPP

#include "tcp_receiver.hpp"
#include "tcp_server.hpp"
#include "logger.hpp"
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <nlohmann/json.hpp>
class DataForwarder {
public:
    /**
     * Конструктор для инициализации DataForwarder с портами.
     * @param receivePort Порт для приема данных.
     * @param sendPort1 Порт для пересылки данных на первый сервер.
     * @param sendPort2 Порт для пересылки данных на второй сервер.
     */
    DataForwarder(unsigned short receivePort, unsigned short sendPort1, unsigned short sendPort2);

    /**
     * Запуск процесса пересылки данных.
     */
    void start();

    /**
     * Остановка процесса пересылки данных.
     */
    void stop();

private:
    /**
     * Запуск сервера для пересылки данных.
     * @param server Экземпляр сервера TCPServer.
     * @param port Номер порта для сервера.
     */
    void startServer(TCPServer& server, const std::string& port);

    /**
     * Получение данных от клиента и распределение их между серверами.
     */
    void receiveAndDistributeData();

    // Члены класса
    TCPReceiver receiver_;               // Прием данных на заданном порту
    TCPServer robot_port_;               // Сервер для пересылки данных на первый порт
    TCPServer client_port_;              // Сервер для пересылки данных на второй порт
    bool stopForwarding = false;         // Флаг остановки пересылки данных

    // Синхронизация готовности серверов
    std::mutex serverReadyMutex;         // Мьютекс для синхронизации
    std::condition_variable serverReadyCondVar; // Условная переменная для ожидания готовности серверов
    int serversReady = 0;                // Счетчик готовности серверов
};

#endif // DATA_FORWARDER_HPP
