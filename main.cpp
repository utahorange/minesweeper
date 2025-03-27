#include <thread>
#include <chrono>
#include "minesweeper.h"

void gameThread() {
    Minesweeper game = Minesweeper();

    // prompt for game dimensions?
    
    std::cout << "Welcome to Minesweeper" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    game.setupBoard(); // board displayed with first move having been played
    
    while (!game.didBombGoOff() && game.getNumBombsFound()!=NUM_BOMBS){
        // TODO: ??? instead of below, should prob be "waitForMove"
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
}

int main() {
    // concurrency stuff, shuffling btwn server thread and game thread
    // all game stuff abstracted into Minesweeper class
    
    // prompt user to either [S]tart or [J]oin a game
    // run serverThread or clientThread
    // if client, no need to run gameThread/multi-thread

    gameThread();
    // std::thread server(serverThread);
}
