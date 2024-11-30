#include "data_forwarder.hpp"

DataForwarder::DataForwarder(unsigned short receivePort, unsigned short sendPort1, unsigned short sendPort2)
    : receiver_(receivePort),
      robot_port_(sendPort1),
      client_port_(sendPort2) {}

void DataForwarder::start() {
    Logger::getInstance().log("DataForwarder started.");
    stopForwarding = false;

    std::thread serverThread1(&DataForwarder::startServer, this, std::ref(robot_port_), "12346");
    std::thread serverThread2(&DataForwarder::startServer, this, std::ref(client_port_), "12347");

    {
        std::unique_lock<std::mutex> lock(serverReadyMutex);
        serverReadyCondVar.wait(lock, [this]() {
            return serversReady == 2;
        });
    }

    Logger::getInstance().log("Both servers are ready. Waiting for client connection to TCPReceiver...");

    std::thread receiverThread(&TCPReceiver::start, &receiver_);

    receiverThread.join();
    Logger::getInstance().log("TCPReceiver connected. Starting data distribution.");

    std::thread receiveThread(&DataForwarder::receiveAndDistributeData, this);

    serverThread1.join();
    serverThread2.join();
    receiveThread.join();
}

void DataForwarder::stop() {
    stopForwarding = true;
    robot_port_.stop();
    client_port_.stop();
}

void DataForwarder::startServer(TCPServer& server, const std::string& port) {
    Logger::getInstance().log("Starting server on port " + port);
    server.start();
    Logger::getInstance().log("Server on port " + port + " is ready.");

    {
        std::lock_guard<std::mutex> lock(serverReadyMutex);
        serversReady++;
    }
    serverReadyCondVar.notify_all();
}

void DataForwarder::receiveAndDistributeData() {
    while (!stopForwarding) {
        std::vector<unsigned char> data = receiver_.receive();
        if (!data.empty()) {
            // Найти разделитель (нулевой байт)
            auto separatorPos = std::find(data.begin(), data.end(), 0);
            if (separatorPos == data.end()) {
                continue;
            }

            std::string metadataBytes(data.begin(), separatorPos);

            try {
                nlohmann::json metadata = nlohmann::json::parse(metadataBytes);

                // if (!metadata.contains("frame_size") || !metadata.contains("timestamp")) {
                //     continue;
                // }

                // if (!metadata["frame_size"].is_number_unsigned() || !metadata["timestamp"].is_number()) {
                //     continue;
                // }

                robot_port_.sendData(data);
                client_port_.sendData(data);

            } catch (const nlohmann::json::exception& e) {
                continue;
            }
        }
    }
}
