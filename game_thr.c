// C:\MinGW\bin\gcc.exe

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <pthread.h>
#include "uthash.h"
#include "tt.h"

typedef struct {
    int from_x, from_y;
    int to_x, to_y;
    int minimax_score;
    int minimax_depth;
} Move;

typedef struct {
    Move arr[24];
    int count;
} MoveList;

typedef struct {
    int score;
    int depth;
} MinimaxScore;

typedef struct {
    Move move;
    char board[5][5];
    int depth;
    int turn;
    int alpha;
    int beta;
    int score;
    int thread;
} ThreadArgs;

typedef struct {
    MinimaxScore score;
    Move move;
} ThreadResult;

pthread_mutex_t lock;

void print_board(char board[5][5]);
bool is_valid_move(Move move, char board[5][5], int turn);
void move_piece(Move move, char board[5][5]);
bool is_game_over();
MoveList get_all_moves(char board[5][5], int turn);
Move minimax_root(char board[5][5], int depth, int turn);

int POWER = 2;
int MOV_MULT = 3;
int R_VAL = 20;
int B_VAL = 20;

int wrong = 0;
int total = 0;

int TEMP = 0;

int turn = 0; // 0 for player 1, 1 for player 2
int score = 0;
int depth = 12;
long minimax_calls = 0;
char board[5][5] = {{'B', 'R', 'K', 'B', 'R'},
                    {' ', ' ', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', ' '},
                    {' ', ' ', ' ', ' ', ' '},
                    {'r', 'b', 'k', 'r', 'b'}};
TranspositionTable tt;
int zobristTable[5][5][6] = {0};
int max_threads = 0;

int* update_game(int from_x, int from_y, int to_x, int to_y, int random, int temp_depth) {
    if (temp_depth > 0) {
        depth = temp_depth;
    }
    minimax_calls = 0;
    int* arr = malloc(4 * sizeof(int));
    Move move = {from_x, from_y, to_x, to_y};
    if (from_x != -1 || from_y != -1 || to_x != -1 || to_y != -1) {
        if (is_valid_move(move, board, turn)) {
            move_piece(move, board);
            turn = !turn;
        } else {
            for (int i = 0; i < 4; i++) {
                arr[i] = -1;
            }
            return arr;
        }
    }
    if (is_game_over()) {
        return arr;
    }
    // 20% chance the move is random
    if (random > 0) {
        MoveList moves = get_all_moves(board, turn);
        int rand_move = random % moves.count;
        move = moves.arr[rand_move];
        move_piece(move, board);
        turn = !turn;
        arr[0] = move.from_x;
        arr[1] = move.from_y;
        arr[2] = move.to_x;
        arr[3] = move.to_y;
        return arr;
    }
    move = minimax_root(board, depth, turn);
    move_piece(move, board);
    setlocale(LC_NUMERIC, "");
    minimax_calls = 0;
    turn = !turn;
    arr[0] = move.from_x;
    arr[1] = move.from_y;
    arr[2] = move.to_x;
    arr[3] = move.to_y;
    return arr;
}

void reset_game() {
    turn = 0;
    score = 0;
    minimax_calls = 0;
    char new_board[5][5] = {{'B', 'R', 'K', 'B', 'R'},
                            {' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' '},
                            {'r', 'b', 'k', 'r', 'b'}};
    memcpy(board, new_board, sizeof(board));
}

bool check_valid_move(int from_x, int from_y, int to_x, int to_y) {
    Move move = {from_x, from_y, to_x, to_y};
    return is_valid_move(move, board, turn);
}

void free_arr(int* arr) { // for python
    free(arr);
}

void print_board(char board[5][5]) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            char piece = board[i][j];
            if (piece == ' ') {
                piece = '.';
            }
            printf("%c ", piece);
        }
        printf("\n");
    }
}

bool is_valid_move(Move move, char board[5][5], int turn) {
    char piece = board[move.from_y][move.from_x];
    if ((turn == 0 && isupper(piece)) || (turn == 1 && islower(piece))) {
        return false;
    }

    int dx = move.to_x - move.from_x;
    int dy = move.to_y - move.from_y;
    piece = toupper(piece);

    switch (piece) {
        case 'R':
            return (dx == 0 && abs(dy) == 1) || (abs(dx) == 1 && dy == 0);
        case 'B':
            return abs(dx) == 1 && abs(dy) == 1;
        case 'K':
            return (abs(dx) == 1 && abs(dy) <= 1) || (abs(dx) <= 1 && abs(dy) == 1);
        default:
            return false;
    }
}

int hypo_move_piece(Move move, char board[5][5], int score) {
    char piece = board[move.from_y][move.from_x];
    char to_piece = board[move.to_y][move.to_x];
    int player = isupper(piece) ? 1 : 0;

    // Update the score based on the direction of movement
    int movement_score = 0;
    int to_y = player == 0 ? 4 - move.to_y : move.to_y;
    int from_y = player == 0 ? 4 - move.from_y : move.from_y;
    for (int i = 1; i < POWER; i++) {
        to_y *= to_y;
        from_y *= from_y;
    }
    movement_score = (to_y - from_y) * (player == 0 ? MOV_MULT : MOV_MULT * -1);

    score += movement_score;

    // Move the piece
    board[move.from_y][move.from_x] = ' ';
    board[move.to_y][move.to_x] = piece;

    // Check for swap
    if (to_piece != ' ' && isupper(to_piece) == isupper(piece)) {
        // Undo the score change
        score -= movement_score;
        board[move.from_y][move.from_x] = to_piece;
    }

    // Check if the destination square contains a piece of the opposite color
    if (to_piece != ' ' && isupper(to_piece) != isupper(piece)) {
        // Update the score based on the piece taken
        if (player == 0) {
            to_y = move.to_y;
            for (int i = 1; i < POWER; i++) {
                to_y *= to_y;
            }
            score += to_y * MOV_MULT;
            switch (to_piece) {
                case 'K':
                    score = 1000;
                    break;
                case 'R':
                    score += R_VAL;
                    break;
                case 'B':
                    score += B_VAL;
                    break;
            }
        } else {
            to_y = 4 - move.to_y;
            for (int i = 1; i < POWER; i++) {
                to_y *= to_y;
            }
            score -= to_y * MOV_MULT;
            switch (to_piece) {
                case 'k':
                    score = -1000;
                    break;
                case 'r':
                    score -= R_VAL;
                    break;
                case 'b':
                    score -= B_VAL;
                    break;
            }
        }
    }

    // Made it to the end of the board
    if (player == 0 && move.to_y == 0) {
        score = 1000;
    } else if (player == 1 && move.to_y == 4) {
        score = -1000;
    }
    return score;
}

void move_piece(Move move, char board[5][5]) {
    score = hypo_move_piece(move, board, score);
}

MoveList get_all_moves(char board[5][5], int turn) {
    MoveList moves;
    moves.count = 0;

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            if ((turn == 0 && islower(board[y][x])) || (turn == 1 && isupper(board[y][x]))) {
                for (int to_y = 0; to_y < 5; to_y++) {
                    for (int to_x = 0; to_x < 5; to_x++) {
                        Move move = {x, y, to_x, to_y};
                        if (is_valid_move(move, board, turn)) {
                            moves.arr[moves.count++] = move;
                        }
                    }
                }
            }
        }
    }

    return moves;
}

bool is_game_over() {
    return (score >= 500 || score <= -500);
}

bool hypo_is_game_over(int score) {
    return (score >= 500 || score <= -500);
}

MoveList greedy_moves(char board[5][5], int turn, int curr_depth) {
    MoveList valid_moves = get_all_moves(board, turn);
    MoveList greedy_moves_1 = { .count = 0 };
    MoveList greedy_moves_2 = { .count = 0 };

    if (curr_depth > depth - 6) {
        return valid_moves;
    }
    for (int i = 0; i < valid_moves.count; i++) {
        Move move = valid_moves.arr[i];
        char original_destination = board[move.to_y][move.to_x];
        int hypo_score = hypo_move_piece(move, board, score);
        if (turn == 0 && hypo_score > score) {
            greedy_moves_1.arr[greedy_moves_1.count++] = move;
        }
        else if (turn == 1 && hypo_score < score) {
            greedy_moves_1.arr[greedy_moves_1.count++] = move;
        }
        hypo_move_piece((Move){move.to_x, move.to_y, move.from_x, move.from_y}, board, hypo_score); // Undo the move
        board[move.to_y][move.to_x] = original_destination;
    }
    return greedy_moves_1;
}

int compare_moves(const void *a, const void *b) {
    Move *moveA = (Move *)a;
    Move *moveB = (Move *)b;
    return moveB->minimax_score - moveA->minimax_score;
}

void init_zobrist() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 6; k++) {
                zobristTable[i][j][k] = rand();
            }
        }
    }
}

uint64_t computeHash(char board[5][5]) {
    uint64_t hash = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (board[i][j] != ' ') {
                int piece;
                switch (board[i][j]) {
                    case 'K':
                        piece = 0;
                        break;
                    case 'k':
                        piece = 1;
                        break;
                    case 'R':
                        piece = 2;
                        break;
                    case 'r':
                        piece = 3;
                        break;
                    case 'B':
                        piece = 4;
                        break;
                    case 'b':
                        piece = 5;
                        break;
                }
                hash ^= zobristTable[i][j][piece];
            }
        }
    }
    return hash;
}

void add_record(uint64_t key, MinimaxScore score, int start_depth, int thread) {
    bool found;
    TTEntry* entry = TranspositionTable_probe(&tt, key, &found);
    TTEntry_save(entry, key, score.score, true, 1, start_depth, 0, 0, &tt);
}

TTEntry* get_record(uint64_t hash, int thread) {
    bool found;
    TTEntry* entry = TranspositionTable_probe(&tt, hash, &found);
    if (found) {
        return entry;
    } else {
        return NULL;
    }
}

MinimaxScore minimax(char board[5][5], int depth, int alpha, int beta, int turn, int score, int thread) {
    minimax_calls++;
    if (depth == 0 || hypo_is_game_over(score)) {
        return (MinimaxScore) {score, depth};
    }

    MoveList valid_moves = get_all_moves(board, turn);
    MoveList sorted_moves = { .count = 0 };
    for (int i = 0; i < valid_moves.count; i++) {
        Move move = valid_moves.arr[i];
        int hypo_score;
        uint64_t hash = computeHash(board);
        TTEntry* r = get_record(hash, thread);
        if (r == NULL) {
            char original_destination = board[move.to_y][move.to_x];
            hypo_score = hypo_move_piece(move, board, score);
            hypo_move_piece((Move){move.to_x, move.to_y, move.from_x, move.from_y}, board, hypo_score);
            board[move.to_y][move.to_x] = original_destination;
        } else {
            hypo_score = r->value16;
        }
        sorted_moves.arr[sorted_moves.count++] = (Move) {move.from_x, move.from_y, move.to_x, move.to_y, hypo_score};
    }
    qsort(sorted_moves.arr, sorted_moves.count, sizeof(Move), compare_moves);

    MinimaxScore best_score = {(turn == 0) ? -10000 : 10000, 10000};

    for (int i = 0; i < sorted_moves.count; i++) {
        Move move = sorted_moves.arr[i];
        char original_destination = board[move.to_y][move.to_x];
        int hypo_score = hypo_move_piece(move, board, score);
        MinimaxScore deep_score;
        uint64_t hash = computeHash(board);
        TTEntry* r = get_record(hash, thread);
        if (r != NULL && r->depth8 >= depth && ((r->depth8 ^ depth) & 1) == 0) {
            deep_score = (MinimaxScore) {r->value16, r->depth8};
        } else {
            deep_score = minimax(board, depth - 1, alpha, beta, !turn, hypo_score, thread);
            add_record(hash, deep_score, depth, thread);
        }
        hypo_move_piece((Move){move.to_x, move.to_y, move.from_x, move.from_y}, board, hypo_score);
        board[move.to_y][move.to_x] = original_destination;

        if (turn == 0) {
            if (deep_score.score > best_score.score || (deep_score.score == best_score.score && deep_score.score > 0 && deep_score.depth > best_score.depth) || (deep_score.score == best_score.score && deep_score.score < 0 && deep_score.depth < best_score.depth)) {
                best_score = deep_score;
            }
            alpha = (alpha > best_score.score) ? alpha : best_score.score;
        } else {
            if (deep_score.score < best_score.score || (deep_score.score == best_score.score && deep_score.score < 0 && deep_score.depth > best_score.depth) || (deep_score.score == best_score.score && deep_score.score > 0 && deep_score.depth < best_score.depth)) {
                best_score = deep_score;
            }
            beta = (beta < best_score.score) ? beta : best_score.score;
        }
        if (beta < alpha || (best_score.score < 500 && best_score.score > -500 && beta == alpha)) {
            break; // Alpha-beta pruning
        }
    }
    return best_score;
}

void* minimax_worker(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*) args;
    int hypo_score = hypo_move_piece(threadArgs->move, threadArgs->board, threadArgs->score);
    MinimaxScore deep_score = minimax(threadArgs->board, threadArgs->depth, threadArgs->alpha, threadArgs->beta, !(threadArgs->turn), hypo_score, threadArgs->thread);
    ThreadResult* result = malloc(sizeof(ThreadResult));
    result->score = deep_score;
    result->move = threadArgs->move;
    return result;
}

Move minimax_root(char board[5][5], int depth, int turn) {
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    MoveList valid_moves = get_all_moves(board, turn);
    pthread_t threads[valid_moves.count];
    ThreadArgs threadArgs[valid_moves.count];

    MinimaxScore best_score = {(turn == 0) ? -10000 : 10000, 10000};
    Move best_move = {0, 0, 0, 0};

    if (valid_moves.count > max_threads) {
        max_threads = valid_moves.count;
    }
    for (int d = 2; d <= depth; d++) {
        minimax_calls = 0;
        // Create threads
        for (int i = 0; i < valid_moves.count; i++) {
            threadArgs[i].move = valid_moves.arr[i];
            memcpy(threadArgs[i].board, board, sizeof(char) * 5 * 5);
            threadArgs[i].depth = d - 1;
            threadArgs[i].turn = turn;
            threadArgs[i].alpha = -10000;
            threadArgs[i].beta = 10000;
            threadArgs[i].score = score;
            threadArgs[i].thread = i;
            pthread_create(&threads[i], NULL, minimax_worker, &threadArgs[i]);
        }

        // Join threads and get results
        for (int i = 0; i < valid_moves.count; i++) {
            printf("%d\n", i);
            ThreadResult* result;
            pthread_join(threads[i], (void**)&result);
            MinimaxScore deep_score = result->score;

            // Only update the best move if it's the highest depth
            if (d == depth) {
                if (turn == 0) {
                    if (deep_score.score > best_score.score || (deep_score.score == best_score.score && deep_score.score > 0 && deep_score.depth > best_score.depth) || (deep_score.score == best_score.score && deep_score.score < 0 && deep_score.depth < best_score.depth)) {
                        best_score = deep_score;
                        best_move = result->move;
                    }
                } else {
                    if (deep_score.score < best_score.score || (deep_score.score == best_score.score && deep_score.score < 0 && deep_score.depth > best_score.depth) || (deep_score.score == best_score.score && deep_score.score > 0 && deep_score.depth < best_score.depth)) {
                        best_score = deep_score;
                        best_move = result->move;
                    }
                }
            }
            free(result);
        }
        printf("Depth %d\n", d);
        printf("Minimax calls: %'ld\n", minimax_calls);
        printf("Score: %d\n", best_score.score);
        printf("Hashfull: %d\n", TranspositionTable_hashfull(&tt));
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Total CPU time: %f seconds\n", cpu_time_used);
    return best_move;
}


int main(int argc, char *argv[]) {
    pthread_mutex_init(&lock, NULL);
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    init_zobrist();
    tt.generation8 = 0;
    TranspositionTable_resize(&tt, 16);
    return 0;
}
