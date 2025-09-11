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

/** SERVER FUNCTIONS */

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

// Discovery response thread
void* discovery_response_thread(void* arg) {
    (void)arg; // Suppress unused parameter warning
    
    int discovery_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (discovery_socket < 0) {
        return nullptr;
    }
    
    struct sockaddr_in recv_addr;
    std::memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(PORT + 1);
    recv_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(discovery_socket, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) < 0) {
        close(discovery_socket);
        return nullptr;
    }
    
    std::cout << "Discovery service listening on port " << (PORT + 1) << std::endl;
    
    while (discovery_running) {
        char buffer[256];
        struct sockaddr_in from_addr;
        socklen_t from_len = sizeof(from_addr);
        
        int received = recvfrom(discovery_socket, buffer, sizeof(buffer) - 1, 0,
                               (struct sockaddr*)&from_addr, &from_len);
        
        if (received > 0) {
            buffer[received] = '\0';
            if (std::strcmp(buffer, "NETWORKING_DISCOVERY") == 0) {
                std::cout << "Discovery request from " 
                          << inet_ntoa(from_addr.sin_addr) << std::endl;
                
                const char *response = "NETWORKING_SERVER";
                sendto(discovery_socket, response, std::strlen(response), 0,
                       (struct sockaddr*)&from_addr, from_len);
            }
        }
    }
    
    close(discovery_socket);
    return nullptr;
}

void run_server() {
    std::cout << "Starting Minesweeper server..." << std::endl;
    
    int server_socket = create_server(PORT);
    if (server_socket == -1) {
        std::cout << "Failed to create server" << std::endl;
        return;
    }
    
    // Start discovery thread
    discovery_running = 1;
    if (pthread_create(&discovery_thread, nullptr, discovery_response_thread, nullptr) != 0) {
        std::cout << "Warning: Failed to start discovery service" << std::endl;
    }
    
    std::cout << "Server is running. Waiting for client connections..." << std::endl;
    std::cout << "Server IP: " << get_local_ip_address() << std::endl;
    
    while (true) {
        int client_socket = accept_client(server_socket);
        if (client_socket == -1) {
            continue;
        }
        
        char buffer[BUFFER_SIZE];
        int bytes_received = receive_bytes(client_socket, buffer, BUFFER_SIZE);
        
        if (bytes_received > 0) {
            std::cout << "Received JSON from client: " << buffer << std::endl;
            
            // Create a response JSON
            std::ostringstream response_oss;
            response_oss << "{\"status\": \"success\", \"message\": \"Server received your data\", \"received\": " << buffer << "}";
            std::string response = response_oss.str();
            
            int bytes_sent = send_bytes(client_socket, response.c_str());
            if (bytes_sent > 0) {
                std::cout << "Sent response to client" << std::endl;
                usleep(100000); // Add a short delay to ensure data is sent
            } else {
                std::cout << "Failed to send response to client" << std::endl;
            }
        } else {
            std::cout << "Failed to receive data from client" << std::endl;
        }
        
        close_socket(client_socket);
    }
    
    // Clean up discovery thread
    discovery_running = 0;
    pthread_join(discovery_thread, nullptr);
    
    close_socket(server_socket);
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

/** CLIENT FUNCTIONS */

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

void run_client(const std::string& server_ip) {
    std::cout << "Connecting to server at " << server_ip << ":" << PORT << "..." << std::endl;
    
    int client_socket = connect_to_server(server_ip.c_str(), PORT);
    if (client_socket == -1) {
        std::cout << "Failed to connect to server" << std::endl;
        return;
    }
    
    // Create a sample JSON message
    std::ostringstream json_oss;
    json_oss << "{\"type\": \"greeting\", \"content\": \"Hello from client!\", \"timestamp\": " 
             << std::time(nullptr) << "}";
    std::string json_message = json_oss.str();
    
    std::cout << "Sending JSON to server: " << json_message << std::endl;
    
    int bytes_sent = send_bytes(client_socket, json_message.c_str());
    if (bytes_sent > 0) {
        std::cout << "Successfully sent " << bytes_sent << " bytes to server" << std::endl;
        
        // Receive response from server
        char buffer[BUFFER_SIZE];
        int bytes_received = receive_bytes(client_socket, buffer, BUFFER_SIZE);
        
        if (bytes_received > 0) {
            std::cout << "Received response from server: " << buffer << std::endl;
        } else {
            std::cout << "Failed to receive response from server" << std::endl;
        }
    } else {
        std::cout << "Failed to send data to server" << std::endl;
    }
    
    close_socket(client_socket);
}

void run_client_discover() {
    std::cout << "Searching for servers on the network..." << std::endl;
    
    std::string server_ip = discover_server_tcp(PORT);
    if (server_ip.empty()) {
        std::cout << "No servers found on the network" << std::endl;
        std::cout << "Make sure a server is running on another machine" << std::endl;
        return;
    }
    
    std::cout << "Found server at: " << server_ip << std::endl;
    run_client(server_ip);
}

void run_client_auto() {
    std::string local_ip = get_local_ip_address();
    if (local_ip.empty()) {
        std::cout << "Error: Could not determine local IP address" << std::endl;
        return;
    }
    
    std::cout << "Auto-detected local IP: " << local_ip << std::endl;
    std::cout << "Connecting to server at " << local_ip << ":" << PORT << "..." << std::endl;
    
    int client_socket = connect_to_server(local_ip.c_str(), PORT);
    if (client_socket == -1) {
        std::cout << "Failed to connect to server" << std::endl;
        return;
    }
    
    // Create a sample JSON message
    std::ostringstream json_oss;
    json_oss << "{\"type\": \"greeting\", \"content\": \"Hello from client!\", \"timestamp\": " 
             << std::time(nullptr) << "}";
    std::string json_message = json_oss.str();
    
    std::cout << "Sending JSON to server: " << json_message << std::endl;
    
    int bytes_sent = send_bytes(client_socket, json_message.c_str());
    if (bytes_sent > 0) {
        std::cout << "Successfully sent " << bytes_sent << " bytes to server" << std::endl;
        
        // Receive response from server
        char buffer[BUFFER_SIZE];
        int bytes_received = receive_bytes(client_socket, buffer, BUFFER_SIZE);
        
        if (bytes_received > 0) {
            std::cout << "Received response from server: " << buffer << std::endl;
        } else {
            std::cout << "Failed to receive response from server" << std::endl;
        }
    } else {
        std::cout << "Failed to send data to server" << std::endl;
    }
    
    close_socket(client_socket);
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