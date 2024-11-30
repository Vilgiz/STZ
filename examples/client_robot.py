import socket
import json
import cv2
import numpy as np

def main():
    host = "127.0.0.1"  # IP-адрес сервера
    port = 12346        # Порт для подключения (server1_)

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((host, port))
            print(f"Connected to server at {host}:{port}")

            while True:
                data = s.recv(65536)
                if not data:
                    break
                separator_index = data.find(b'\0')
                if separator_index == -1:
                    print("Error: No metadata separator found!")
                    continue

                metadata_bytes = data[:separator_index]

                try:
                    metadata = json.loads(metadata_bytes.decode('utf-8', errors='surrogateescape'))
                    print("Parsed metadata:", metadata)
                except json.JSONDecodeError:
                    print("Error: Failed to parse metadata as JSON!")
                    continue

    except Exception as e:
        print(f"Error: {e}")
    finally:
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()