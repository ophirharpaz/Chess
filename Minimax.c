# include "Minimax.h"
# include "Flow.h"

int score(PtrConfig c, int turn) {
	int wins = player_wins(c);
	if (wins == -1) { //malloc failure
		return 20000;
	}
	if (wins == 1) { //player wins
		return 500;
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

int alphabeta(Config c, List* moves) {
	int board_num = moves->size;
	if (rec_alphabeta(c, moves, c.DEPTH, -100000, 100000, 1, c.TURN, &board_num)
			== 20000) {
		return 0;
	}
	//printf("number_of_boards is=%d\n",board_num);
	return 1;

}

int rec_alphabeta(Config c, List* moves, int depth, int alpha, int beta,
		int max_player, int score_turn, int* board_num) {
	(*board_num)++;
	if (depth != c.DEPTH) {
		if (!generate_legal_moves(&c, moves)) {
			free_list(moves); //malloc failure
			return 20000;
		}
	}
	int v;
	int i = 0;
	Node* child_node = moves->first;
	if (c.DEPTH == 0 || moves->size == 0) { // node is a terminal node
		if (c.DEPTH != depth) {
			free_list(moves);
		}
		if (moves->size == 0) {
			c.TURN = 1 - c.TURN;
		}
		int s = score(&c, score_turn);
		if (s == 20000) { //malloc failure
			free_list(moves);
		}
		return s;
	}
	if (max_player) {
		v = -100000;
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
		v = 100000;
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
