#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <iostream>

#define PORT 9999
#define BUFFER_SIZE 1024

// Global flag for discovery thread
volatile int discovery_running = 0;
pthread_t discovery_thread;
// Server functions
int create_server(int port);
int accept_client(int server_socket);

void* discovery_response_thread(void* arg); // TODO: what is void*
void run_server(void);

int receive_bytes(int client_socket, char *buffer, int buffer_size);
int send_bytes(int client_socket, const char *json_data);

// Client functions
int connect_to_server(const char *server_ip, int port);
void run_client(const std::string* server_ip);
void run_client_discover(void);

// Utility functions
void close_socket(int socket);
void print_error(const std::string& message);
std::string get_local_ip_address();

// Network discovery functions
int broadcast_server_discovery(int port);
std::string discover_server_tcp(int port);

#endif // NETWORK_HPP