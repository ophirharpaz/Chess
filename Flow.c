# include "Flow.h"
# include "FileWriter.h"
# include "gui.h"
# include "Minimax.h"

int main(int argc, char* argv[]) {
	Config c = init_config();
	main2(c);
//	test(&c,8);
//	print_board(&c);
//	settings_state(&c);
//	game_state(&c);
	return 0;
}

int test(PtrConfig c, int num) {
	Location src = create_loc(1, 4);
	if (num == 1) {
		List * moves = init_list(0);
		Move move = { src, create_loc(3, 5) };
		Move move_2 = { src, create_loc(2, 5) };
		insert_move(moves, move);
		insert_move(moves, move_2);
		print_list(moves);
		free_list(moves);
	}
	else if (num == 2) {//create moves for c2
		List * moves = init_list(0);
		generate_moves_loc(c, moves, src,1);
		print_list(moves);
		free_list(moves);
	}
	else if (num == 3) {
		printf("reached line=%d\n", __LINE__);
		List * moves = init_list(0);
		generate_moves(c, moves,1);
		print_list(moves);
		free_list(moves);
	}
	else if (num == 4) {
		printf("reached line=%d\n", __LINE__);
		List * moves = init_list(0);
		BOARD(4,4)='b';
		BOARD(1,4)=EMPTY;
		print_board(c);
		generate_moves(c, moves,1);
		print_list(moves);
		free_list(moves);
	}else if (num == 5) {
		printf("reached line=%d\n", __LINE__);
		List * moves = init_list(0);
		print_board(c);
		generate_legal_moves(c, moves);
		print_list(moves);
		free_list(moves);
	}else if (num == 6) {
		printf("reached line=%d\n", __LINE__);
		List * moves = init_list(0);
		generate_legal_moves(c,moves);
		c->DEPTH=4;
		alphabeta(*c, moves);//config, move list,depth,alpha,beta,maximize player,score_turn
		Move best=best_move(moves);
		print_move(best);
		//print_list(moves);
		free_list(moves);
	}else if (num == 7) {//checks get_best_move
		printf("reached line=%d\n", __LINE__);
		int depth=3;
		BOARD(5,6)='m';
		BOARD(1,5)=' ';
		List * moves = init_list(0);
		generate_legal_moves(c,moves);
		get_best_moves(c,moves,depth);
	}else if (num == 8) {//checks get_score
		printf("reached line=%d\n", __LINE__);
		//(Location src, Location dst, char type, int with_board, Config* c, int threat)
		BOARD(5,6)='m';
		BOARD(1,5)=' ';
		Move move= create_move(create_loc(5,6),create_loc(6,7),'o',0,c,1);
		int depth=3;
		List * moves = init_list(0);
		generate_legal_moves(c,moves);
		get_score(c,depth,moves,move);
	}
	return 1;
}

//* --------------------- SETTINGS STATE FUNCTIONS --------------------- *//

int settings_state(PtrConfig c) {
	while (1) {
		print_message(ENTER_SETTINGS);
		char input[51];
		if (fgets(input, 50, stdin) == NULL) {
			perror_message("fgets");
			c->END_GAME = 1;
			return 0;
		}
		if (strncmp(input, "game_mode", 9) == 0) {
			char* x = strchr(input, ' ');
			int mode = atoi(x);
			if ((mode != 1) && (mode != 2)) {
				print_message(WRONG_GAME_MODE);
			} else {
				c->MODE = mode;
				if (mode == 1) {
					print_message(TWO_PLAYERS_GAME_MODE);
				} else {
					print_message(PLAYER_VS_AI_GAME_MODE);
				}
			}
		} else if (strncmp(input, "difficulty", 10) == 0) {
			if (c->MODE != 2) {
				print_message(ILLEGAL_COMMAND);
			}
			char* p = strchr(input, ' ');
			if (!set_minimax_depth(p, c)) {
				print_message(WRONG_MINIMAX_DEPTH);
			}
		} else if (strncmp(input, "user_color", 10) == 0) {
			if (c->MODE != 2) {
				print_message(ILLEGAL_COMMAND);
			}
			char* p = strchr(input, ' ');
			c->USER_COLOR = strcmp(p, " black") == 0 ? BLACK : WHITE;
		} else if (strncmp(input, "load", 4) == 0) {
			char* path = strchr(input, ' ') + 1;
			if (!load_file(path, c)) {
				print_message(WRONG_FILE_NAME);
			}
		} else if (strncmp(input, "clear", 5) == 0) {
			clear_board(c);
		} else if (strncmp(input, "next_player", 11) == 0) {
			char* p = strchr(input, ' ');
			c->TURN = strcmp(p, " black") == 0 ? BLACK : WHITE;
		} else if (strncmp(input, "rm", 2) == 0) {
			char* p = strchr(input, ' ');
			Location rm_loc = parse_location(p);
			if (!remove_by_pos(c, rm_loc)) {
				print_message(WRONG_POSITION);
			}
		} else if (strncmp(input, "set", 3) == 0) {
			char* p = strchr(input, ' ') + 1;
			char* color_start = strchr(p, ' ') + 1;
			int color = (strncmp(color_start, "white", 5) == 0) ? WHITE : BLACK;
			char* type_start = strchr(color_start, ' ') + 1;
			char type = get_piece_from_str(type_start, color);
			Location set_loc = parse_location(p);
			int res = set_by_pos(c, set_loc, color, type);
			if (res == 0) {
				print_message(WRONG_POSITION);
			}
			if (res == 1) {
				print_message(NO_PIECE);
			}
		} else if (strcmp(input, "print\n") == 0) {//remove print config later
			print_board(c);
		} else if (strcmp(input, "quit\n") == 0) {
			c->END_GAME = 1;
			break;
		} else if (strcmp(input, "start\n") == 0) {
			if (ok_to_start(c)){//need to add no pawns in final rows
				break;
			}
			print_message(WRONG_BOARD_INITIALIZATION);
		} else {
			print_message(ILLEGAL_COMMAND);
		}
	}
	return 1;
}

// plays computer player and human player according to determined settings.
// switches turns after turn is made. if board is losing position, announces and quits game.
int set_minimax_depth(char* p, Config* config) {
	if (strcmp(p, " best\n") == 0) {
		config->DIFFICULTY = 1;
		config->DEPTH = 4;
		return 1;
	}
	p = strchr(p, ' ');
	int d = atoi(p);
	if (1 <= d && d <= 4) {
		config->DIFFICULTY = 0;
		config->DEPTH = d;
		return 1;
	}
	return 0;
}

//* ----------------------- GAME STATE FUNCTIONS ----------------------- *//

int game_state(PtrConfig c) {
	while (!c->END_GAME) {
		List* moves = init_list(1);
		if (moves == NULL) { // malloc failure...
			return 0;
		}
		if (!generate_legal_moves(c, moves)) { // malloc failure...
			free_list(moves);
			return 0;
		}
		int ch=check(c);
		if (ch==2){
			c->END_GAME = 1;
			free_list(moves);
			break;
		}
		if (moves->size==0) {//no legal moves for player
			if (ch){//player is threatened
				printf("Mate! %s player wins!\n",c->TURN == BLACK ? "white" : "black");
			}else{
				print_message(TIE);
			}
			c->END_GAME = 1;
			break;
		}
		if (c->MODE == 1 || c->USER_COLOR == c->TURN) {
			if (ch){//opponent checks!
				print_message(CHECK);
			}
			user_turn(c, moves);
		} else {
			computer_turn(c, moves);
		}
		c->TURN = 1 - c->TURN;
	}
	return 0;
}

//* Checks whether one of the c->TURN's king is threatened by opponent *//

int check(Config* c) {
	Location king_location = piece_location(c, c->TURN == 1 ? 'k' : 'K');
	List* moves = init_list(0);
	if (moves==NULL){
		return 2;
	}
	c->TURN=!c->TURN;
	if (!generate_moves(c, moves,0)){
		return 2;
	}
	Node* curr = moves->first;
	while (curr != NULL) {
		if (compare_locations(king_location, curr->m.dst) && curr->m.threat) {
			c->TURN=!c->TURN;
		//	printf("king is taken in this board!\n");
			free_list(moves);
			return 1;
		}
		curr = curr->next;
	}
	free_list(moves);
	c->TURN=!c->TURN;
	return 0;
}

int king_is_taken(PtrConfig c, Move move) {
	Config new=create_new_config(c,c->TURN,move.board,0);
	return check(&new);
}

int generate_legal_moves(PtrConfig c, List * moves) {
	if(!generate_moves(c, moves,1)){
		return 0;
	}
	PtrNode curr_node = moves->first;
	while (curr_node != NULL) {
		int taken= king_is_taken(c, curr_node->m);
		if (taken==2){
			free_list(moves);
			return 0;
		}if (taken==1){
			pop_data(moves);
			curr_node = moves->first;
		}else{
			break;
		}

	}
	while (curr_node != NULL && curr_node->next != NULL) {
		PtrNode next_node = curr_node->next;
		int taken=king_is_taken(c, next_node->m);
		if (taken==2){
			free_list(moves);
			return 0;
		}
		if (taken==1) {
			curr_node->next = next_node->next;
			free(next_node);
			moves->size--;
		}else{
			curr_node = next_node;
		}
	}
	return 1;
}

/** Reads user input, and performs specified operations on board. plays user_turn **/
int user_turn(Config* c,List * moves) {
	while (1) {
		printf("%s player - enter your move:\n",
				(c->TURN == WHITE) ? "white" : "black");
		char input[51];
		if (fgets(input, 50, stdin) == NULL) {
			perror_message("fgets");
			c->END_GAME = 1;
			return 0;
		}
		if (strncmp(input, "move", 4) == 0) {
			Move move = parse_move(input, c);
			printf("\n");
			if (make_move(c, moves, move)) {
				print_board(c);
				break;
			}
		} else if (strncmp(input, "get_moves", 9) == 0) {
			Location loc=parse_location(input);
			if (invalid_position(loc) ) { //position not found on board
				print_message(WRONG_POSITION);
			}
			else if(!valid_start_pos(c, loc)){
				print_message(NO_PIECE);
			}else{
				get_location_moves(c,moves,loc);
			}
		} else if (strncmp(input, "get_best_moves", 14) == 0) {
			char* depth = strchr(input, ' ') + 1;
			int d=(strncmp(depth, "best", 4) == 0)?4:atoi(depth);
			if(!get_best_moves(c,moves,d)){
				c->END_GAME = 1;//malloc error
				break;
			}
		} else if (strncmp(input, "get_score", 9) == 0) {
			char* depth = strchr(input, ' ') + 1;
			depth[1]='\0';
			int d=(strncmp(depth, "best", 4) == 0)?4:atoi(depth);
			depth[1]=' ';
			Move move = parse_move(depth+2, c);
			if(legal_move(c,moves,move,1)){
				get_score(c,d,moves,move);
			}
		} else if (strncmp(input, "save", 4) == 0) {
			char* path = strchr(input, ' ') + 1;
			if (!save_file(path, c)) {
				print_message(WRONG_FILE_NAME);
			}
		} else if (strcmp(input, "quit\n") == 0) {
			c->END_GAME = 1;
			break;
		} else {
			print_message(ILLEGAL_COMMAND);
		}
	}
	free_list(moves);
	return 1;
}

// Performs computer's best move calculated by "Minimax".
int computer_turn(Config* c,List *moves) {
	if (!alphabeta(*c, moves)) {//malloc error
		c->END_GAME = 1;
		return 0;
	}
	Move comp_move = best_move(moves);
	make_legal_move(c->board, comp_move, c->TURN);
	printf("Computer: move ");
	print_move(comp_move);
	printf("\n");
	free_list(moves);
	return 1;
}

int player_wins(PtrConfig c){
	c->TURN=!c->TURN;
	int ch=check(c);//see if opponent is being checked by player
	if (ch==2){
		return -1;
	}
	List* moves = init_list(0);
	if (moves == NULL) { // malloc failure...
		return -1;
	}
	if (!generate_legal_moves(c, moves)) { // malloc failure...
		c->TURN=!c->TURN;
		free_list(moves);
		return -1;
	}
	c->TURN=!c->TURN;
	if (moves->size==0){
		if (ch==1){//wins
			return 1;
		}
		return 2;//tie
	}
	return 0;//not a win and not a tie
}

//* ------------------------ LEGAL MOVES CHECKS ------------------------ *//

int legal_move(PtrConfig c, List* poss_moves, Move move, int print) {
	if (invalid_position(move.src) || invalid_position(move.dst)) { //position not found on board
		if (print) {
			print_message(WRONG_POSITION);
		}
		return 0;
	}
	if (!valid_start_pos(c, move.src)) { //starting position does not contain user's piece
		if (print) {
			print_message(NO_PIECE);
		}
		return 0;
	}


	int legal = find_move(poss_moves, move);
	if (!legal) {
		if (print) {
			print_message(ILLEGAL_MOVE);
		}
		return 0;
	}
	return 1;
}

int valid_start_pos(PtrConfig c, Location src) {
	return is_piece_turn(BOARD(src.row, src.col), c->TURN);
}

int invalid_position(Location loc) {
	return loc.row < 0 || loc.row > BOARD_SIZE - 1 || loc.col < 0
			|| loc.col > BOARD_SIZE - 1;
}

//* -------------------- MOVES-GENERATING FUNCTIONS -------------------- *//

// returns 1 if p1, p2 are of the same type (man / king), 0 otherwise
Move parse_move(char* input, PtrConfig c) {
	Location src = parse_location(input);
	Location dst = parse_location(strchr(input, '<') + 1);
	char type = 'o';
	if ((c->TURN == WHITE && dst.row == 7)
			|| (c->TURN == BLACK && dst.row == 0)) {
		if (strlen(input) > 20) {
			input += 20;
			type = (strncmp(input, "knight\n", 2) == 0) ? input[1] : input[0];
		} else { //strlen(input==20
			type = 'q';
		}
	}
	Move move = create_move(src, dst, type, 0, c, 0);
	return move;
}

int generate_moves(Config* c, List* moves,int create_boards) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (is_piece_turn(BOARD(i, j), c->TURN)) {
				if (!generate_moves_loc(c, moves, create_loc(i, j),create_boards)) { // generate_moves_loc returns 0 iff loc is empty! redundant.
					return 0;
				}
			}
		}
	}
	return 1;
}

// fills List moves with all legal, possible moves of the current turn's player
int generate_moves_loc(Config* c, List* moves, Location loc, int create_boards) {
	char piece = tolower(BOARD(loc.row, loc.col));
	switch (piece) {
	case EMPTY:
		return 1;
	case 'm':
		if (!(generate_man_moves(c, moves, loc,create_boards))){
			return 0;
		}
		break;
	case 'r':
		if (!(generate_rook_moves(c, moves, loc,create_boards))){
			return 0;
		}
		break;
	case 'b':
		if (!(generate_bishop_moves(c, moves, loc,create_boards))){
			return 0;
		}
		break;
	case 'n':
		if (!(generate_knight_moves(c, moves, loc,create_boards))){
			return 0;
		}
		break;
	case 'q':
		if (!(generate_queen_moves(c, moves, loc,create_boards))){
			return 0;
		}
		break;
	case 'k':
		if (!(generate_king_moves(c, moves, loc,create_boards))){
			return 0;
		}
		break;
	}
	return 1;
}

int generate_man_moves(Config* c, List* moves, Location loc,int create_boards) {
	char piece = BOARD(loc.row, loc.col);
	int sign = islower(piece) ? 1 : -1;
	int promote = ((islower(piece) && loc.row + sign == 7)
			|| (islower(piece) && loc.row + sign == 0));
	Location diag_right = { loc.row + sign, loc.col + 1 };
	Location diag_left = { loc.row + sign, loc.col - 1 };
	Location one_step = { loc.row + sign, loc.col };
	char types[5];
	if (promote) {
		strcpy(types, "rbqn");
		types[4] = '\0';
	} else {
		strcpy(types, "o");
		types[1] = '\0';
	}
	int i;
	for (i = 0; types[i] != '\0'; i++) {
		if (BOARD(one_step.row, one_step.col)== EMPTY) {
			Move move = create_move(loc, one_step, types[i], create_boards, c, 0);
			if (!insert_move(moves, move)) {
				return 0;
			}
		}
		if (!invalid_position(diag_right) && is_piece_not_turn(BOARD(diag_right.row,diag_right.col),c->TURN)) {
			Move move = create_move(loc, diag_right, types[i], create_boards, c, 1);
			if (!insert_move(moves, move)) {
				return 0;
			}
		}
		if (!invalid_position(diag_left) && is_piece_not_turn(BOARD(diag_left.row,diag_left.col),c->TURN)) {
			Move move=create_move(loc, diag_left, types[i], create_boards, c, 1);
			if(!insert_move(moves, move)) {
				return 0;;
			}
		}
	}
	return 1;
}

int generate_rook_moves(Config* c, List* moves, Location loc,int create_boards) {
	Location dst = { loc.row + 1, loc.col };
	while (!invalid_position(dst) && BOARD(dst.row, dst.col)== EMPTY ) {
		if (!insert_move(moves, create_move(loc, dst, 'o', 1, c,1))) {
			return 0;
		}
		dst.row++;
	}
	if (!invalid_position(dst)&& is_piece_not_turn(BOARD(dst.row, dst.col), c->TURN)) {
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
	}
	dst = create_loc(loc.row - 1, loc.col);
	while (!invalid_position(dst) && BOARD(dst.row, dst.col)== EMPTY) {
		if (!insert_move(moves, create_move(loc, dst, 'o',create_boards, c,1))) {
			return 0;
		}
		dst.row--;
	}
	if (!invalid_position(dst)
			&& is_piece_not_turn(BOARD(dst.row, dst.col), c->TURN)) {
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
	}
	dst = create_loc(loc.row, loc.col - 1);
	while (!invalid_position(dst) && BOARD(dst.row, dst.col)== EMPTY) {
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
		dst.col--;
	}
	if (!invalid_position(dst)
			&& is_piece_not_turn(BOARD(dst.row, dst.col), c->TURN)) {
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
	}
	dst = create_loc(loc.row, loc.col + 1);
	while (!invalid_position(dst) && BOARD(dst.row, dst.col)== EMPTY) {
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
		dst.col++;
	}
	if (!invalid_position(dst)
			&& is_piece_not_turn(BOARD(dst.row, dst.col), c->TURN)) {
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
	}
	return 1;
}

int generate_bishop_moves(Config* c, List* moves, Location loc, int create_boards) {
	Location dst = { loc.row + 1, loc.col + 1 };
	while (!invalid_position(dst) && BOARD(dst.row, dst.col)== EMPTY ) { // up-right
		if (!insert_move(moves, create_move(loc, dst, 'o', 1, c, 1))) {

			return 0;
		}
		dst.row++;
		dst.col++;
	}
	if (!invalid_position(dst)
			&& is_piece_not_turn(BOARD(dst.row, dst.col), c->TURN)) {
		if (!insert_move(moves, create_move(loc, dst, 'o', 1, c, 1))) {
			return 0;
		}
	}
	dst = create_loc(loc.row + 1, loc.col - 1);
	while (!invalid_position(dst) && BOARD(dst.row, dst.col)== EMPTY ) { // up-left
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
		dst.row++;
		dst.col--;
	}
	if (!invalid_position(dst)
			&& is_piece_not_turn(BOARD(dst.row, dst.col), c->TURN)) {
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
	}
	dst = create_loc(loc.row - 1, loc.col + 1);
	while (!invalid_position(dst) && BOARD(dst.row, dst.col)== EMPTY ) { // down-right
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c,1))) {
			return 0;
		}
		dst.row--;
		dst.col++;
	}
	if (!invalid_position(dst)
			&& is_piece_not_turn(BOARD(dst.row, dst.col), c->TURN)) {
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
	}
	dst = create_loc(loc.row - 1, loc.col - 1);
	while (!invalid_position(dst) && BOARD(dst.row, dst.col)== EMPTY ) { // down-left
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c,1))) {
			return 0;
		}
		dst.row--;
		dst.col--;
	}
	if (!invalid_position(dst)
			&& is_piece_not_turn(BOARD(dst.row, dst.col), c->TURN)) {
		if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c, 1))) {
			return 0;
		}
	}
	return 1;
}

int generate_knight_moves(Config* c, List* moves, Location loc, int create_boards) {
	int options[8][2] = { { 2, 1 }, { 2, -1 }, { 1, 2 }, { 1, -2 }, { -2, 1 }, {
			-2, -1 }, { -1, 2 }, { -1, -2 } };
	int i;
	for (i = 0; i < 8; i++) {
		Location dst = create_loc(loc.row + options[i][0],
				loc.col + options[i][1]);
		if (!invalid_position(dst) && ((BOARD(dst.row, dst.col) == EMPTY)
		|| (is_piece_not_turn(BOARD(dst.row, dst.col),c->TURN)))) {
			if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c,1))) {
				return 0;
			}
		}
	}
	return 1;
}

int generate_queen_moves(Config* c, List* moves, Location loc,int create_boards) {
	return (generate_rook_moves(c, moves, loc,create_boards)
			&& generate_bishop_moves(c, moves, loc, create_boards));
}

int generate_king_moves(Config* c, List* moves, Location loc,int create_boards) {
	int options[8][2] = { { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 },
			{ -1, 0 }, { -1, 1 }, { 0, 1 } };
	int i;
	for (i = 0; i < 8; i++) {
		Location dst = create_loc(loc.row + options[i][0],
				loc.col + options[i][1]);
		if (!invalid_position(dst) && (BOARD(dst.row, dst.col)== EMPTY
		||is_piece_not_turn(BOARD(dst.row, dst.col),c->TURN))) {
			if (!insert_move(moves, create_move(loc, dst, 'o', create_boards, c,1))) {
				return 0;
			}
		}
	}
	return 1;
}

//* ------------------- PIECE-MANIPULATION FUNCTIONS ------------------- *//

char get_piece_from_str(char* p, int color) {
	char res;
	if (strncmp(p, "kn", 2) == 0) {
		res = 'n';
	} else if (strncmp(p, "pa", 2) == 0) {
		res = 'm';
	} else {
		res = p[0];
	}
	return (color == BLACK) ? toupper(res) : res;
}

int same_piece_type(char p1, char p2) {
	return tolower(p1) == tolower(p2);
}

int is_piece_not_turn(char p1, int turn) {
	int c = islower(p1) > 0 ? WHITE : BLACK;
	return c != turn && p1 != EMPTY;
}

int is_piece_turn(char p1, int turn) {
	int c = islower(p1) > 0 ? WHITE : BLACK;
	return c == turn && p1 != EMPTY;
}

int make_move(PtrConfig c, List* poss_moves, Move move) {
	if (!(legal_move(c, poss_moves, move, 1))) {
		return 0;
	}
	return make_legal_move(c->board, move, c->TURN);
}

int set_by_pos(PtrConfig c, Location set_loc, int color, char type) {
	if (invalid_position(set_loc)) {
		return 0;
	}
	if (type == BOARD(set_loc.row, set_loc.col)) { // piece already in place
		return 2;
	}
	int cnt = count_pieces(c, type);
	type = tolower(type);

	if ((type == 'k' || type == 'q') && (cnt >= 1)) {
		return 1;
	} else if ((type == 'r' || type == 'n' || type == 'b') && (cnt >= 2)) {
		return 1;
	} else if (type == 'm' && (cnt >= 8)) {
		return 1;
	} else {
		if (color == BLACK) {
			type = (char) toupper((int) type);
		}
		BOARD(set_loc.row, set_loc.col)= type;
		return 2;
	}
}

int remove_by_pos(PtrConfig c, Location rm_loc) {
	if (rm_loc.row < 0 || rm_loc.row > BOARD_SIZE - 1 || rm_loc.col < 0
			|| rm_loc.col > BOARD_SIZE - 1) {
		return 0;
	}
	BOARD(rm_loc.row, rm_loc.col)= EMPTY;
	return 1;
}

int count_pieces(PtrConfig c, char type) {
	int i, j;
	int cnt = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (BOARD(i,j)== type) {
				cnt++;
			}
		}
	}
	return cnt;
}

Location piece_location(Config* c, char piece) { //need to define default location;
	int i, j;
	Location res;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (BOARD(i,j)== piece) {
				return res;
			}
		}
	}
	return res;
}

int ok_to_start(Config * c){
	if (!(count_pieces(c,'k')==1) && (count_pieces(c,'K')==1)){
		return 0;
	}int j;
	for (j=0;j<BOARD_SIZE;j++){
		if (BOARD(BOARD_SIZE-1,j)=='m'||BOARD(0,j)=='M'){
			return 0;
		}
	}
	return 1;
}
