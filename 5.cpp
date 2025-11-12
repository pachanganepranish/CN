//TCPClient.cpp

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>  // For inet_pton
#include <unistd.h>     // For close, recv

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // --- 1. Create Socket ---
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // --- 2. Configure Server Address ---
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address from text to binary
    // "127.0.0.1" is localhost
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return -1;
    }

    // --- 3. Connect to Server ---
    // This is a blocking call to establish the connection
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }
    std::cout << "Connected to server!" << std::endl;

    // --- 4. Send Data ---
    const char* hello_msg = "Hello from client!";
    send(sock, hello_msg, strlen(hello_msg), 0);
    std::cout << "Hello message sent to server." << std::endl;

    // --- 5. Receive Data ---
    int valread = recv(sock, buffer, BUFFER_SIZE, 0);
    if (valread < 0) {
        std::cerr << "Receive failed" << std::endl;
    } else {
        std::cout << "Server says: " << buffer << std::endl;
    }

    // --- 6. Close Socket ---
    close(sock);
    return 0;
}


//TCPServer.cpp

#include <iostream>     // For std::cout, std::cerr
#include <cstring>      // For memset
#include <sys/socket.h> // For socket, bind, listen, accept
#include <netinet/in.h> // For sockaddr_in, INADDR_ANY, htons
#include <unistd.h>     // For close, recv

#define PORT 9090 // The port to listen on
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // --- 1. Create Socket ---
    // socket(domain, type, protocol)
    // AF_INET = IPv4, SOCK_STREAM = TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // --- 2. Configure Address Structure ---
    // Zero out the structure
    memset(&address, 0, sizeof(address)); 
    address.sin_family = AF_INET;
    // INADDR_ANY means listen on all available IPs (0.0.0.0)
    address.sin_addr.s_addr = INADDR_ANY; 
    // htons = "host to network short"
    // Converts the port number to network byte order
    address.sin_port = htons(PORT); 

    // --- 3. Bind the Socket ---
    // Assign the address and port to the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    // --- 4. Listen for Connections ---
    // Put the socket in passive listening mode
    // The '3' is the backlog: max number of pending connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << "..." << std::endl;

    // --- 5. Accept a Connection ---
    // This is a blocking call! It waits for a client.
    // It returns a *new* socket file descriptor for the client.
    if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        std::cerr << "Accept failed" << std::endl;
        return -1;
    }
    std::cout << "Client connected!" << std::endl;

    // --- 6. Receive Data ---
    // Read data from the client's socket
    int valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (valread < 0) {
        std::cerr << "Receive failed" << std::endl;
    } else {
        std::cout << "Client says: " << buffer << std::endl;
    }

    // --- 7. Send Data ---
    const char* hello_msg = "Hello from server!";
    send(client_socket, hello_msg, strlen(hello_msg), 0);
    std::cout << "Hello message sent to client." << std::endl;

    // --- 8. Close Sockets ---
    close(client_socket); // Close the client connection
    close(server_fd);     // Close the listening socket
    return 0;
}


//UDPClient.cpp

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 9876
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    // --- 1. Create Socket ---
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // --- 2. Configure Server Address ---
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return -1;
    }

    // --- 3. Send Data ---
    // No 'connect()' step is needed for UDP
    const char* hello_msg = "Hello from client";
    sendto(sockfd, hello_msg, strlen(hello_msg), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    std::cout << "Hello message sent." << std::endl;

    // --- 4. Receive Data ---
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&servaddr, &len);
    if (n < 0) {
        std::cerr << "recvfrom failed" << std::endl;
    }

    buffer[n] = '\0';
    std::cout << "Server says: " << buffer << std::endl;

    // --- 5. Close Socket ---
    close(sockfd);
    return 0;
}

// UDPServer.cpp

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 9876
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    // --- 1. Create Socket ---
    // SOCK_DGRAM = UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // --- 2. Configure Server Address ---
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // --- 3. Bind the Socket ---
    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    std::cout << "UDP Server is listening on port " << PORT << "..." << std::endl;

    socklen_t len = sizeof(cliaddr);

    // --- 4. Receive Data ---
    // This is a blocking call!
    // It waits for any packet and populates 'cliaddr'
    // with the sender's address information.
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&cliaddr, &len);
    if (n < 0) {
        std::cerr << "recvfrom failed" << std::endl;
    }

    buffer[n] = '\0'; // Null-terminate the string
    std::cout << "Client says: " << buffer << std::endl;

    // --- 5. Send Data ---
    const char* hello_msg = "Hello from server";
    // Send the reply *to* the client address we just received
    sendto(sockfd, hello_msg, strlen(hello_msg), 0, (const struct sockaddr*)&cliaddr, len);
    std::cout << "Hello message sent." << std::endl;

    // --- 6. Close Socket ---
    close(sockfd);
    return 0;
}
