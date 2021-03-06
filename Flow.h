#ifndef FLOW_H_
#define FLOW_H_

# include "Minimax.h"
# include "FileWriter.h"
# include "Gui.h"
# include "DataStructures.h"

int chess_main(int argc, char* argv[]);

/* --------------------- SETTINGS STATE FUNCTIONS --------------------- */

int settings_state(PtrConfig c);
int set_minimax_depth(char* p, PtrConfig c);
int set_by_pos(PtrConfig c, Location set_loc, int color,char type);
int remove_by_pos(PtrConfig c, Location rm_loc);
int count_pieces(PtrConfig c, char type);
int ok_to_start(Config * c);

/* ----------------------- GAME STATE FUNCTIONS ----------------------- */

int game_state(PtrConfig c);
int user_turn(Config* c, List *moves);
int computer_turn(Config* c,List *moves);
int player_wins(PtrConfig c);
int check(Config* c);
int king_is_taken(PtrConfig c, Move move);

/* ------------------------ LEGAL MOVES CHECKS ------------------------ */

int legal_move(PtrConfig c, List* poss_moves ,Move move,int print);
int valid_start_pos(PtrConfig c, Location src);
int invalid_position(Location loc);

/* -------------------- MOVES-GENERATING FUNCTIONS -------------------- */

Move parse_move(char* input, PtrConfig c);
int generate_moves(Config* c, List* moves,int create_board);
int generate_legal_moves_loc(Config* c, List* moves,List * legal_moves, Location loc);
int generate_legal_moves(PtrConfig c, List * moves);
int generate_moves_loc(Config* c, List* moves, Location loc,int create_board);
int generate_man_moves(Config* c, List* moves, Location loc,int create_board);
int generate_rook_moves(Config* c, List* moves, Location loc,int create_board);
int generate_bishop_moves(Config* c, List* moves, Location loc,int create_board);
int generate_knight_moves(Config* c, List* moves, Location loc,int create_board);
int generate_queen_moves(Config* c, List* moves, Location loc,int create_board);
int generate_king_moves(Config* c, List* moves, Location loc,int create_board);

/* ------------------- PIECE-MANIPULATION FUNCTIONS ------------------- */

char get_piece_from_str(char* p, int color);
int same_piece_type(char p1, char p2);
int is_piece_not_turn(char p1, int turn);
int is_piece_turn(char p1, int turn);
int make_move(PtrConfig c, List* poss_moves, Move move);
Location piece_location(Config* c, char piece);

#endif /* FLOW_H_ */



