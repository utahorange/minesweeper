#include <thread>
#include <chrono>
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <regex>

#include "minesweeper.hpp"
#include "utilities.hpp"
#include "network.hpp"


/* NETWORKING FUNCTIONS */

#define PORT 9999
#define BUFFER_SIZE 1024

// Global flag for discovery thread
volatile int discovery_running = 0;
pthread_t discovery_thread;

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


/* MINESWEEPER FUNCTIONS */

Minesweeper* setupGame() { // the thread for running a game, should start serverThread 
    int numRows = -1;
    int numCols = -1;

    std::string s;
    std::cout << "Input number of columns: ";
    std::getline(std::cin,s); 
    while (s.size() != 0 && isdigit(s) && (numCols < 0 || numCols > INT32_MAX)) {
        numCols = std::stoi(s);
    }
    std::cout << "Input number of rows: ";
    std::getline(std::cin,s);
    while (s.size() != 0 && isdigit(s) && (numRows < 0 || numRows > INT32_MAX)) {
        numRows = std::stoi(s); 
    }
    Minesweeper* game = new Minesweeper(numRows,numCols);

    game->setupBoard(); // board displayed with first move having been played
    return game;
}

/** @brief code for anyone including server to play the game */
void playGame(Minesweeper* game) {
    while (!game->didBombGoOff() && game->getNumBombsFound()!=game->getNumBombs()){
        game->attemptMove();
    }
    game->gameOver();
}

int main(void) {
    clearScreen(); 
    std::cout << "Welcome to Minesweeper" << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "[S]tart or [J]oin a game: ";

    std::string action;
    do {
        std::getline(std::cin, action);
    } while (action != "S" && action != "s" && action != "J" && action != "j");
    
    if (action == "S" || action == "s") { // is server
        std::thread server_thread(run_server);
        Minesweeper* game = setupGame();
        std::thread client_thread(playGame, game);
        delete game;
    } else { // client
        std::string server_ip;
        const std::regex regex_ip(
            "^(25[0-5]|2[0-4]\\d|[01]?\\d\\d?)\\."
            "(25[0-5]|2[0-4]\\d|[01]?\\d\\d?)\\."
            "(25[0-5]|2[0-4]\\d|[01]?\\d\\d?)\\."
            "(25[0-5]|2[0-4]\\d|[01]?\\d\\d?)$",
            std::regex::ECMAScript);        
        std::cout << "Server IP in the form of 192.168.1.1: ";
        do {
            std::getline(std::cin, server_ip);
        } while (! std::regex_match(server_ip, regex_ip));

        run_client(server_ip);
        // receive server game json
        // make json into game object
        // playGame()
    }
    return 0;
} 

