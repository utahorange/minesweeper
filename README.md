# Multi-Threaded Minesweeper

## Installing and Playing the Game

To install the game and start it, just clone this repository and run `make all; ./Minesweeper` in this project's directory. This should start the game, at which point you'll be prompted to either start or join a game. Make moves on the board by inputting your coordinates and the action you want to take.

## Design

I implemented terminal colors with [ANSI color codes](https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal) which only works on some terminals.

// note


you need to unflag a square before you can reveal it