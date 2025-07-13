#ifndef MINESWEEPER_HPP
#define MINESWEEPER_HPP

/* INCLUDES */
#include <iostream>
#include <fstream> // for std::ofstream
#include <string>
#include <vector>
#include <sstream>

#include "utilities.hpp"
#include "network.hpp"
#include "json.hpp"
using json = nlohmann::json;

class Minesweeper {
    public:
        Minesweeper(int r, int c) : NUM_ROWS(r), NUM_COLS(c), NUM_BOMBS(r*c/5) {
            numFlagsLeft = NUM_BOMBS;
            gameBoard = std::vector<std::vector<char>>(NUM_ROWS,std::vector<char>(NUM_COLS,'#'));
            realBoard = std::vector<std::vector<bool>>(NUM_ROWS,std::vector<bool>(NUM_COLS,false));
        }
        /** @brief used when client gets first game state from server */
        Minesweeper() {
            int result = unserializeStateFromJSON(); // TODO: this is actually unserialize from bytes, not jSON
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

        int serializeStateToJSON();
        int unserializeStateFromJSON(); // updateBoard for client-side
        
        // functions
        void displayBoard(int current_r, int current_c);
        void reveal(int r, int c);
};

#endif
