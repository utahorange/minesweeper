#include <iostream>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

#define NUM_ROWS 4
#define NUM_COLS 4
#define NUM_BOMBS (NUM_ROWS*NUM_COLS/5)

/* GLOBALS */

char gameBoard[NUM_ROWS][NUM_COLS]; // what you show user
bool realBoard[NUM_ROWS][NUM_COLS]; // 2D bool array showing where the flags are
// ^ these need to be dynamically allocated if 
// i want session host to be able to change them
// when making a session

bool bombWentOff = false;
int numFlagsLeft = NUM_BOMBS;
int numBombsFound = 0;

/** @brief converts current gameBoard, realBoard to json file */
json serializeGameState() {
    json gameState;
    gameState["numRows"] = NUM_ROWS;
    gameState["numCols"] = NUM_COLS;
    gameState["gameBoard"] = gameBoard;
    gameState["realBoard"] = realBoard;
    gameState["bombWentOff"] = bombWentOff ? 1 : 0;
    gameState["numFlagsLeft"] = numFlagsLeft;
    gameState["numBombsFound"] = numBombsFound;

    return gameState;
}

/** @brief plays 1st step of game and generates board */
void setupBoard() {
    // set up realBoard and gameBoard
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            gameBoard[r][c] = '#';
            realBoard[r][c] = false;
        }
    }
}
void displayBoard(int current_r=-1, int current_c=-1) {
    // number of flags can be negative
    for (int c = 0; c < NUM_COLS; c++) {
        std::cout << std::to_string(c) << " ";
    }
    std::cout << std::endl;
    for (int r = 0; r < NUM_ROWS; r++) {
        std::cout << std::to_string(r) << " ";
        for (int c = 0; c < NUM_COLS; c++) {
            if (current_r == r && current_c == c) { // current square, color blue
                std::cout << "\033[1;34m" << gameBoard[r][c] << "\033[0m ";
            } else if (gameBoard[r][c] == 'F') { // flag, color red
                std::cout << "\033[1;31mF\033[0m ";
            } else if (gameBoard[r][c] == '*') { // bomb, color magenta
                std::cout << "\033[1;35m*\033[0m "; 
            } else if (std::isdigit(gameBoard[r][c])) { // numbers, color green
                std::cout << "\033[1;32m" << gameBoard[r][c] << "\033[0m "; 
            } else { // nothing, or normal #
                std::cout << gameBoard[r][c] << " ";
            }
        }
        std::cout << std::endl;
    }
}

int main () {
    setupBoard();
    displayBoard();
    json j;
    std::ofstream outFile("assets/gameState.json");
    outFile << serializeGameState().dump(4);
}
