import socket


SERVER_IP = "192.168.65.229"  
PORT = 12345
BUFFER_SIZE = 4096

try:
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((SERVER_IP, PORT))
    print("Connected to server. Type commands to execute or 'exit' to quit.")
except Exception as e:
    print(f"Connection failed: {e}")
    exit(1)

while True:
    try:
        command = input("> ")
        client_socket.send(command.encode())  

        
        if command.lower() == "exit":
            break

        
        response = client_socket.recv(BUFFER_SIZE).decode()
        print(response)
    except Exception as e:
        print(f"An error occurred: {e}")
        break

client_socket.close()
