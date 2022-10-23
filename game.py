import sys
import random as rand
import board as vis

class Move():

    def __init__(self, x0, y0, x1, y1, player, raw):
        if raw:
            self.x0 = x0
            self.y0 = y0
            self.x1 = x1
            self.y1 = y1
        else:
            self.x0 = x0 - 1
            self.y0 = 5 - y0
            self.x1 = x1 - 1
            self.y1 = 5 - y1
        self.player = player

class State():

    def __init__(self, board = None, states = None, p0 = True, score = 0):
        if board == None:
            self.board = [  
                [5, 6, 4, 5, 6],
                [0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0],
                [3, 2, 1, 3, 2],
            ]
        else:
            self.board = board
        self.p0 = p0
        self.score = score
        if states == None:
                self.states = []
        else:    
            self.states = states
    
    def getAllMoves(self, player):
        moves = []
        for i in range(len(self.board)):
            for j in range(len(self.board[0])):
                self.movesFromPos(j, i, player, moves)
        return moves

    def movesFromPos(self, x, y, player, moves = None):
        if moves == None:
            moves = []
        piece = self.board[y][x]
        if (player == 0 and piece == 1) or (player == 1 and piece == 4):
            if y + 1 <= 4 and x + 1 <= 4:
                moves.append(Move(x,y,x+1,y+1, player, True))
            if y + 1 <= 4 and x - 1 >= 0:
                moves.append(Move(x,y,x-1,y+1, player, True))
            if y - 1 >= 0 and x + 1 <= 4:
                moves.append(Move(x,y,x+1,y-1, player, True))
            if y - 1 >= 0 and x - 1 >= 0:
                moves.append(Move(x,y,x-1,y-1, player, True))
            if y + 1 <= 4:
                moves.append(Move(x,y,x,y+1, player, True))
            if y - 1 >= 0:
                moves.append(Move(x,y,x,y-1, player, True))
            if x + 1 <= 4:
                moves.append(Move(x,y,x+1,y, player, True))    
            if x - 1 >= 0:
                moves.append(Move(x,y,x-1,y, player, True))
        if (player == 0 and piece == 2) or (player == 1 and piece == 5):
            if y + 1 <= 4 and x + 1 <= 4:
                moves.append(Move(x,y,x+1,y+1, player, True))
            if y + 1 <= 4 and x - 1 >= 0:
                moves.append(Move(x,y,x-1,y+1, player, True))
            if y - 1 >= 0 and x + 1 <= 4:
                moves.append(Move(x,y,x+1,y-1, player, True))
            if y - 1 >= 0 and x - 1 >= 0:
                moves.append(Move(x,y,x-1,y-1, player, True))
        if (player == 0 and piece == 3) or (player == 1 and piece == 6):
            if y + 1 <= 4:
                moves.append(Move(x,y,x,y+1, player, True))
            if y - 1 >= 0:
                moves.append(Move(x,y,x,y-1, player, True))
            if x + 1 <= 4:
                moves.append(Move(x,y,x+1,y, player, True))    
            if x - 1 >= 0:
                moves.append(Move(x,y,x-1,y, player, True))
        return moves

    def judgeState(self):
        return self.score

    def incrementStateScore(self, move, repetition = False):
        if self.board[move.y0][move.x0] <= 3 and not repetition:
            if self.board[move.y1][move.x1] >= 4:
                self.score += 20
            self.score += (4 - move.y1)**2 - (4 - move.y0)**2
            if move.y1 == 0 or self.board[move.y1][move.x1] == 4:
                self.score = 1000
        if self.board[move.y0][move.x0] >= 4 and not repetition:
            if self.board[move.y1][move.x1] <= 3 and self.board[move.y1][move.x1] > 0:
                self.score -= 20
            self.score -= (move.y1)**2 - (move.y0)**2
            if move.y1 == 4 or self.board[move.y1][move.x1] == 1:
                self.score = -1000
        if repetition and self.board in self.states:
            if self.p0:
                self.score = 1000 
            else:
                self.score = -1000

    def copy(self):
        new_state = State([row[:] for row in self.board], self.states.copy(), self.p0, self.score)
        return new_state
        
    def executeMove(self, move):
        if self.validMove(move):
            self.states.append([row[:] for row in self.board])
            self.incrementStateScore(move)
            piece = self.board[move.y0][move.x0]
            landing_piece = self.board[move.y1][move.x1]
            if move.player == 0 and landing_piece > 3:
                landing_piece = 0
            if move.player == 1 and landing_piece < 4:
                landing_piece = 0
            self.board[move.y0][move.x0] = landing_piece
            self.board[move.y1][move.x1] = piece
            self.p0 = not self.p0
            self.incrementStateScore(move, True)
            return True
        return False

    def validMove(self, move):
        if (move.x0 < 0 or move.x0 > 4) or (move.x1 < 0 or move.x1 > 4) or (move.y0 < 0 or move.y0 > 4) or (move.y1 < 0 or move.y1 > 4):
            return False
        piece = self.board[move.y0][move.x0]
        if piece == 0:
            return False
        if move.x0 == move.x1 and move.y0 == move.y1:
            return False
        if move.player == 0:
            if piece > 3:
                return False
        if move.player == 1:
            if piece < 4:
                return False
        if piece == 1 or piece == 4:
            if abs(move.x0 - move.x1) > 1 or abs(move.y0 - move.y1) > 1:
                return False
            return True   
        if piece == 2 or piece == 5:
            if abs(move.x0 - move.x1) > 1 or abs(move.y0 - move.y1) > 1:
                return False
            if move.x0 == move.x1 or move.y0 == move.y1:
                return False
            return True     
        if piece == 3 or piece == 6:
            if abs(move.x0 - move.x1) > 1 or abs(move.y0 - move.y1) > 1:
                return False
            if move.x0 != move.x1 and move.y0 != move.y1:
                return False
            return True
    
    def getWinner(self):
        if self.score >= 500:
            return 0
        if self.score <= -500:
            return 1
        else:
            return -1

    def gameOver(self):
        if self.getWinner() == -1:
            return False
        return True

    def __str__(self):
        string = ""
        for row in self.board:
            for ele in row:
                string += str(ele) + " "
            string += "\n"
        return string
                

def miniMax(state, player, depth):
    temp_state = state.copy()
    return miniMaxHelper(temp_state, player, depth, True, None)

def miniMaxHelper(state, player, depth, root, last_best_score):
    if depth <= 0 or state.gameOver():
        return (state.judgeState(), depth)
    best_score = None
    best_move = None
    all_moves = state.getAllMoves(player)

    if depth >= 2:
        ordered_moves = {}
        for move in all_moves:
            temp_state = state.copy()
            temp_state.executeMove(move)
            score = miniMaxHelper(temp_state, 1 if player == 0 else 0, 0, False, best_score)[0]
            if score not in ordered_moves:
                ordered_moves[score] = []
            ordered_moves[score].append(move)
        all_moves = []
        sorted_keys = []
        if player == 0:
            sorted_keys = sorted(ordered_moves.keys())
            sorted_keys.reverse()
        else:
            sorted_keys = sorted(ordered_moves.keys())
        for score in sorted_keys:
            for move in ordered_moves[score]:
                all_moves.append(move)

    for move in all_moves:
        temp_state = state.copy()
        temp_state.executeMove(move)
        score = miniMaxHelper(temp_state, 1 if player == 0 else 0, depth - 1, False, best_score)
        if player == 0:
            if best_score == None or score[0] > best_score[0]:
                best_score = score
                best_move = move
            if score[0] == best_score[0]:
                if score[1] == best_score[1]:
                    if rand.randint(0, 1) <= 0.2:
                        best_score = score
                        best_move = move
                if (score[1] > best_score[1] and score[0] > 0) or (score[1] < best_score[1] and score[0] < 0):
                    best_score = score
                    best_move = move
            if last_best_score != None and best_score[0] > last_best_score[0]:
                break
        else:
            if best_score == None or score[0] < best_score[0]:
                best_score = score
                best_move = move
            if score[0] == best_score[0]:
                if score[1] == best_score[1]:
                    if rand.randint(0, 1) <= 0.2:
                        best_score = score
                        best_move = move
                if (score[1] > best_score[1] and score[0] < 0) or (score[1] < best_score[1] and score[0] > 0):
                    best_score = score
                    best_move = move
            if last_best_score != None and best_score[0] < last_best_score[0]:
                break
    if root:
        # print(best_score[0])
        # if (best_score[0] == 1000 or best_score[0] == -1000):
        #     print("Mate in " + str(depth - 1 - best_score[1]))
        return (best_move, best_score[0])
    else:
        return best_score
    
def get_click():
    pos = vis.event_listener()
    if pos == None: # Game window was closed
        sys.exit()
    return pos

def get_move(game, p0):
    loop = True
    while loop:
        vis.display_board(game.board)
        pos = get_click()
        x, y = pos
        moves = game.movesFromPos(x, y, 0 if p0 else 1)
        vis.highlight_squares([(move.x1, move.y1) for move in moves], game.board)

        if len(moves) > 0:
            pos = get_click()
            if pos == None:
                sys.exit()
            x1, y1 = pos
            move = Move(x, y, x1, y1, 0 if p0 else 1, True)
            if game.validMove(move):
                return move

def playGame(computer_depth):
    game = State()
    while not game.gameOver():
        if game.p0:
            move = get_move(game, game.p0)
            game.executeMove(move)
            vis.display_board(game.board)
        else:
            m = miniMax(game, 1, computer_depth)[0]
            game.executeMove(m)
    vis.display_board(game.board)
    vis.print_message("GAME OVER!", 85, 130)
    if game.getWinner() == 0:
        vis.print_message("Player 1 wins!", 81, 150)
    else:
        vis.print_message("Computer wins!", 72, 150)
    get_click()

while True:
    playGame(6)