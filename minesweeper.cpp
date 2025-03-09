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
#define NUM_ROWS 8
#define NUM_COLS 10
#define NUM_BOMBS (NUM_ROWS*NUM_COLS*0.2)

/* GLOBALS */
std::string gameBoard[NUM_ROWS][NUM_COLS]; // what you show user
bool realBoard[NUM_ROWS][NUM_COLS]; // 2D bool array showing where the flags are
bool bombWentOff = false;
int numFlagsLeft = NUM_BOMBS;
int numBombsFound = 0;

/* HELPER FUNCTIONS */
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

// see if (r_test, c_test) is in 3x3 vicinity of (r,c)
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

void revealAllBombs() { // called by gameOver
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            if (realBoard[r][c]) {
                gameBoard[r][c] = "\033[1;35m*\033[0m";
            }
        }
    }
}
/* CORE FUNCTIONS */
void displayBoard() {
    // print stats - number of flags, can be negative
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
            std::cout << gameBoard[r][c] << " ";
        }
        std::cout << std::endl;
    }
}

void reveal(int r, int c) {
    // if (r < 0 || c < 0 || r >= NUM_ROWS || c >= NUM_COLS) return; // really only used for revealing full board
    if (realBoard[r][c]) {
        gameBoard[r][c] = "\033[1;35m*\033[0m";
        bombWentOff = true;
        // TODO: should reveal all bombs
    } else {
        int numBombs = getNumBombs(r,c);
        if (numBombs==0) {
            gameBoard[r][c] = " ";
            for (int i = -1; i <= 1; i++) {
                if (r+i >= 0 && r+i < NUM_ROWS) {
                    for (int j = -1; j <=1; j++) {
                        if (c+j >= 0 && c+j<NUM_COLS && gameBoard[r+i][c+j] == "#" && !(i==0 && j==0)) {
                            reveal(r+i,c+j);
                        }
                    }
                }
            }
        } else {
            // https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
            gameBoard[r][c] = "\033[1;32m" + std::to_string(numBombs) +"\033[0m"; 
        }
    }
}

// plays 1st step of game, plus generates board
void setupBoard() {
    // set up realBoard and gameBoard
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            gameBoard[r][c] = "#";
            realBoard[r][c]=false;
        }
    }
    clearScreen();
    displayBoard();
    int r = 0;
    int c = 0;
    std::cout << "Input x: ";
    std::cin >> c; // take in x
    std::cout << "Input y: ";
    std::cin >> r; // take in y

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
    // clearScreen();
    displayBoard();
}

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

void playOneIteration() {
    // prompt for x, y and action
    int r = 0;
    int c = 0;
    std::cout << "Input x: ";
    std::cin >> c; // take in x
    std::cout << "Input y: ";
    std::cin >> r; // take in y
    // if (gameBoard[r][c] != "#" || gameBoard[r][c] != "F") return;
    
    // highlight the square user wants to do smth to
    gameBoard[r][c] = "\033[1;34m" + gameBoard[r][c] + "\033[0m"; 
    clearScreen();
    displayBoard();
    // TODO: fix blue highlight being left behind even when quit current run

    std::cout << "[F]lag, [R]eveal, or [U]nflag: ";
    string action;
    cin.ignore(10000, '\n');
    getline(cin, action);

    // update boards
    // TODO: make it case insensitive?
    if (action=="f" || action == "F") { // Flag
        // TODO: can only flag on #
        gameBoard[r][c] = "\033[1;31mF\033[0m";
        numFlagsLeft--;
        if (realBoard[r][c]) numBombsFound++; // if actually a bomb
    } else if (action == "r" || action == "R") { // Reveal
        // TODO: can only reveal on #
        reveal(r,c);
    } else if (action == "u" || action == "U") { // unflag
        // TODO: can only flag on F
        gameBoard[r][c] = "#";
        numFlagsLeft++;
    }
    
    clearScreen();
    displayBoard();
}

void playGame() {
    std::cout << "Welcome to Minesweeper" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    setupBoard();
    // board now displayed with first move having been played
    while (!bombWentOff && numBombsFound!=NUM_BOMBS){
        playOneIteration();
    }
    gameOver();
}

int main() {
    playGame();
}