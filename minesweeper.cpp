/* INCLUDES */
#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <sstream>
// #include <regex> // https://en.cppreference.com/w/cpp/regex
#include "utilities.h"
using namespace std;

/* CONSTANTS */
#define NUM_ROWS 4
#define NUM_COLS 4
#define NUM_BOMBS (NUM_ROWS*NUM_COLS/5)

/* GLOBALS */
char gameBoard[NUM_ROWS][NUM_COLS]; // what you show user
bool realBoard[NUM_ROWS][NUM_COLS]; // 2D bool array showing where the flags are
bool bombWentOff = false;
int numFlagsLeft = NUM_BOMBS;
int numBombsFound = 0;

/* HELPER FUNCTIONS */

/** @brief returns whether a string can be converted to int */
bool isdigit(string s) {
  for (char& c : s) {
    if (!isdigit(c)) return false;
  }
  return true;
}

/** @brief 
* @return 0 for success, -1 for failure
*/
int getMoveCoords(int& r, int&c) {
    string s;
    std::cout << "Input x: ";
    getline(cin,s);
    if (s.size() != 0 && isdigit(s)) {
        c = stoi(s); 
        if (c < 0 || c >= NUM_COLS) return -1;
    } else {
        return -1;
    }
    std::cout << "Input y: ";
    getline(cin,s); // take in y
    if (s.size() != 0 && isdigit(s)) {
        r = stoi(s);
        if (r < 0 || r >= NUM_ROWS) return -1;
    } else {
        return -1;
    }
    return 0;
}

/** @brief get number of bombs in 3x3 around (r,c) */
int getNumBombs(int r, int c) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        if (r+i >= 0 && r+i < NUM_ROWS) {
            for (int j = -1; j <= 1; j++) {
                if (c+j >= 0 && c+j < NUM_COLS && !(i==0 && j==0) && realBoard[r+i][c+j]) {
                    count += 1;
                }
            }
        }
    }
    return count;
}

/** @brief see if (r_test, c_test) is in 3x3 vicinity of (r,c) */
bool inLocalThreeByThree(int r, int c, int r_test, int c_test) {
    for (int i = -1; i <=1; i++) {
        for (int j = -1; j <=1; j++) {
            if (r+i==r_test && c+j==c_test) {
                return true;
            }
        }
    }
    return false;
}

/** @brief reveal all the bombs, called by gameOver */
void revealAllBombs() {
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            if (realBoard[r][c]) {
                gameBoard[r][c] = '*'; 
            }
        }
    }
}

/* CORE FUNCTIONS */

/** @brief displays board, number of flags used 
* @note takes r,c which is coord of current move, highlighted blue
* for colors, see https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
**/
void displayBoard(int current_r=-1, int current_c=-1) {
    // number of flags can be negative
    std::stringstream oss;
    oss << numFlagsLeft;
    std::cout << "Flags: " << oss.str() << "\n\n  ";
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

/** @brief reveal recursively around square */
void reveal(int r, int c) {
    if (realBoard[r][c]) {
        gameBoard[r][c] = '*';
        bombWentOff = true;
    } else {
        int numBombs = getNumBombs(r,c);
        if (numBombs==0) {
            gameBoard[r][c] = ' ';
            for (int i = -1; i <= 1; i++) {
                if (r+i >= 0 && r+i < NUM_ROWS) {
                    for (int j = -1; j <=1; j++) {
                        if (c+j >= 0 && c+j<NUM_COLS && gameBoard[r+i][c+j] == '#' && !(i==0 && j==0)) {
                            reveal(r+i,c+j);
                        }
                    }
                }
            }
        } else {
            gameBoard[r][c] = ('0' + numBombs); 
        }
    }
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
    clearScreen();
    displayBoard();
    int r; 
    int c;

    int i = getMoveCoords(r,c);
    while (i==-1) {
        i = getMoveCoords(r,c);
    }

    for (int i = 0; i < NUM_BOMBS; i++) {
        int temp_r = randInt(0,NUM_ROWS-1);
        int temp_c = randInt(0,NUM_COLS-1);
        while (temp_r == r || temp_c == c || realBoard[temp_r][temp_c] || inLocalThreeByThree(r,c,temp_r,temp_c)) { 
            // keep finding new coords for bombs if there are already 
            // bombs there or in 3x3 around player 1st move
            temp_r = randInt(0,NUM_ROWS-1);
            temp_c = randInt(0,NUM_COLS-1);
        }
        realBoard[temp_r][temp_c] = true;
    }
    reveal(r,c);
    clearScreen();
    displayBoard();
}

/** @brief game over */
void gameOver() {
    clearScreen();
    revealAllBombs();
    displayBoard();
    if(bombWentOff) {
        std::cout << "the minefield exploded..." << std::endl;
    } else {
        std::cout << "and so you survive another day..." << std::endl;
    }
}

/** @brief play one full iteration of the game */
void playOneIteration() {
    // prompt for x, y and action
    int r;
    int c;
    int i = getMoveCoords(r,c);
    while (i == -1) {
        i = getMoveCoords(r,c);
    }
    // update screen board with blue highlight for selected coord
    clearScreen();
    displayBoard(r,c);
 
    std::cout << "[F]lag, [R]eveal, or [U]nflag: ";
    
    string action;
    do {
        getline(cin, action);
    } while (action != "f" && action != "F" && action != "r" && action != "R" && action != "u" && action != "U");
    
    // update boards
    if (action=="f" || action == "F") { // Flag
        if (gameBoard[r][c] == '#') {
            gameBoard[r][c] = 'F';
            numFlagsLeft--;
            if (realBoard[r][c]) numBombsFound++; // if actually a bomb
        }
    } else if (action == "r" || action == "R") { // Reveal
        if (gameBoard[r][c] == '#') {
            reveal(r,c);
        } 
    } else if (action == "u" || action == "U") { // unflag
        cout << gameBoard[r][c] << endl;
        if (gameBoard[r][c] == 'F'){
            gameBoard[r][c] = '#';
            numFlagsLeft++;
        }
    }
    clearScreen();
    displayBoard();
}

/** @brief play game */
void playGame() {
    std::cout << "Welcome to Minesweeper" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    setupBoard(); // board displayed with first move having been played
    while (!bombWentOff && numBombsFound!=NUM_BOMBS){
        playOneIteration();
    }
    gameOver();
}

int main() {
    playGame();
}