# include "DataStructures.h"
# include "Minimax.h"

Location create_loc(int i, int j) {
	Location loc = {i, j};
	return loc;
}

// Returns a specific location from a substring of move string entered by user
Location parse_location(char* loc_str) {
	Location loc;
	char* brack1 = strchr(loc_str, '<') + 1;
	loc.col = (int) brack1[0] - 97;
	brack1 += 2;
	char* brack2 = strchr(brack1, '>');
	brack2[0] = '\0';
	loc.row = atoi(brack1) - 1;
	brack2[0] = '>';
	return loc;
}

// Prints a struct Location in the required format
int print_location(Location loc) {
	if (loc.col == -2 && loc.row == -2) {
		printf("Null Loc");
	} else {
		char c = (char) (loc.col + 97);
		printf("<%c,%d>", c, loc.row + 1);
	}
	return 0;
}

int compare_locations(Location l1, Location l2) {
	return (l1.col == l2.col) && (l1.row == l2.row);
}

// Prints struct Move as required
int print_move(Move move) {
	if (move.type == 'c') {
		printf("castle ");
		print_location(move.src);
		return 0;
	}
	print_location(move.src);
	printf(" to ");
	print_location(move.dst);
	printf(" ");
	switch (move.type) {
	case 'n':
		printf("knight");
		break;
	case 'r':
		printf("rook");
		break;
	case 'b':
		printf("bishop");
		break;
	case 'q':
		printf("queen");
		break;
	}
	printf(" %d\n",move.score);
//	print_board_board(move.board);
	return 0;
}

// Recursively prints content of all nodes in list
void print_list(List* lst) {
	rec_print_list(lst->first);
}

void rec_print_list(PtrNode head) {
	if (head != NULL) {
		print_move(head->m);
		printf("\n");
		rec_print_list(head->next);
	}
}

void print_line() {
	int i;
	printf("  |");
	for (i = 1; i < BOARD_SIZE * 4; i++) {
		printf("-");
	}
	printf("|\n");
}

void print_board(Config* c) {
	int i, j;
	print_line();
	for (j = BOARD_SIZE - 1; j >= 0; j--) {
		printf((j < 9 ? " %d" : "%d"), j + 1);
		for (i = 0; i < BOARD_SIZE; i++) {
				printf("| %c ", BOARD(j, i));
			}
		printf("|\n");
		print_line();
	}
	printf("   ");
	for (j = 0; j < BOARD_SIZE; j++) {
		printf(" %c  ", (char) ('a' + j));
	}
	printf("\n");
}

void init_board(Config* c) {
	char pieces[] ="rnbqkbnr";
	int i, j;
	for (j = 0; j < BOARD_SIZE; j++) {
		BOARD(BOARD_SIZE-1,j) = toupper(BOARD(0,j) = pieces[j]);
		BOARD(BOARD_SIZE-2,j) = toupper(BOARD(1,j) = 'm');
	}
	for (i = 2; i < BOARD_SIZE-2; i++) {
		for (j = 0; j < BOARD_SIZE; j++ )
			BOARD(i, j) = EMPTY;
	}
}

// Removes all pieces from board.
int clear_board(Config* c) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			BOARD(i,j)=EMPTY;
		}
	}
	return 0;
}

int copy_board(char board[BOARD_SIZE][BOARD_SIZE],char next_board[BOARD_SIZE][BOARD_SIZE]) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			next_board[i][j] = board[i][j];
		}
	}
	return 1;
}

Config init_config(){
	Config c={ 4 , 1 , WHITE , 0 , WHITE, 1};//need to change depth to 1 still...
	init_board(&c);
	return c;
}

Config create_new_config(PtrConfig c, int turn, char board[BOARD_SIZE][BOARD_SIZE], int depth){
	Config new=init_config();
	new.TURN=turn;
	new.DEPTH=depth;
	copy_board(board,new.board);
	return new;
}

void print_config(PtrConfig c){
//	printf("score=%d\n",score(c,c->TURN));
	printf("depth=%d\n",c->DEPTH);
	printf("mode=%d\n",c->MODE);
	printf("user_color=%d\n",c->USER_COLOR);
	printf("end_game=%d\n",c->END_GAME);
	printf("turn=%d\n",c->TURN);
	printf("castle=%d\n",c->CASTLE);
	printf("difficulty=%d\n",c->DIFFICULTY);
	print_board(c);
}

List* init_list(int sort) {
	List* new = malloc(sizeof(List));
	if (new == NULL) {
		perror_message("malloc");
		return NULL;
	}
	new->first = NULL;
	new->size = 0;
	new->sort= sort;
	return new;
}

Move create_move(Location src, Location dst, char type, int with_board, Config* c, int threat) {
	Move move = { src, dst, type, -1, threat };
	if (with_board) {
		create_board_move(c, &move);
	}
	return move;
}

int create_board_move(PtrConfig c,Move * move) {
	copy_board(c->board,move->board);
	make_legal_move(move->board, *move, c->TURN);
	return 1;
}

int get_best_moves(PtrConfig c,List * legal_moves, int depth){
	Config new=create_new_config(c, c->TURN, c->board, depth);
	alphabeta(new,legal_moves);
	int best=0;
	Node * curr_node;
	printf("in get_best_moves\n");
	for (curr_node=legal_moves->first;curr_node!=NULL;curr_node=curr_node->next){
		best=max(best,curr_node->m.score);
	}for (curr_node=legal_moves->first;curr_node!=NULL;curr_node=curr_node->next){
		if(curr_node->m.score==best){
			print_move(curr_node->m);
			printf("\n");
		}
	}
	return 1;
}

int get_location_moves(PtrConfig c,List * legal_moves, Location loc){
	Node * curr_node;
	for(curr_node=legal_moves->first;curr_node!=NULL;curr_node=curr_node->next){
		if(compare_loc(curr_node->m.src,loc)){
			print_move(curr_node->m);
			printf("\n");
		}
	}
	return 1;
}

int get_score(PtrConfig c, int depth,List * legal_moves,Move move){
	Config new=create_new_config(c, c->TURN, c->board, depth);
	alphabeta(new,legal_moves);
	Node * curr_node;
	for (curr_node=legal_moves->first;curr_node!=NULL;curr_node=curr_node->next){
		if(compare_move(curr_node->m,move)){
			printf("%d",curr_node->m.score);
			break;
		}
	}
	return 1;
}

Move best_move(List* moves) {
	int max_score = -10000;
	Move best_move;
	int i = 0;
	int best_num=0;
	Node* head = moves->first;
	while (i < moves->size) {
		if (head->m.score > max_score) {
			max_score = head->m.score;
			best_move = head->m;
			best_num=0;
		}if (head->m.score == max_score) {
			best_num++;
		}
		head = head->next;
		i++;
	}
	srand(time(NULL));

	int num= (rand()%(best_num))+1;
	i=0;
	head = moves->first;
	while(i<num){
		if (head->m.score==max_score){
			i++;
			if (i==num){
				break;
			}
		}
		head = head->next;
	}
	best_move=head->m;
	return best_move;
}

int make_legal_move(char board[BOARD_SIZE][BOARD_SIZE], Move move, int turn) {
	char piece = board[move.src.row][move.src.col];
	board[move.src.row][move.src.col] = EMPTY;
	switch (move.type) {
	case 'o':
		board[move.dst.row][move.dst.col] = piece;
		break;
	case 'c':
		// castle(c, move);
		break;
	default:
		board[move.dst.row][move.dst.col] =
				turn == BLACK ? toupper(move.type) : move.type;
	}
	return 1;
}

Node* create_node(Move d) {
	Node* new = malloc(sizeof(Node));
	if (new == NULL) {
		perror_message("malloc");
		return 0;
	}
	new->m = d;
	new->next = NULL;
	return new;
}

void pop_data(List* list) {
	PtrNode next=list->first->next;
	free(list->first);
	list->first = next;
	list->size--;
}

void rec_insert_after(PtrNode head,PtrNode new){
	if (head->next==NULL || (head->m.score <= new->m.score && new->m.score<= head->next->m.score)){
		new->next=head->next;
		head->next=new;
	}else{
		rec_insert_after(head->next,new);
	}
}

void print_board_board(char board[BOARD_SIZE][BOARD_SIZE]) {
	int i, j;
	print_line();
	for (j = BOARD_SIZE - 1; j >= 0; j--) {
		printf((j < 9 ? " %d" : "%d"), j + 1);
		for (i = 0; i < BOARD_SIZE; i++) {
			printf("| %c ", board[j][i]);
		}
		printf("|\n");
		print_line();
	}
	printf("   ");
	for (j = 0; j < BOARD_SIZE; j++) {
		printf(" %c  ", (char) ('a' + j));
	}
	printf("\n");
}

int insert_move(List* list, Move move) {
	Node* new = create_node(move);
	if (new == NULL) {
		free_list(list);
		return 0;
	}
	if (list->size==0){
		list->first = new;
		list->size++;
		return 1;
	}
	if (new->m.score>list->first->m.score ||!list->sort){
		new->next = list->first;
		list->first = new;
	}else{
		rec_insert_after(list->first,new);
	}
	list->size++;
	return 1;
}

int print_node(Node* node) {
	print_move(node->m);
	return 1;
}

int free_list(List* list) {
	while(list->size>0){
		pop_data(list);
	}
	free(list);
	return 1;
}

int find_move(List * list, Move move) {
	Node* head = list->first;
	while (head!=NULL) {
		if (compare_move(head->m, move)) {
			return 1;
		}
		head = head->next;
	}
	return 0;
}

int compare_move(Move this, Move other) {
	return (compare_loc(this.src,other.src)) && (compare_loc(this.dst,other.dst));
}

int compare_loc(Location this, Location other) {
	return this.col == other.col && this.row == other.row;
}
