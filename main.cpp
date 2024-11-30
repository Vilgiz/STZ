#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <netinet/in.h>
#include <unistd.h>
#include "Image.hpp"
#include "Camera.hpp"

// Shared data
std::queue<std::string> coordinateQueue;
std::mutex queueMutex;
std::condition_variable dataReady;

// Function to analyze images and push coordinates to the queue
void analyzeImages() {
    try {
        Camera camera;

        while (true) {
            cv::Mat frame = camera.getImage(); // Capture image from camera

            Image image(frame);

            // Perform image transformations and analysis
            frame = image.transformZone(frame);      // Transform zone
            frame = image.imageCorrection(frame);    // Correct image
            std::vector<cv::Point> coordinates;
            std::vector<std::string> angles;
            frame = image.detectContours(frame, coordinates, angles); // Detect contours

            camera.show(frame); // Display the processed frame

            // Prepare coordinates and angles as a string
            std::string coordinateData;
            for (size_t i = 0; i < coordinates.size(); ++i) {
                coordinateData += "X:" + std::to_string(coordinates[i].x) +
                                  " Y:" + std::to_string(coordinates[i].y) +
                                  " Angle:" + angles[i] + "\n";
            }

            // Push data to the shared queue
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                coordinateQueue.push(coordinateData);
            }
            dataReady.notify_one(); // Notify the sender thread

            // Exit loop if 'q' is pressed
            if (cv::waitKey(1) == 'q') break;
        }

        camera.end(); // Properly release camera resources
    } catch (const std::exception &e) {
        std::cerr << "Error in analyzeImages: " << e.what() << std::endl;
    }
}

void sendCoordinates() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    try {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("Socket failed");
            return;
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("Setsockopt failed");
            return;
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(12346);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("Bind failed");
            return;
        }

        if (listen(server_fd, 3) < 0) {
            perror("Listen failed");
            return;
        }

        std::cout << "Server listening on port 12346..." << std::endl;

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            return;
        }

        std::cout << "Client connected successfully!" << std::endl;

        while (true) {
            char buffer[1024] = {0};
            int valread = read(new_socket, buffer, sizeof(buffer)); // Read client request

            if (valread > 0) {
                std::string request(buffer);

                if (request == "GET_COORDINATES") {
                    std::cout << "Client requested coordinates." << std::endl;

                    // Работа с очередью
                    std::string coordinates;
                    {
                        std::lock_guard<std::mutex> lock(queueMutex);
                        if (!coordinateQueue.empty()) {
                            coordinates = coordinateQueue.front(); // Берем первый элемент
                            coordinateQueue.pop();                 // Удаляем его из очереди
                        } else {
                            coordinates = "NO_DATA_AVAILABLE"; // Если очередь пуста
                        }
                    }

                    // Отправка данных клиенту
                    ssize_t bytes_sent = send(new_socket, coordinates.c_str(), coordinates.length(), 0);
                    if (bytes_sent < 0) {
                        perror("Send failed");
                    } else {
                        std::cout << "Sent coordinates: " << coordinates << std::endl;
                    }
                } else {
                    std::cout << "Unknown request: " << request << std::endl;
                }
            } else if (valread == 0) {
                std::cout << "Client disconnected." << std::endl;
                break;
            } else {
                perror("Read error occurred");
                break;
            }
        }

        close(new_socket);
        close(server_fd);
    } catch (const std::exception &e) {
        std::cerr << "Error in sendCoordinates: " << e.what() << std::endl;
    }
}


int main() {
    try {
        std::thread imageThread(analyzeImages);
        std::thread senderThread(sendCoordinates);

        imageThread.join();
        senderThread.join();
    } catch (const std::exception &e) {
        std::cerr << "Error in main: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
