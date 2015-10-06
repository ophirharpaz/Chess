# include "Flow.h"

/** Expects: main arguments - argc and argv.
 *  Chooses between console or gui modes.
 *  Returns: 0 when finishes. **/
int main(int argc, char* argv[]) {
	Config c = init_config();
	if (argc == 2 && (strcmp(argv[1], "gui") == 0)) {
		main_gui(c);
	} else if ((argc == 2 && (strcmp(argv[1], "console") == 0)) || argc == 1) {
		settings_state(&c);
		game_state(&c);
	}
	return 0;
}

/** --------------------- SETTINGS STATE FUNCTIONS --------------------- **/

/** Expects: pointer to configuration c.
 *  Translates user commands into configuration settings.
 *  Returns: 0 in case of error, 1 otherwise. **/
int settings_state(PtrConfig c) {
	print_board(c);
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
			} else {
				char* p = strchr(input, ' ');
				if (!set_minimax_depth(p, c)) {
					print_message(WRONG_MINIMAX_DEPTH);
				}
			}
		} else if (strncmp(input, "user_color", 10) == 0) {
			if (c->MODE != 2) {
				print_message(ILLEGAL_COMMAND);
			}
			char* p = strchr(input, ' ');
			c->USER_COLOR = strcmp(p, " black\n") == 0 ? BLACK : WHITE;
		} else if (strncmp(input, "load", 4) == 0) {
			char* path = strchr(input, ' ') + 1;
			if (!load_file(path, c)) {
				print_message(WRONG_FILE_NAME);
			}else{
				print_board(c);
			}
		} else if (strncmp(input, "clear", 5) == 0) {
			clear_board(c);
		} else if (strncmp(input, "next_player", 11) == 0) {
			char* p = strchr(input, ' ');
			c->TURN = strcmp(p, " black\n") == 0 ? BLACK : WHITE;
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
				print_message(INVALID_SETTING);
			}
		} else if (strcmp(input, "print\n") == 0) {
			print_board(c);
		} else if (strcmp(input, "quit\n") == 0) {
			c->END_GAME = 1;
			break;
		} else if (strcmp(input, "start\n") == 0) {
			if (ok_to_start(c)){
				break;
			}
			print_message(WRONG_BOARD_INITIALIZATION);
		} else {
			print_message(ILLEGAL_COMMAND);
		}
	}
	return 1;
}

/** Expects: pointer to configuration c, string p.
 *  sets DEPTH, BEST fields in configuration c according to p's content.
 *  Returns: 0 in case of error, 1 otherwise. **/
int set_minimax_depth(char* p, Config* config) {
	if (strcmp(p, " best\n") == 0) {
		config->BEST = 1;
		config->DEPTH = 4;
		return 1;
	}
	p = strchr(p + 1, ' ');
	int d = atoi(p);
	if (1 <= d && d <= 4) {
		config->BEST = 0;
		config->DEPTH = d;
		return 1;
	}
	return 0;
}

/** Expects: pointer to configuration c, Location set_loc, piece type and color.
 *  Check the validity of placing the piece in set_loc. If valid - places the piece.
 *  Returns: 0 in case of invalid position, 2 if set_loc is occupied or placement went right,
 *  1 if placement is not logic according to game rules. **/
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

/** Expects: pointer to configuration c, location rm_loc.
 *  Checks the validity of removal and removes if possible.
 *  Returns: 0 in case of invalidity, 1 in success. **/
int remove_by_pos(PtrConfig c, Location rm_loc) {
	if (rm_loc.row < 0 || rm_loc.row > BOARD_SIZE - 1 || rm_loc.col < 0
			|| rm_loc.col > BOARD_SIZE - 1) {
		return 0;
	}
	BOARD(rm_loc.row, rm_loc.col)= EMPTY;
	return 1;
}

/** Expects: pointer to configuration c and piece type.
 *  Returns: the number of pieces of type "type". **/
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

/** Expects: pointer to configuration c.
 *  Returns: 0 in case the board is not set well, 1 otherwise. **/
int ok_to_start(Config * c) {
	if (!(count_pieces(c,'k')==1) || !(count_pieces(c,'K')==1)){
		return 0;
	}int j;
	for (j=0;j<BOARD_SIZE;j++){
		if (BOARD(BOARD_SIZE-1,j)=='m'||BOARD(0,j)=='M'){
			return 0;
		}
	}
	return 1;
}

//* ----------------------- GAME STATE FUNCTIONS ----------------------- *//

/** Expects: pointer to configuration c.
 *  Switches between user and computer turns, while printing necessary messages to the console.
 *  Returns: 0. **/
int game_state(PtrConfig c) {

	while (!c->END_GAME) {
		List* moves = init_list();
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
				printf("Mate! %s player wins the game\n",c->TURN == BLACK ? "White" : "Black");
			}else{
				print_message(TIE);
			}
			c->END_GAME = 1;
			free_list(moves);
			break;
		}
		if (ch){
			print_message(CHECK);
			}
		if (c->MODE == 1 || c->USER_COLOR == c->TURN) {
			user_turn(c, moves);
		} else {
			computer_turn(c, moves);
		}
		c->TURN = 1 - c->TURN;
	}
	return 0;
}

/** Expects: pointer to configuration c, moves list.
 *  Performs user turn (along other required commands), considering possible moves.
 *  Returns: 0 in case of error, 1 otherwise. **/
int user_turn(Config* c,List * moves) {
	while (1) {
		printf("%s player - enter your move:\n",
				(c->TURN == WHITE) ? "White" : "Black");
		char input[51];
		if (fgets(input, 50, stdin) == NULL) {
			perror_message("fgets");
			c->END_GAME = 1;
			free_list(moves);
			return 0;
		}
		if (strncmp(input, "move", 4) == 0) {
			Move move = parse_move(input, c);
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
			int temp=c->BEST;
			c->BEST=(strncmp(depth, "best", 4) == 0)?1:0;
			if(!get_best_moves(c,moves,d)){
				c->BEST=temp;
				c->END_GAME = 1;//malloc error
				break;
			}c->BEST=temp;
		} else if (strncmp(input, "get_score", 9) == 0) {
			char* depth = strchr(input, ' ') + 1;
			depth[1]='\0';
			int d=(strncmp(depth, "best", 4) == 0)?4:atoi(depth);
			int temp=c->BEST;
			c->BEST=(strncmp(depth, "best", 4) == 0)?1:0;
			depth[1]=' ';
			Move move = parse_move(depth+2, c);
			if(legal_move(c,moves,move,1)){
				get_score(c,d,moves,move);
			}c->BEST=temp;
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

/** Expects: pointer to configuration c, moves list.
 *  Performs computer turn, considering possible moves and the set difficulty.
 *  Returns: 0 in case of error, 1 otherwise. **/
int computer_turn(Config* c,List *moves) {
	if (!alphabeta(*c, moves)) { // malloc error
		c->END_GAME = 1;
		return 0;
	}
	Move comp_move = best_move(moves);
	make_legal_move(c->board, comp_move, c->TURN);
	printf("Computer: move ");
	print_move(comp_move);
	printf("\n");
	free_list(moves);
	print_board(c);
	return 1;
}

/** Expects: pointer to configuration c.
 *  Scans the board and determines, according to the user moves, whether the match ends with tie, mate or neither.
 *  Returns: -1 in case of error, 1 in case user wins, 2 for tie and 0 for none. **/
int player_wins(PtrConfig c){
	c->TURN=!c->TURN;
	int ch=check(c);//see if opponent is being checked by player
	if (ch==2){
		return -1;
	}
	List* moves = init_list();
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
	free_list(moves);
	return 0;//not a win and not a tie
}

/** Expects: pointer to configuration c.
 *  Scans the board to determine if a player is checked by opponent.
 *  Returns: 2 in case of error, 1 in case of check and 0 otherwise. **/
int check(Config* c) {
	Location king_location = piece_location(c, c->TURN == WHITE ? 'k' : 'K');
	List* moves = init_list();
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
			free_list(moves);
			return 1;
		}
		curr = curr->next;
	}
	free_list(moves);
	c->TURN=!c->TURN;
	return 0;
}

/** Expects: pointer to configuration c and a move.
 *  Checks whether check will be performed in the next turn.
 *  Returns: same as check, only for next turn. **/
int king_is_taken(PtrConfig c, Move move) {
	Config new=create_new_config(c,c->TURN,move.board,0);
	return check(&new);
}

//* ------------------------ LEGAL MOVES CHECKS ------------------------ *//

/** Expects: pointer to configuration c, list of possible moves, a move (and print).
 *  Returns: 0 if move is illegal, 1 otherwise. **/
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

/** Expects: user input and configuration pointer, c.
 *  Returns: parsed move from the user input. **/
Move parse_move(char* input, PtrConfig c) {
	Location src = parse_location(input);
	Location dst = parse_location(strchr(input, '<') + 1);
	char type = 'o';
	if ((c->TURN == WHITE && dst.row == 7 &&BOARD(src.row,src.col)=='m')
			|| (c->TURN == BLACK && dst.row == 0&&BOARD(src.row,src.col)=='M')) {
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

/** Expects: pointer to configuration c, moves list, create_board int.
 *  Generates all pseudo-legal moves for configuration c (if create_board == 1, also generates the consequent boards).
 *  Returns: 0 in case of error, 1 otherwise. **/
int generate_moves(Config* c, List* moves,int create_boards) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (is_piece_turn(BOARD(i, j), c->TURN)) {
				if (!generate_moves_loc(c, moves, create_location(i, j),create_boards)) { // generate_moves_loc returns 0 iff loc is empty! redundant.
					return 0;
				}
			}
		}
	}
	return 1;
}

/** Expects: pointer to configuration c, moves list, Location loc and create_boards.
 *  Fills List moves with all legal, possible moves from the given location loc
 *  (if create_board == 1, also generates the consequent boards).
 *  Returns: 0 in case of success, 1 if loc is empty or otherwise. **/
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

/** Expects: pointer to configuration c, moves list.
 *  Fills List moves with all legal, possible moves for this turn's player.
 *  Returns: 0 in case of error, 1 otherwise. **/
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


/** ALL THE GENERATE_X_MOVES FUNCTIONS BELOW:
 *  Expect: pointer to configuration c, moves list, Location loc and create_boards.
 *  Fill List moves with all legal, possible moves for the piece located in loc.
 *  (if create_board == 1, also generates the consequent boards).
 *  Return: 0 in case of error, 1 otherwise. **/

int generate_man_moves(Config* c, List* moves, Location loc,int create_boards) {
	char piece = BOARD(loc.row, loc.col);
	int sign = islower(piece) ? 1 : -1;
	int promote = ((islower(piece) && loc.row + sign == 7)
			|| (isupper(piece) && loc.row + sign == 0));
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
	dst = create_location(loc.row - 1, loc.col);
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
	dst = create_location(loc.row, loc.col - 1);
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
	dst = create_location(loc.row, loc.col + 1);
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
	dst = create_location(loc.row + 1, loc.col - 1);
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
	dst = create_location(loc.row - 1, loc.col + 1);
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
	dst = create_location(loc.row - 1, loc.col - 1);
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
		Location dst = create_location(loc.row + options[i][0],
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
		Location dst = create_location(loc.row + options[i][0],
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

/** Expects: string p, color.
 *  Returns: a char corresponding to the piece in p, considering color. **/
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

/** Expects: two pieces given as chars.
 *  Returns: 1 if both are of the same type, 0 otherwise. **/
int same_piece_type(char p1, char p2) {
	return tolower(p1) == tolower(p2);
}

/** Expects: piece p1, int turn.
 *  Returns: 1 if it's not piece's turn **/
int is_piece_not_turn(char p1, int turn) {
	int c = islower(p1) > 0 ? WHITE : BLACK;
	return c != turn && p1 != EMPTY;
}

/** Expects: piece p1, int turn.
 *  Returns: 1 if it's piece's turn **/
int is_piece_turn(char p1, int turn) {
	int c = islower(p1) > 0 ? WHITE : BLACK;
	return c == turn && p1 != EMPTY;
}

/** Expects: configuration pointer c, list of possible moves and a move.
 * 	Checks if the move is legal and performs it if it is.
 *  Returns: 1 if the move was made properly, 0 otherwise. **/
int make_move(PtrConfig c, List* poss_moves, Move move) {
	if (!(legal_move(c, poss_moves, move, 1))) {
		return 0;
	}
	return make_legal_move(c->board, move, c->TURN);
}

/** Expects: configuration pointer c, piece.
 * 	Returns: the retrieved location of piece on the board in c. **/
Location piece_location(Config* c, char piece) {
	int i, j;
	Location res=create_location(-1,-1);
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (BOARD(i,j)== piece) {
				res=create_location(i,j);
			}
		}
	}
	return res;
}

