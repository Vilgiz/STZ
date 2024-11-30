#include <yaml-cpp/yaml.h>
#include "data_forwarder.hpp"
#include "logger.hpp"

int main() {
  try {
        // Путь к конфигурационному файлу
        std::string config_path(std::string(CONFIG_DIR) + "/videoConfigure.yaml");
        YAML::Node config = YAML::LoadFile(config_path);

        // Чтение параметров из конфигурации
        unsigned short port_server = config["cv_port"].as<unsigned short>();
        unsigned short port_robot = config["robot_port"].as<unsigned short>();
        unsigned short client_port = config["client_port"].as<unsigned short>();
        DataForwarder forwarder(port_server, // Порт для приема данных
                                port_robot, // Первый порт для предоставления данных
                                client_port // Второй порт для предоставления данных
        );
        forwarder.start();

    } catch (const std::exception &e) {
        Logger::getInstance().log("Error: " + std::string(e.what()));
        return 1;
    }
    return 0;
}