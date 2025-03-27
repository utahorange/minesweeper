# Multi-Threaded Minesweeper

## Gameplay

## Design

I implemented terminal colors with [ANSI color codes](https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal) which only works on some terminals.

The `json` file to represent a particular gameState looks like this

```json
{
    "numRows": 10,
    "numCols": 10,
    "gameBoard": [
        ["#", "#", "#", " ", "F"],
        [" ", "#", "#", " ", " "],
        ["F", " ", "#", "#", "#"],
        [" ", " ", " ", " ", " "],
        ["#", "#", "F", "#", "#"]
    ],
    "realBoard": [
        [false, true, false, false, false],
        [false, false, false, false, false],
        [true, false, false, true, false],
        [false, false, false, false, false],
        [false, true, true, false, false]
    ],
    "bombWentOff": 0,
    "numFlagsLeft": ???,
    "numBombsFound": 0
}
```



// note

we might have a thing where there's a 

make client
make server

defined in makefile

bc there should be a main func in client AND server

make either one based on what you're doing?

or put everything into server tbh

server should prompt you then to either start a game or join one


you need to unflag a square before you can reveal it