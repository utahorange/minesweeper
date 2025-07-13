#include <thread>
#include <chrono>
#include <iostream>
#include "minesweeper.h"
#include "utilities.h"

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
void clientThread(Minesweeper* game) {
    while (!game->didBombGoOff() && game->getNumBombsFound()!=game->getNumBombs()){
        game->attemptMove();
    }
    game->gameOver();
    delete game;
    // should minesweeper.h be devoid of networking code? is this even possible?

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
    clearScreen(); 
    std::cout << "Welcome to Minesweeper" << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "[S]tart or [J]oin a game: ";

    std::string action;
    do {
        std::getline(std::cin, action);
    } while (action != "S" && action != "s" && action != "J" && action != "j");
    
    if (action == "S" || action == "s") { // is server
        Minesweeper* game = setupGame();

    } else { // client
        // request 
    }
}

/*
okay so this is an interesting problem
clients that are not server obv need to play
but must they create their own instance of the minesweeper class?
or i need to refactor clientThread in such a way that it doesn't require creating a minesweeper game
and instead just sends messages to the serverThread, which processes them?

hmmm problem is, client needs to know what to show to user, ie how to play a move
is this a static function?


so what does the client need?

it needs to:
1. send a msg to server to request board
2. process (or have access to smth to process) the board
3. continually request person for moves to send to server
4. end thread when game is finished
- this is detected by receiving a gameState.json where numBombsFound = numBombs or bombWentOff = 1

let's just have them make a minesweeper game lowkey
*/
