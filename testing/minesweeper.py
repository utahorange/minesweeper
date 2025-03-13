# minesweeper

# make sure color module is imported
import importlib.util
import sys

name = 'termcolor'

if name in sys.modules:
    print(f"{name!r} already in sys.modules")
elif (spec := importlib.util.find_spec(name)) is not None:
    # If you choose to perform the actual import ...
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
else:
    print(f"can't find the {name!r} module")
    exit()

import random, re, os
from termcolor import colored

def main():
    print("Welcome to Minesweeper")
    
    global game_board
    global real_board # real board is a boolean 2D array, true for bomb
    global BOARD_SIZE; BOARD_SIZE = 10 

    game_board = [["#" for x in range(0,BOARD_SIZE)] for y in range(0,BOARD_SIZE)]
    real_board = [[False for x in range(0,BOARD_SIZE)] for y in range(0,BOARD_SIZE)]
    display_board(game_board)
    # F is marked/flag
    # space is nothing
    # 1-8 are the corresponding numbers
    
    global FLAGS; FLAGS = 13 # number of bombs
    global bomb; bomb = False
    coord_pattern = re.compile("^[0-9],[0-9]$")
    action_pattern = re.compile("^[F,R,U,f,r,u]$")
    action = ""
    flags_missing = FLAGS

    generate_board() # generate board, making sure not to generate a bomb on the square user chooses

    display_board(game_board)
    while(not (bomb or flags_missing==0)):
        next_coords = ""
        action = ""    
        while(not coord_pattern.match(next_coords)):
            next_coords = input("Input row,col: ")
        while(not action_pattern.match(action)):
            action = input("[F]lag, [R]eveal, or [U]nflag: ")
        
        coords = (int(next_coords.split(",")[0]),int(next_coords.split(",")[1]))
        if(game_board[coords[0]][coords[1]] == 'F' or game_board[coords[0]][coords[1]]=='#'):
            if(action.lower()=="f"):
                game_board[coords[0]][coords[1]] = colored('F','red')
                flags_missing -=1
            elif(action.lower()=="r"):
                reveal(coords) 
            elif(action.lower()=="u"):
                game_board[coords[0]][coords[1]] = "#"
                flags_missing +=1
            display_board(game_board)
    game_over()

def reveal(coords):
    if(real_board[coords[0]][coords[1]]):
        global bomb
        bomb =True
    else:    
        t = get_num_bombs(coords)
        if(t==0):
            game_board[coords[0]][coords[1]] = " "
            # cascade stuff
            for i in range(-1,2):
                if(coords[0]+i>=0 and coords[0]+i<BOARD_SIZE):
                    for j in range(-1,2):
                        if(coords[1]+j>=0 and coords[1]+j<BOARD_SIZE 
                        and game_board[coords[0]+i][coords[1]+j] == "#" and not(i==0 and j==0)):
                            reveal((coords[0]+i,coords[1]+j))
        else:
            game_board[coords[0]][coords[1]] = colored(t,'green')
    
def generate_board(): # the first move of the game
    # each 10x10 board has 13 bombs, makes sure start_coords is not occupied
    coord_pattern = re.compile("^[0-9],[0-9]$")
    start_coords = ""
    while(not coord_pattern.match(start_coords)):
        start_coords = input("Input row,col: ")
    start_coords = (int(start_coords.split(",")[0]),int(start_coords.split(",")[1]))
    
    bombs = set()

    # don't add bombs in 3x3 grid around where player first selects
    temp_coords = []
    for i in range(-1,2):
        if(start_coords[0]+i>=0 and start_coords[0]+i<BOARD_SIZE):
            for j in range(-1,2):
                if(start_coords[1]+j>=0 and start_coords[1]+j<BOARD_SIZE and not(i==0 and j==0)):
                    temp_coords.append((start_coords[0]+i,start_coords[1]+j))

    # actually add bombs
    while(len(bombs)<=FLAGS):
        temp = (random.randint(0,9),random.randint(0,9))
        while(temp==start_coords or temp in temp_coords): # make sure its not the starting coord or else it'd be annoying for user
            temp = (random.randint(0,9),random.randint(0,9))
        bombs.add(temp)
    for c in bombs:
        real_board[c[0]][c[1]] = True

    reveal(start_coords)

def get_num_bombs(coords): # get number of bombs around one tile
    count=0
    for i in range(-1,2):
        if(coords[0]+i>=0 and coords[0]+i<BOARD_SIZE):
            for j in range(-1,2):
                if(coords[1]+j>=0 and coords[1]+j<BOARD_SIZE and not(i==0 and j==0) and real_board[coords[0]+i][coords[1]+j]):
                    count+=1
    return count

def display_board(b):
    print("\n  ",end="")
    for i in range(BOARD_SIZE):
        print(str(i)+" ",end="")
    print("")
    for row in range(len(b)):
        print(str(row)+" ",end="")
        for col in range(len(b[0])):
            print(str(b[row][col])+" ", end="")
        print("")

def game_over(): # check if won/lost
    if(bomb):
        #os.system('cls' if os.name == 'nt' else 'clear')
        print(colored("the minefield exploded...",'blue'))
        exit()
    else:
        #os.system('cls' if os.name == 'nt' else 'clear')
        print(colored("and so you survive another day...",'blue'))
        exit()

if __name__ == "__main__":
    main()