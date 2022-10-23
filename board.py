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
    1: pygame.image.load("img/w_king.png"), 2: pygame.image.load("img/w_bishop.png"), 3: pygame.image.load("img/w_rook.png"), 
    4: pygame.image.load("img/b_king.png"), 5: pygame.image.load("img/b_bishop.png"), 6: pygame.image.load("img/b_rook.png")
}

pygame.display.set_caption('Monsiv')
pygame.display.set_icon(PIECES.get(1))

def draw_board():
    do_white = True
    for i in range(ROWS):
        for j in range(ROWS):
            rect = pygame.Rect(i*BOX_WIDTH, j*BOX_WIDTH, BOX_WIDTH, BOX_WIDTH)
            if not do_white:
                pygame.draw.rect(SCREEN, GREY, rect)
            pygame.draw.rect(SCREEN, BLACK, rect, 1)
            do_white = not do_white

def add_pieces(board):
    for i in range(len(board)):
        for j in range(len(board[i])):
            if board[i][j] != 0:
                img = PIECES.get(board[i][j])
                SCREEN.blit(img, (j*BOX_WIDTH, i*BOX_WIDTH))

def get_square(pos):
    x, y = pos
    return (x // BOX_WIDTH, y // BOX_WIDTH)

def display_board(board):
    SCREEN.fill(WHITE)
    draw_board()
    add_pieces(board)
    pygame.display.flip()

def highlight_squares(squares, board):
    SCREEN.fill(WHITE)
    draw_board()
    for pos in squares:
        x, y = pos
        rect = pygame.Rect(x*BOX_WIDTH, y*BOX_WIDTH, BOX_WIDTH, BOX_WIDTH)
        pygame.draw.rect(SCREEN, GREEN, rect)
        pygame.draw.rect(SCREEN, BLACK, rect, 1)
    add_pieces(board)
    pygame.display.flip()

def print_message(msg, x, y):
    font = pygame.font.SysFont('calabri.ttf', 30)
    SCREEN.blit(font.render(msg, True, BLACK, WHITE), (x, y))
    pygame.display.flip()

def event_listener():
    while True:
        pygame.time.delay(50)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return None
            elif event.type == pygame.MOUSEBUTTONDOWN:
                square = get_square(pygame.mouse.get_pos())
                return(square)
        pygame.display.flip()