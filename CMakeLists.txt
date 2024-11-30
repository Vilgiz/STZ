cmake_minimum_required(VERSION 3.27)
project(VideoStreamer)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к библиотекам OpenCV и Boost
if (WIN32)
    # Пути для Windows
    set(OpenCV_DIR "C:/msys64/mingw64/lib/cmake/opencv4")
    set(Boost_DIR "C:/msys64/mingw64/lib/cmake/Boost-1.86.0")
elseif (UNIX)
    # Пути для Linux
    # set(OpenCV_DIR "/usr/local/lib/cmake/opencv4") # Пример для Linux
endif()

# Находим пакеты OpenCV и Boost
find_package(OpenCV REQUIRED COMPONENTS core imgcodecs highgui videoio calib3d) # Добавляем calib3d для cv::undistort
find_package(Boost REQUIRED COMPONENTS system thread)

# Проверяем, что OpenCV и Boost найдены
if (NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found!")
endif()

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

# Указываем путь к конфигурационной директории
set(CONFIG_DIR "${CMAKE_SOURCE_DIR}/config")

# Указываем директорию для заголовочных файлов
include_directories(${CMAKE_SOURCE_DIR}/include)

# Устанавливаем директорию для выходных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Общие флаги компиляции
set(COMMON_FLAGS "-Wall -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wdouble-promotion")

# Флаги компиляции для отладочной и релизной сборки
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ${COMMON_FLAGS}")

# Добавляем директиву препроцессора для пути к конфигурации
add_definitions(-DCONFIG_DIR=\"${CONFIG_DIR}\")

# Общие библиотеки для линковки
set(COMMON_LIBS ${OpenCV_LIBS} Boost::system Boost::thread yaml-cpp)

# Устанавливаем пути к исходным файлам
set(SOURCES_MANAGER
    src/tcp_receiver.cpp
    src/tcp_server.cpp
    src/tcp_sender.cpp
    src/logger.cpp
    src/data_forwarder.cpp
    src/main_manager.cpp
)

set(SOURCES_SENDER
    src/tcp_sender.cpp
    src/video_sender.cpp
    src/logger.cpp
    src/main_sender.cpp
    src/camera.cpp
    src/image.cpp
)

# Создаем исполняемые файлы
add_executable(bt_sender ${SOURCES_SENDER})
add_executable(bp_manager ${SOURCES_MANAGER})

# Подключаем заголовочные файлы и библиотеки к исполняемым файлам
target_include_directories(bt_sender PRIVATE ${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
target_include_directories(bp_manager PRIVATE ${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

# Линкуем библиотеки
target_link_libraries(bt_sender PRIVATE ${COMMON_LIBS})
target_link_libraries(bp_manager PRIVATE ${COMMON_LIBS})

# Платформозависимые настройки для Windows и Linux
if (WIN32)
    message(STATUS "Building for Windows")
    target_compile_definitions(bt_sender  PRIVATE -D_WIN32_WINNT=0x0601)
    target_compile_definitions(bp_manager PRIVATE -D_WIN32_WINNT=0x0601)
    target_link_libraries(bt_sender PRIVATE ws2_32)
    target_link_libraries(bp_manager PRIVATE ws2_32)
elseif (UNIX)
    message(STATUS "Building for Linux")
    target_compile_options(bt_sender PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(bp_manager PRIVATE -Wall -Wextra -Wpedantic)
endif()
