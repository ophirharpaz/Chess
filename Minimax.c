# include "Minimax.h"
# include "Flow.h"


/** Expects: pointer to configuration and int turn
 *  calculates the score of the configuration,
 *  relative to argument turn, using best_piece_score.
 *  Returns: score of configuration (int). **/

int best_score(PtrConfig c, int turn) {
	int wins = player_wins(c);
	if (wins == -1) { //malloc failure
		return 20000;
	}
	if (wins == 1) { //player wins
		if (c->TURN==turn){
			return 1000000;
		}return -1000000;
	}
	if (wins == 2) { // player ties
		return -999999;
	}
	int score = 0;
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			score += best_piece_score(BOARD(i, j), turn);
		}
	}
	return score;
}


/** Expects: char piece and int turn
 *  returns score of piece according to best scoring function
 *  Returns: score of piece (int). **/

int best_piece_score(char piece, int turn) {
	if (piece == EMPTY) {
		return 0;
	}
	int p = islower(piece) > 0 ? WHITE : BLACK;
	int sign = ((p == turn) ? 1 : -1);
	piece = tolower(piece);
	int pawn = 100;	int knight = 320;	int bishop = 325;
	int rook = 500;	int queen = 975;	int king = 32767;
	switch (piece) {
	case 'm':
		return pawn * sign;
	case 'n':
		return knight * sign;
	case 'b':
		return bishop * sign;
	case 'r':
		return rook * sign;
	case 'q':
		return queen * sign;
	case 'k':
		return king * sign;
	}
	return 0;
}


/** Expects: pointer to configuration and int turn
 *  calculates the score of the configuration,
 *  relative to argument turn, using piece_score.
 *  Returns: score of configuration (int). **/
int score(PtrConfig c, int turn) {
	int wins = player_wins(c);
	if (wins == -1) { //malloc failure
		return 20000;
	}
	if (wins == 1) { //player wins
		if (c->TURN==turn){
			return 500;
		}return -500;
	}
	if (wins == 2) { // player ties
		return -499;
	}
	int score = 0;
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			score += piece_score(BOARD(i, j), turn);
		}
	}
	return score;
}

/** Expects: char piece and int turn
 *  returns score of piece according to scoring function
 *  Returns: score of piece (int). **/
int piece_score(char piece, int turn) {
	if (piece == EMPTY) {
		return 0;
	}
	int p = islower(piece) > 0 ? WHITE : BLACK;
	int sign = ((p == turn) ? 1 : -1);
	piece = tolower(piece);
	int pawn = 1;
	int knight = 3;
	int bishop = 3;
	int rook = 5;
	int queen = 9;
	int king = 400;
	switch (piece) {
	case 'm':
		return pawn * sign;
	case 'n':
		return knight * sign;
	case 'b':
		return bishop * sign;
	case 'r':
		return rook * sign;
	case 'q':
		return queen * sign;
	case 'k':
		return king * sign;
	}
	return 0;
}

/** Expects: pointer to configuration and list of legal moves.
 *  determines score of moves in list, according to configuration paramaters.
 *  records number of calls to alphabeta in variable board_num
 *  Returns: 0 in case of failure, 1 otherwise. **/
int alphabeta(Config c, List* moves) {
	int board_num = moves->size;
	if (rec_alphabeta(c, moves, c.DEPTH, -100000, 100000, 1, c.TURN, &board_num)
			== 20000) {
		return 0;
	}
	//printf("number_of_boards is=%d\n",board_num);
	return 1;

}


/** Expects: pointer to configuration, list of legal moves, original depth, alpha and beta parameters
 *  for pruning,max/min player,score_turn,number of calls to alphabeta.
 *  performs the alphabeta algorithm recursively on all moves (nodes) in list moves.
 *  Returns: 20000 in case of failure, score of move otherwise. **/
int rec_alphabeta(Config c, List* moves, int depth, int alpha, int beta,
		int max_player, int score_turn, int* board_num) {
	(*board_num)++;

	if (depth != c.DEPTH) { //if it is not the first level of the alphabeta algorithm
		//calculate list of legal moves for configuration only if it is not the first level of the alphabeta.
		if (!generate_legal_moves(&c, moves)) {
			free_list(moves); //malloc failure
			return 20000;
		}
	}
	int v;
	int i = 0;
	Node* child_node = moves->first;
	if (c.DEPTH == 0 || moves->size == 0) { // node is a terminal node

		if (moves->size == 0) {
			c.TURN = 1 - c.TURN;
		}
		if (c.DEPTH != depth) {
			free_list(moves);
		}
		int s = c.BEST ? best_score(&c, score_turn):score(&c, score_turn);
		if (s == 20000) { //malloc failure
			free_list(moves);
		}
		return s;
	}
	if (max_player) {//maximizing player
		v = -1000001;
		while (i < moves->size) {
			Move child = child_node->m;
			List * next_moves = init_list();
			if (next_moves == NULL) {
				free_list(moves);
				return 20000;
			}
			Config new = create_new_config(&c, 1 - c.TURN, child.board,
					c.DEPTH - 1);
			int new_score = rec_alphabeta(new, next_moves, depth, alpha, beta,
					0, score_turn, board_num);
			if (new_score == 20000) {
				free_list(moves);
				return 20000;
			}
			v = max(v, new_score);
			alpha = max(alpha, v);
			if (c.DEPTH == depth) {
				child_node->m.score = new_score;
			}
			if (beta < alpha) {
				break;
			}
			// pruning starts only in second level of the alphabeta tree
			if ((beta == alpha) && (c.DEPTH < depth - 1)) {
				break;
			}

			i++;
			child_node = child_node->next;
		}
		if (c.DEPTH != depth) {
			free_list(moves);
		}
		return v;
	} else {
		v = 1000001;
		while (i < moves->size) {
			Move child = child_node->m;
			List * next_moves = init_list();
			if (next_moves == NULL) {
				free_list(moves);
				return 20000;
			}
			Config new = create_new_config(&c, 1 - c.TURN, child.board,
					c.DEPTH - 1);
			int new_score = rec_alphabeta(new, next_moves, depth, alpha, beta,
					1, score_turn, board_num);
			v = min(v, new_score);
			beta = min(beta, v);
			if (beta < alpha) {
				break;
			}
			if ((beta == alpha) && (c.DEPTH < depth - 1)) {
				break;
			}
			i++;
			child_node = child_node->next;
		}
	}
	if (c.DEPTH != depth) {
		free_list(moves);
	}
	return v;
}

int max(int a, int b) {
	return a > b ? a : b;
}

int min(int a, int b) {
	return a < b ? a : b;
}
