#include "network.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <sys/select.h>
#include <iostream>
#include <sstream>

// Create a server socket and bind it to the specified port
int create_server(int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        print_error("Failed to create server socket");
        return -1;
    }

    // Set socket option to reuse address
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        print_error("Failed to set socket options");
        close(server_socket);
        return -1;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        print_error("Failed to bind server socket");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, 5) < 0) {
        print_error("Failed to listen on server socket");
        close(server_socket);
        return -1;
    }

    std::cout << "Server started and listening on port " << port << std::endl;
    return server_socket;
}

// Accept a client connection
int accept_client(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket < 0) {
        print_error("Failed to accept client connection");
        return -1;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Client connected from " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
    
    return client_socket;
}

/** @brief store bytes data
* @return 0 for success, -1 for failure
*/
int receive_bytes(int client_socket, char *buffer, int buffer_size) {
    std::uint32_t data_len;
    int bytes_received = recv(client_socket, &data_len, sizeof(data_len), 0);
    if (bytes_received <= 0) {
        return -1;
    }
    
    // Convert from network byte order to host byte order
    data_len = ntohl(data_len);
    
    if (data_len > static_cast<std::uint32_t>(buffer_size - 1)) {
        std::cout << "Warning: JSON data too large for buffer (" << data_len << " bytes)" << std::endl;
        return -1;
    }
    
    // Receive the actual JSON data
    bytes_received = recv(client_socket, buffer, data_len, 0);
    if (bytes_received <= 0) {
        return -1;
    }
    
    // Null-terminate the received data
    buffer[bytes_received] = '\0';
    
    return bytes_received;
}

// Send JSON data to a client
int send_bytes(int client_socket, const char *json_data) {
    std::uint32_t data_len = std::strlen(json_data);
    std::uint32_t network_length = htonl(data_len);
    
    // Send the length first
    int bytes_sent = send(client_socket, &network_length, sizeof(network_length), 0);
    if (bytes_sent <= 0) {
        return -1;
    }
    
    // Send the data
    bytes_sent = send(client_socket, json_data, data_len, 0);
    if (bytes_sent <= 0) {
        return -1;
    }
    
    return bytes_sent;
}

// Connect to a server
int connect_to_server(const char *server_ip, int port) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        print_error("Failed to create client socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        print_error("Invalid server IP address");
        close(client_socket);
        return -1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        print_error("Failed to connect to server");
        close(client_socket);
        return -1;
    }

    std::cout << "Connected to server " << server_ip << ":" << port << std::endl;
    return client_socket;
}

// Close a socket
void close_socket(int socket) {
    if (socket >= 0) {
        close(socket);
    }
}

// Print error message with errno information
void print_error(const std::string& message) {
    std::perror(message.c_str());
}

// Get local IP address
std::string get_local_ip_address() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return "";
    }
    
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("8.8.8.8"); // Google DNS
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return "";
    }
    
    socklen_t len = sizeof(addr);
    if (getsockname(sock, (struct sockaddr*)&addr, &len) < 0) {
        close(sock);
        return "";
    }
    
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    close(sock);
    
    return std::string(ip);
}

// Broadcast server discovery
int broadcast_server_discovery(int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return -1;
    }
    
    int broadcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        close(sock);
        return -1;
    }
    
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port + 1);
    addr.sin_addr.s_addr = INADDR_BROADCAST;
    
    const char *message = "NETWORKING_DISCOVERY";
    int sent = sendto(sock, message, std::strlen(message), 0, 
                      (struct sockaddr*)&addr, sizeof(addr));
    
    close(sock);
    return sent;
}

// Discover server using TCP
std::string discover_server_tcp(int port) {
    // First try UDP broadcast
    if (broadcast_server_discovery(port) < 0) {
        return "";
    }
    
    // Wait for response
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return "";
    }
    
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port + 1);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return "";
    }
    
    // Set timeout
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    char buffer[256];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    
    int received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                           (struct sockaddr*)&from_addr, &from_len);
    
    close(sock);
    
    if (received > 0) {
        buffer[received] = '\0';
        if (std::strcmp(buffer, "NETWORKING_SERVER") == 0) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &from_addr.sin_addr, ip, INET_ADDRSTRLEN);
            return std::string(ip);
        }
    }
    
    return "";
} 