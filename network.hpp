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

// Server functions
int create_server(int port);
int accept_client(int server_socket);
int receive_json(int client_socket, char *buffer, int buffer_size);
int send_json(int client_socket, const char *json_data);

// Client functions
int connect_to_server(const char *server_ip, int port);
int send_json_to_server(int socket, const char *json_data);
int receive_json_from_server(int socket, char *buffer, int buffer_size);

// Utility functions
void close_socket(int socket);
void print_error(const std::string& message);
std::string get_local_ip_address();

// Network discovery functions
int broadcast_server_discovery(int port);
std::string discover_server_tcp(int port);

#endif // NETWORK_HPP