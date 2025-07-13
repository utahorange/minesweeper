#include "minesweeper.hpp"

/* HELPER FUNCTIONS */

/** @brief 
* @return 0 for success, -1 for failure
*/
int Minesweeper::getMoveCoords(int& r, int&c) {
    std::string s;
    std::cout << "Input x: ";
    std::getline(std::cin,s);
    if (s.size() != 0 && isdigit(s)) {
        c = std::stoi(s); 
        if (c < 0 || c >= NUM_COLS) return -1;
    } else {
        return -1;
    }
    std::cout << "Input y: ";
    std::getline(std::cin,s); // take in y
    if (s.size() != 0 && isdigit(s)) {
        r = std::stoi(s);
        if (r < 0 || r >= NUM_ROWS) return -1;
    } else {
        return -1;
    }
    return 0;
}

/** @brief get number of bombs in 3x3 around (r,c) */
int Minesweeper::getNumBombs(int r, int c) {
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
bool Minesweeper::inLocalThreeByThree(int r, int c, int r_test, int c_test) {
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
void Minesweeper::revealAllBombs() {
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            if (realBoard[r][c]) {
                gameBoard[r][c] = '*'; 
            }
        }
    }
}

/** @brief converts current gameBoard, realBoard to json file */
int Minesweeper::serializeGameState() {
    json gameState;
    gameState["numRows"] = NUM_ROWS;
    gameState["numCols"] = NUM_COLS;
    gameState["numBombs"] = NUM_BOMBS;
    
    gameState["gameBoard"] = json::array();
    for (int i = 0; i < NUM_ROWS; ++i) {
        json row = json::array();
        for (int j = 0; j < NUM_COLS; ++j) {
            row.push_back(gameBoard[i][j]);
        }
        gameState["gameBoard"].push_back(row);
    }

    gameState["realBoard"] = realBoard;
    gameState["bombWentOff"] = bombWentOff ? 1 : 0;
    gameState["numFlagsLeft"] = numFlagsLeft;
    gameState["numBombsFound"] = numBombsFound;

    std::ofstream outFile("assets/gameState.json");
    outFile << gameState.dump(4);
    
    return 0;
}

/** @brief loads json file into gameBoard, realBoard 
 * @return 0 for success, -1 if no gameState.json file to read
*/
int Minesweeper::unserializeGameState() {
    std::ifstream inFile("assets/gameState.json");
    if (!inFile.good()) return -1;
    json loadedState = json::parse(inFile);

    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            gameBoard[r][c] = loadedState["gameBoard"][r][c].get<char>();
            realBoard[r][c] = loadedState["realBoard"][r][c].get<bool>();
        }
    }
    bombWentOff = (loadedState["bombWentOff"]==1) ? true : false;
    numFlagsLeft = loadedState["numFlagsLeft"];
    numBombsFound = loadedState["numBombsFound"];

    return 0;
}

/* CORE FUNCTIONS */

/** @brief displays board, number of flags used 
* @note takes r,c which is coord of current move, highlighted blue
**/
void Minesweeper::displayBoard(int current_r=-1, int current_c=-1) {
    // number of flags can be negative
    std::stringstream oss;
    oss << numFlagsLeft;
    std::cout << "Flags Left: " << oss.str() << "\n\n  ";
    for (int c = 0; c < NUM_COLS; c++) {
        std::cout << std::to_string(c).back() << " ";
    }
    std::cout << std::endl;

    for (int r = 0; r < NUM_ROWS; r++) {
        std::cout << std::to_string(r).back() << " ";
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
void Minesweeper::reveal(int r, int c) {
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

/** @brief plays 1st step of game, generates board, saves board to gameState.json */
void Minesweeper::setupBoard() {
    // set up realBoard and gameBoard
    
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

    // should other players be able to see an empty board and maybe collab with player on which spot to pick for first move? - no for now
    serializeGameState();
}

/** @brief game over */
void Minesweeper::gameOver() {
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
void Minesweeper::playOneIteration() {
    int r;
    int c;
    int i = getMoveCoords(r,c);
    while (i == -1) {
        i = getMoveCoords(r,c);
    }
    // update screen board with blue highlight for selected coord
    clearScreen();
    displayBoard(r,c);
 
    std::cout << "[F]lag, [R]eveal, [U]nflag, or [Q]uit turn: ";
    
    std::string action;
    do {
        std::getline(std::cin, action);
    } while (action != "f" && action != "F" && action != "r" && action != "R" && action != "u" && action != "U" && action != "Q" && action != "q");
    
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
        if (gameBoard[r][c] == 'F'){
            gameBoard[r][c] = '#';
            numFlagsLeft++;
        }
    } else { // was Q or q
        ;
    }
    clearScreen();
    displayBoard();
}

/** @brief networked version of playOneIteration */
void Minesweeper::attemptMove() {
    int r;
    int c;
    int i = getMoveCoords(r,c);
    while (i == -1) {
        i = getMoveCoords(r,c);
    }
    // update screen board with blue highlight for selected coord
    clearScreen();
    displayBoard(r,c);
 
    std::cout << "[F]lag, [R]eveal, [U]nflag, or [Q]uit turn: ";
    
    std::string action;
    do {
        std::getline(std::cin, action);
    } while (action != "f" && action != "F" && action != "r" && action != "R" && action != "u" && action != "U" && action != "Q" && action != "q");
    
    if ((action=="f" || action == "F") && gameBoard[r][c] == '#') { // Flag
        // send to server

    } else if ((action == "r" || action == "R") && gameBoard[r][c] == '#') { // Reveal
        // send to server
    } else if ((action == "u" || action == "U") && gameBoard[r][c] == 'F') { // unflag
    } else { // was Q or q
        ;
    }
    // only update board if affirmative response from server? (another gamestate.json file)
}