import socket

server_ip = '127.0.0.1'
server_port = 8080

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client_socket.connect((server_ip, server_port))


# Receive username prompt
username = client_socket.recv(50)
print(username.decode('utf-8'), end="")

# Send username
username = input()
client_socket.send((username + '\n').encode('utf-8'))

# Receive password prompt
password = client_socket.recv(50)
print(password.decode('utf-8'), end="")

# Send password
password = input()
client_socket.send((password + '\n').encode('utf-8'))

auth_message = client_socket.recv(1024)

if "Authentication successful." in auth_message.decode('utf-8'):
    prompt = client_socket.recv(100)
    print(prompt.decode('utf-8'), end="")

    command = input()
    client_socket.send((command + '\n').encode('utf-8'))

    if "download" in command:
        out_file = "downloaded_" + command.split(" ")[1]
        with open(out_file, 'w') as f:
            while True:
                data = client_socket.recv(1024)
 
                if "Transfer Complete" in data.decode('utf-8'):
                    break
 
                if not data:
                    break
                
                f.write(data.decode('utf-8'))

    elif "upload" in command:
        filename = command.split(" ")[1]
        
        try:
            with open(filename, 'r') as f:
                while True:
                    data = f.read(1024)

                    if not data:
                        break

                    client_socket.send((data).encode('utf-8'))

        except FileNotFoundError:
            print(f"File '{filename}' not found.")             

        client_socket.sendall(b'\n\n')

client_socket.close()