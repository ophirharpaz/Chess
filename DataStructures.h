/*
 * DataStructures.h
 *
 *  Created on: Jul 25, 2015
 *      Author: linesh
 */

#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

# include "Chess.h"
# include <time.h>

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
} List;

typedef struct Config {
	int DEPTH;
	int MODE;
	int USER_COLOR;
	int END_GAME;
	int TURN;
	int BEST;
	char board [BOARD_SIZE][BOARD_SIZE];
} Config;

typedef Config* PtrConfig;

/* --------------------- CONFIG STRUCTURE FUNCTIONS --------------------- */
Config init_config();
Config create_new_config(PtrConfig c, int turn, char board[BOARD_SIZE][BOARD_SIZE], int depth);
void print_config(PtrConfig c);

/* --------------------- BOARD FUNCTIONS --------------------- */
void init_board(PtrConfig c);
void print_line();
void print_board(PtrConfig c);
int clear_board(PtrConfig c);
int copy_board(char board[BOARD_SIZE][BOARD_SIZE],char next_board[BOARD_SIZE][BOARD_SIZE]);

/* --------------------- LOCATION STRUCTURE FUNCTIONS --------------------- */
Location create_location(int i, int j);
Location parse_location(char* loc_str);
int print_location(Location loc);
int get_location_moves(PtrConfig c,List * legal_moves, Location loc);
int compare_locations(Location l1, Location l2);

/* --------------------- MOVE STRUCTURE FUNCTIONS --------------------- */
Move create_move(Location src, Location dst, char type, int with_board, Config* c, int threat);
int print_move(Move move);
int get_best_moves(PtrConfig c,List * legal_moves, int depth);
Move best_move(List* moves);
int compare_move(Move this, Move other);
int find_move(List * list, Move move);
int insert_move(List* list, Move move);
int create_board_move(PtrConfig c,Move * move);
int get_score(PtrConfig c, int depth,List * legal_moves,Move move);
int make_legal_move(char board[BOARD_SIZE][BOARD_SIZE], Move move, int turn);

/* --------------------- LIST STRUCTURE FUNCTIONS --------------------- */
List* init_list();
void print_list_rec(PtrNode head);
void print_list(List * lst);
int free_list(List* list);
Node* create_node(Move move);
int print_node(Node* node);
void pop_data(List* list);

#endif /* DATASTRUCTURES_H_ */

