//file_to_send.txt

//TCPFileClient.cpp
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream> // For file I/O

#define PORT 9091
#define BUFFER_SIZE 1024

const char* FILENAME_TO_SEND = "file_to_send.txt";

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // --- 1. Check if file exists ---
    std::ifstream infile(FILENAME_TO_SEND, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file '" << FILENAME_TO_SEND << "'" << std::endl;
        std::cerr << "Please create this file in the same directory." << std::endl;
        return -1;
    }

    // --- Standard Client Setup (Socket, Connect) ---
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    std::cout << "Connected to server, starting file send..." << std::endl;

    // --- Send File ---
    char buffer[BUFFER_SIZE] = {0};
    
    // 2. Loop: read data from file, send to socket
    while (infile.read(buffer, BUFFER_SIZE)) {
        if (send(sock, buffer, infile.gcount(), 0) < 0) {
            std::cerr << "Send failed" << std::endl;
            break;
        }
        // Clear buffer after send
        memset(buffer, 0, BUFFER_SIZE);
    }
    
    // 3. Send the last remaining chunk (if file size wasn't a perfect multiple)
    if (infile.gcount() > 0) {
        if (send(sock, buffer, infile.gcount(), 0) < 0) {
            std::cerr << "Send failed on final chunk" << std::endl;
        }
    }

    std::cout << "File sent successfully." << std::endl;

    // --- Cleanup ---
    // 4. Closing the socket is crucial!
    // This signals the server's recv() loop to stop.
    infile.close();
    close(sock);
    return 0;
}

//TCPFileServer.cpp

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream> // For file I/O

#define PORT 9091
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // --- Standard Server Setup (Socket, Bind, Listen) ---
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << " for a file..." << std::endl;

    // --- Accept Connection ---
    if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        std::cerr << "Accept failed" << std::endl;
        return -1;
    }

    std::cout << "Client connected, starting file receive." << std::endl;

    // --- Receive File ---
    // 1. Open a new file to write the received data
    // std::ios::binary is important for non-text files (images, etc.)
    std::ofstream received_file("received_file.txt", std::ios::binary);
    if (!received_file.is_open()) {
        std::cerr << "Could not open output file for writing." << std::endl;
        close(client_socket);
        close(server_fd);
        return -1;
    }

    int bytes_received;
    // 2. Loop: read data from socket, write to file
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        received_file.write(buffer, bytes_received);
    }

    // 3. recv() returns 0 when the client closes the connection
    std::cout << "File transfer complete. File saved as 'received_file.txt'." << std::endl;

    // --- Cleanup ---
    received_file.close();
    close(client_socket);
    close(server_fd);
    return 0;
}

//TCPHelloClient.cpp

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>  // For inet_pton
#include <unistd.h>     // For close, recv, send

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // 1. Create Client Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // 2. Configure Server Address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address "127.0.0.1" (localhost) to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return -1;
    }

    // 3. Connect to Server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    // 4. Send "Hello" to Server
    const char* hello_msg = "Hello Server!";
    send(sock, hello_msg, strlen(hello_msg), 0);
    std::cout << "Sent: " << hello_msg << std::endl;

    // 5. Receive "Hello" from Server
    int valread = recv(sock, buffer, BUFFER_SIZE, 0);
    if (valread > 0) {
        std::cout << "Server says: " << buffer << std::endl;
    }

    // 6. Close Socket
    close(sock);
    return 0;
}

//TCPHelloServer.cpp

#include <iostream>
#include <cstring>      // For memset
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in, INADDR_ANY, htons
#include <unistd.h>     // For close, recv, send

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // 1. Create Server Socket
    // AF_INET = IPv4, SOCK_STREAM = TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // 2. Configure Server Address
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on 0.0.0.0
    address.sin_port = htons(PORT);       // Port in network byte order

    // 3. Bind the Socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    // 4. Listen for Connections
    if (listen(server_fd, 3) < 0) { // 3 = max pending connections
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << "..." << std::endl;

    // 5. Accept a Connection
    // This is a blocking call and waits for a client
    if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        std::cerr << "Accept failed" << std::endl;
        return -1;
    }

    std::cout << "Client connected!" << std::endl;

    // 6. Receive "Hello" from Client
    int valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (valread > 0) {
        std::cout << "Client says: " << buffer << std::endl;
    }

    // 7. Send "Hello" back to Client
    const char* hello_msg = "Hello Client!";
    send(client_socket, hello_msg, strlen(hello_msg), 0);
    std::cout << "Replied with: " << hello_msg << std::endl;

    // 8. Close Sockets
    close(client_socket);
    close(server_fd);
    return 0;
}
