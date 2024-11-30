#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void requestCoordinates() {
    const std::string server_ip = "127.0.0.1";
    const int server_port = 12346;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Создаем сокет
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return;
    }

    // Настраиваем серверный адрес
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        close(sock);
        return;
    }

    // Подключаемся к серверу
    std::cout << "Connecting to server at " << server_ip << ":" << server_port << "..." << std::endl;

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(sock);
        return;
    }

    std::cout << "Connected to server!" << std::endl;

    while (true) {
        std::cout << "Enter command (GET_COORDINATES to fetch data or QUIT to exit): ";
        std::string command;
        std::getline(std::cin, command);

        // Убедиться, что команда не пуста
        if (command.empty()) {
            std::cerr << "Command cannot be empty. Try again." << std::endl;
            continue;
        }

        // Завершение работы клиента
        if (command == "QUIT") {
            std::cout << "Exiting..." << std::endl;
            break;
        }

        if (command == "GET_COORDINATES") {
            // Отправляем запрос на сервер
            ssize_t bytes_sent = send(sock, command.c_str(), command.length(), 0);
            if (bytes_sent < 0) {
                perror("Send failed");
                break;
            }

            // Получаем ответ от сервера
            memset(buffer, 0, sizeof(buffer));
            int valread = read(sock, buffer, sizeof(buffer));
            if (valread > 0) {
                std::cout << "Received data: " << buffer << std::endl;
            } else if (valread == 0) {
                std::cout << "Server closed the connection." << std::endl;
                break;
            } else {
                perror("Read failed");
                break;
            }
        } else {
            std::cout << "Unknown command. Please enter GET_COORDINATES or QUIT." << std::endl;
        }
    }

    close(sock);
}

int main() {
    requestCoordinates();
    return 0;
}
