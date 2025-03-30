#include <thread>
#include <chrono>
#include <iostream>
#include "minesweeper.h"
#include "utilities.h"

void gameThread() { // the thread for running a game, should start serverThread 
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
    Minesweeper game = Minesweeper(numRows,numCols);

    game.setupBoard(); // board displayed with first move having been played
    
    while (!game.didBombGoOff() && game.getNumBombsFound()!=game.getNumBombs()){
        game.playOneIteration();
    }
    game.gameOver();
}

void serverThread() {

}

void clientThread() {
    // code for any client (other player) to play the game

    // some func to communicate w server on udp datagrams
    // sending json 

    // enter game code
    // save game code somewhere
    // while not getting proper response to server w game code, reprompt for game code
    // proper response is auth in the form of getting gameState.json back or equiv :skull: - might need to rewrite?
    // q ^ was, why have json if we just doing plaintext transfer over udp?

    // wait for user input
    // continually, playOneIteration but like to send over network if not server

}

int main() {
    // concurrency stuff, shuffling btwn server thread and game thread
    // all game stuff abstracted into Minesweeper class
    
    // prompt user to either [S]tart or [J]oin a game
    // run serverThread or clientThread
    // if client, no need to run gameThread/multi-thread
    
    clearScreen(); 
    std::cout << "Welcome to Minesweeper" << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "[S]tart or [J]oin a game: ";

    std::string action;
    do {
        std::getline(std::cin, action);
    } while (action != "S" && action != "s" && action != "J" && action != "j");
    
    if (action == "S" || action == "s") { // is server
        gameThread();
        // std::thread server(serverThread);

    } else { // client
        // client - prob doesn't need to be threaded
    }
}
