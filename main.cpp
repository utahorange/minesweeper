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

/* MINESWEEPER FUNCTIONS */

Minesweeper* setupGame() {
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
        game->serverMove();
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

        // connect to server
        // receive server board response
        run_client(server_ip);
        // unserialize board into game object
        Minesweeper* game = setupGame();
        playGame(game);
    }
    return 0;
} 

