# multi-player minesweeper

## what is this

I got bored one day and implemented Minesweeper in Python. That was 4 years ago. Having spent way too much time in C++ for my college courses, I figured I should get some use out of the knowledge and implement a networked Minesweeper using threads and C sockets for people to play with friends.

## installing and playing the game 

To install the game and start it, just clone this repository and run `make all; ./Minesweeper` in this project's directory. This should start the game, at which point you'll be prompted to either start or join a game by inputting your friend's IP. Make moves on the board by inputting your coordinates and the action you want to take.

// you need to unflag a square before you can reveal it

## design 

This project uses a client-server architecture :( because I could not figure out P2P networking. It also only works within a LAN because I bad at NAT traversal. 




I implemented terminal colors with [ANSI color codes](https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal) which only works on some terminals.

