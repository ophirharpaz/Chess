#ifndef CHESS_
#define CHESS_

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>

//* CONSIDER TO NOT USE *//
#define WHITE_P 'm'
#define WHITE_B 'b'
#define WHITE_N 'n'
#define WHITE_R 'r'
#define WHITE_Q 'q'
#define WHITE_K 'k'
#define BLACK_P 'M'
#define BLACK_B 'B'
#define BLACK_N 'N'
#define BLACK_R 'R'
#define BLACK_Q 'Q'
#define BLACK_K 'K'
//* UNTIL HERE *//

#define EMPTY ' '

#define BOARD_SIZE 8
#define BOARD(i,j) c->board[i][j]

#define WHITE 1
#define BLACK 0
#define TWO_PLAYERS 1
#define ONE_PLAYER 2

#define PATH_MAX 100

// board initialization
#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_GAME_MODE "Wrong game mode\n"
#define TWO_PLAYERS_GAME_MODE "Running game in 2 players mode\n"
#define PLAYER_VS_AI_GAME_MODE "Running game in player vs. AI mode\n"
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 4\n"
#define WRONG_FILE_NAME "Wrong file name\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_PIECE "The specified position does not contain your piece\n"
#define WRONG_BOARD_INITIALIZATION "Wrong board initialization\n"

#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
 
#define WRONG_ROOK_POSITION "Wrong position for a rook\n" 
#define ILLEGAL_CASTLING_MOVE "Illegal castling move\n"

#define TIE "The game ends in a tie\n"
#define CHECK "Check!\n"
 
#define perror_message(func_name) (fprintf(stderr, "Error: standard function %s has failed", func_name));
#define print_message(message) (printf("%s", message));

#endif

