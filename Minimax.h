#include "DataStructures.h"

#ifndef MINIMAX_H_
#define MINIMAX_H_


int score(PtrConfig c,int turn);
int piece_score(char piece, int turn);
int rec_alphabeta(Config c, List* moves, int depth,int alpha, int beta, int max_player, int score_turn, int* board_num);
int alphabeta(Config c, List* moves);
int max(int a, int b);
int min(int a, int b);
#endif
