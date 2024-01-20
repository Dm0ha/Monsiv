import pygame

pygame.init()

SCREEN_WIDTH = 300
ROWS = 5
BOX_WIDTH = SCREEN_WIDTH // ROWS
BLACK = (0, 0, 0)
GREY = (100, 100, 100)
WHITE = (255, 255, 255)
GREEN = (152, 255, 152)
SCREEN = pygame.display.set_mode([SCREEN_WIDTH, SCREEN_WIDTH])
PIECES = {
    'k': pygame.image.load("img/w_king.png"), 'b': pygame.image.load("img/w_bishop.png"), 'r': pygame.image.load("img/w_rook.png"), 
    'K': pygame.image.load("img/b_king.png"), 'B': pygame.image.load("img/b_bishop.png"), 'R': pygame.image.load("img/b_rook.png")
}

pygame.display.set_caption('Monsiv')
pygame.display.set_icon(PIECES.get('k'))

def draw_board():
    do_white = True
    for i in range(ROWS):
        for j in range(ROWS):
            rect = pygame.Rect(i*BOX_WIDTH, j*BOX_WIDTH, BOX_WIDTH, BOX_WIDTH)
            if not do_white:
                pygame.draw.rect(SCREEN, GREY, rect)
            pygame.draw.rect(SCREEN, BLACK, rect, 1)
            do_white = not do_white

def add_pieces(board, excluded):
    for i in range(len(board)):
        for j in range(len(board[i])):
            if board[i][j] != ' ' and (j, i) != excluded:
                img = PIECES.get(board[i][j])
                SCREEN.blit(img, (j*BOX_WIDTH, i*BOX_WIDTH))

def get_square(pos):
    x, y = pos
    return (x // BOX_WIDTH, y // BOX_WIDTH)

def display_board(board, excluded = None):
    SCREEN.fill(WHITE)
    draw_board()
    add_pieces(board, excluded)
    pygame.event.get()
    pygame.display.flip()

def print_message(msg, x, y):
    font = pygame.font.SysFont('calabri.ttf', 30)
    SCREEN.blit(font.render(msg, True, BLACK, WHITE), (x, y))
    pygame.display.flip()

def draw_dragged_piece(piece, pos):
    x, y = pos
    x -= BOX_WIDTH // 2
    y -= BOX_WIDTH // 2
    SCREEN.blit(piece, (x, y))

def event_listener(board):
    piece = None
    start_pos = None
    excluded = None

    while True:
        pygame.time.delay(50)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return None
            elif event.type == pygame.MOUSEBUTTONDOWN:
                square = get_square(pygame.mouse.get_pos())
                piece = board[square[1]][square[0]]
                if piece != ' ':
                    start_pos = square
                    excluded = square
            elif event.type == pygame.MOUSEMOTION and start_pos is not None:
                display_board(board, excluded)
                draw_dragged_piece(PIECES[piece], pygame.mouse.get_pos())
                pygame.display.flip()
            elif event.type == pygame.MOUSEBUTTONUP and start_pos is not None:
                new_square = get_square(pygame.mouse.get_pos())
                return [start_pos[0], start_pos[1], new_square[0], new_square[1]]
                
        pygame.display.flip()