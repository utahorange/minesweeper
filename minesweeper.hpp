#ifndef MINESWEEPER_H
#define MINESWEEPER_H

/* INCLUDES */
#include <iostream>
#include <fstream> // for std::ofstream
#include <string>
#include <vector>
#include <sstream>

#include "utilities.h"
#include "json.hpp"
using json = nlohmann::json;

class Minesweeper {
    public:
        Minesweeper(int r, int c) : NUM_ROWS(r), NUM_COLS(c), NUM_BOMBS(r*c/5) {
            numFlagsLeft = NUM_BOMBS;
            gameBoard = std::vector<std::vector<char>>(NUM_ROWS,std::vector<char>(NUM_COLS,'#'));
            realBoard = std::vector<std::vector<bool>>(NUM_ROWS,std::vector<bool>(NUM_COLS,false));
        }
        /** @brief used when client gets first gameState.json from server */
        Minesweeper() {
            int result = unserializeGameState(); 
            if (result == -1) {
                std::cout << "Problem encountered when loading board" << std::endl;
            }
        }
        ~Minesweeper() {} 
        void setupBoard();
        void gameOver();
        void playOneIteration();

        void attemptMove();

        bool didBombGoOff() {
            return bombWentOff;
        }
        int getNumBombsFound() {
            return numBombsFound;
        }
        int getNumBombs() {
            return NUM_BOMBS;
        }
    private:
        // state
        std::vector<std::vector<char>> gameBoard; // what you show user
        std::vector<std::vector<bool>> realBoard; // 2D bool array showing where the flags are
        bool bombWentOff = false;
        int numFlagsLeft;
        int numBombsFound = 0;
        int NUM_ROWS;
        int NUM_COLS;
        int NUM_BOMBS;

        // helper functions
        int getMoveCoords(int&r, int& c);
        int getNumBombs(int r, int c);
        bool inLocalThreeByThree(int r, int c, int r_test, int c_test);
        void revealAllBombs();

        int serializeGameState();
        int unserializeGameState(); // updateBoard for client-side
        
        // functions
        void displayBoard(int current_r, int current_c);
        void reveal(int r, int c);
};

#endif

/*
Gameplay Progression

1a. Any user starts a game by calling setupGame by typing [S]
- will eventually need to generate a hash that is the game code (probably like 4 random digits)
- needs to be able to respond to client requests for game (server thread open)

1b. Any user joins a game (it may or may not exist) by typing [J]
- 

2.  

networking reqs:
- need server instance to both run the game for that player, and respond to clients that broadcast asking for gamestate via game code
- clients join the game (by server setting up connection to client, and sending game state to client)
- server needs to persistently store connections to everyone


*/