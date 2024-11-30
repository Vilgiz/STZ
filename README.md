# Инструкция по использованию проекта Bipitrone

## Описание проекта
Проект **Bipitrone** предназначен для обработки и передачи видеоданных с использованием TCP. Программа состоит из двух основных приложений:

1. **bt_sender** – отправляет видеопоток.
2. **bp_manager** – принимает данные и передаёт их клиентам на другие порты.

---

## Установка зависимостей на Ubuntu

Перед началом работы необходимо установить все зависимости, включая библиотеки OpenCV, Boost и YAML-CPP. Используйте следующую команду для установки:

```bash
sudo apt update && sudo apt install -y cmake g++ libopencv-dev libboost-all-dev libyaml-cpp-dev
```

## Сборка проекта
1. Склонировать репозиторий
2. Создание директории сборки
```bash
mkdir build
cd build
```
3. Генерация make файлов
```bash
cmake ..
```
4. Сборка проекта
```bash
make
```
## Запуск программ
После успешной сборки исполняемые файлы находятся в директории `build/bin`.
1. Запуск отправителя (bt_sender):
```
./bit/bt_sender
```
2. Запуск менеджера передачи (bp_manager):
```
./bin/bp_manager
```
## Файл конфигации
Файл конфигурации videoConfigure.yaml должен быть расположен в директории config. 
Пример файла:
```yaml
cv_port: 12345
robot_port: 12346
client_port: 12347
targetFPS: 30
videoWidth: 1280
videoHeight: 720
ip_address: "127.0.0.1"
```
Файл videoConfigure.yaml используется обеими программами для определения портов передачи и получения данных.