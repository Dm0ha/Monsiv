import board
import sys
import ctypes
import random
import numpy as np
import datetime
from ctypes import *

def get_move(state):
    pos = board.event_listener(state)
    if pos == None: # Game window was closed
        sys.exit()
    return pos
    
def move_piece(move):
    x, y, x1, y1 = move
    to_piece = state[y1][x1]
    from_piece = state[y][x]
    state[y1][x1] = from_piece
    state[y][x] = to_piece
    if to_piece.isupper() != from_piece.isupper():
        state[y][x] = ' '


game = CDLL('game.so') 
game.update_game.restype = ctypes.POINTER(ctypes.c_int32)
game.main()
state = [['B', 'R', 'K', 'B', 'R'], 
         [' ', ' ', ' ', ' ', ' '], 
         [' ', ' ', ' ', ' ', ' '], 
         [' ', ' ', ' ', ' ', ' '], 
         ['r', 'b', 'k', 'r', 'b']]


# ---- PLAY WHITE ----
while not game.is_game_over():
    board.display_board(state)
    player_move = get_move(state)
    if not game.check_valid_move(*player_move, 0, 0):
        continue
    move_piece(player_move)

    board.display_board(state)
    computer_move_arr = game.update_game(*player_move)
    if computer_move_arr == None:
        break
    computer_move = [computer_move_arr[i] for i in range(4)]
    game.free_arr(computer_move_arr)
    move_piece(computer_move)

print("Game over!")