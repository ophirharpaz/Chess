/*
 * DataStructures.h
 *
 *  Created on: Jul 25, 2015
 *      Author: linesh
 */

#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

# include "Chess.h"


typedef struct Locations {
   int row;
   int col;
} Location;

typedef struct Moves {
	Location src;
	Location dst;
	char type; // c(astle), q/b/r/n (promote), o(rdinary)
	int score;
	int threat;
	char board[BOARD_SIZE][BOARD_SIZE];
} Move;

typedef struct Node {
	Move m;
	struct Node* next;
	} Node;

typedef struct Node* PtrNode;

typedef struct List{
	struct Node* first;
	int size;
	int sort;
} List;

typedef struct Config {
	int DEPTH;
	int MODE;
	int USER_COLOR;
	int END_GAME;
	int TURN;
	int CASTLE;
	int DIFFICULTY;
	char board [BOARD_SIZE][BOARD_SIZE];
} Config;

typedef Config* PtrConfig;

Location create_loc(int i, int j);
Location parse_location(char* loc_str);
int compare_locations(Location l1, Location l2);

void print_line();
void print_board(PtrConfig c);
void init_board(PtrConfig c);
int clear_board(PtrConfig c);
int copy_board(char board[BOARD_SIZE][BOARD_SIZE],char next_board[BOARD_SIZE][BOARD_SIZE]);
void print_board_board(char board[BOARD_SIZE][BOARD_SIZE]);


List* init_list(int sort);
Move best_move(List* moves);
int get_best_moves(PtrConfig c,List * legal_moves, int depth);
int get_score(PtrConfig c, int depth,List * legal_moves,Move move);
int get_location_moves(PtrConfig c,List * legal_moves, Location loc);

int free_list(List* list);
void print_list(List * lst);
void rec_print_list(PtrNode head);
void pop_data(List* list);

Config init_config();
Config create_new_config(PtrConfig c, int turn, char board[BOARD_SIZE][BOARD_SIZE], int depth);
void print_config(PtrConfig c);

Move create_move(Location src, Location dst, char type, int with_board, Config* c, int threat);
int create_board_move(PtrConfig c,Move * move);
int print_move(Move move);
int compare_move(Move this, Move other);
int find_move(List * list, Move move);
int compare_loc(Location this, Location other);
int insert_move(List* list, Move move);
int make_legal_move(char board[BOARD_SIZE][BOARD_SIZE], Move move, int turn);

#endif /* DATASTRUCTURES_H_ */

