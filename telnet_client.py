import socket

SERVER_IP = "127.0.0.1"  # wskaz adres servera
PORT = 12345
BUFFER_SIZE = 1024

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((SERVER_IP, PORT))

print("Connected to server. Type commands to execute or 'exit' to quit.")

while True:
    command = input("> ")
    client_socket.send(command.encode())
    if command.lower() == "exit":
        break
    response = client_socket.recv(BUFFER_SIZE).decode()
    print(response)

client_socket.close()
