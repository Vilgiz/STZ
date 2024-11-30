#include <iostream>
#include <yaml-cpp/yaml.h>
#include "video_sender.hpp"

int main() {
    try {
        // Путь к конфигурационному файлу
        std::string config_path(std::string(CONFIG_DIR) + "/videoConfigure.yaml");
        YAML::Node config = YAML::LoadFile(config_path);

        // Чтение параметров из конфигурации
        unsigned short port = config["cv_port"].as<unsigned short>();
        std::string ip_address = config["ip_address"].as<std::string>();
        unsigned short cameraIndex = config["cameraIndex"].as<unsigned short>();

        VideoSender sender(ip_address, port, cameraIndex);
        sender.start();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
