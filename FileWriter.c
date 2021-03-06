# include "FileWriter.h"

/* --------------------- FILE LOADING --------------------- */


/** Expects: pointer to char (string)
 *  returns a pointer to the next line **/

char* next_line(char* p) {
	return strchr(p, '\n') + 1;
}

/** Expects: pointer to char (string) and const pointer to char (string)
 * compares the two strings- start of p the next appearence of ">" with text
 *  returns result of comparison **/
int compare_line_field(char* p, const char* text) {
	p = strchr(p, '>') + 1;
	int n = strlen(text);
	char tmp = p[n];
	p[n] = '\0';
	int res = strcmp(text, p);
	p[n] = tmp;
	return res;
}

/** Expects: pointer to configuration, pointer to char (string), and number of row.
 * loads the string to the row of board in configuration.
 *  returns void **/
void load_line(PtrConfig c, char* p, int row) {
	p += 9;
	int j;
	for (j = 0; j < BOARD_SIZE; j++) {
		BOARD(row,j)=p[j]=='_'?EMPTY:p[j];
	}
}


/** Expects: pointer to configuration, pointer to char (relative or absolute path string)
 * opens the file in the path, and loads it to configuration c
 *  returns 0 in case of failure, 1 otherwise**/
int load_file(char* path, PtrConfig c) {
	char resolved_path[PATH_MAX];
	path[strlen(path) - 1] = '\0';
	char buffer[500];
	if (realpath(path, resolved_path) == NULL) {
		perror("realpath");
		return 0;
	}
	FILE *ifp = NULL;
	ifp = fopen(resolved_path, "r");
	if (ifp == NULL) {
		perror_message("fopen");
		return 0;
	}

	char chr;
	int i = 0;
	while (fscanf(ifp, "%c", &chr) > 0) {
		buffer[i++] = chr;
	}
	buffer[i - 1] = '\0';
	char * line3 = next_line(next_line(buffer));
	c->TURN = (compare_line_field(line3, "White") == 0) ? WHITE : BLACK;
	char *line4 = next_line(line3);
	c->MODE = (compare_line_field(line4, "1") == 0) ? 1 : 2;
	char *line5 = next_line(line4);
	char *line6 = next_line(line5);
	if (c->MODE == 2) {
		if (compare_line_field(line5, "best") == 0) {
			c->BEST = 1;
			c->DEPTH = 4;
		} else {
			c->BEST = 0;
			char *depth = strchr(line5, '>') + 1;
			c->DEPTH = atoi(depth);
		}
		c->USER_COLOR =
				(compare_line_field(line6, "White") == 0) ? WHITE : BLACK;
	} else {
		c->BEST = 0;
		c->DEPTH = 1;
		c->USER_COLOR = 0;
	}

	char *line8 = next_line(next_line(line6));

	char *curr_line = line8;

	for (i = BOARD_SIZE - 1; i > -1; i--) {
		load_line(c, curr_line, i);
		curr_line = next_line(curr_line);
	}
	c->END_GAME=0;
	fclose(ifp);
	return 1;
}

/* --------------------- FILE SAVING --------------------- */

/** Expects: pointer to file, pointer to char (field string), pointer to char (content string), int depth
 * writes line with field inx ml style, with depth number of padding tabs
 *  returns 0 in case of failure, 1 otherwise**/
int write_line(FILE* fp, char* field, char* content, int depth) {
	int i;
	for (i = 0; i < depth; i++) {
		if (fprintf(fp, "\t") < 0) {
			perror_message("fprintf");
			return 0;
		}
	}
	char new_line[8];
	strncpy(new_line, content, 8);
	for (i = 0; new_line[i]!='\0' && i < 8; i++) {
		new_line[i] = new_line[i] == ' ' ? '_' : new_line[i];
	}
	if (*content=='e'){
		if (fprintf(fp, "<%s></%s>\n", field,field) < 0) {
			perror_message("fprintf");
			return 0;
		}
	}else{
		if (fprintf(fp, "<%s>%.8s</%s>\n", field, new_line, field) < 0) {
			perror_message("fprintf");
			return 0;
		}
	}
	return 1;
}

/** Expects: pointer to configuration, pointer to char (relative or absolute path string)
 * saves the configuration to file path as instructed
 *  *  returns 0 in case of failure, 1 otherwise**/
int save_file(char* path, PtrConfig c) {
	char resolved_path[PATH_MAX];
	realpath(path, resolved_path);
	path[strlen(path) - 1] = '\0';
	resolved_path[strlen(resolved_path) - 1] = '\0';
	FILE *fp = NULL;
	fp = fopen(resolved_path, "w");
	if (fp == NULL) {
		perror_message("fopen");
		return 0;
	}

	if (fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<game>\n")
			< 0) {
		perror_message("fprintf");
		return 0;
	}

	if (!write_line(fp, "next_turn", c->TURN == WHITE ? "White" : "Black", 1)) {
		return 0;
	}

	char res[2];
	res[1] = '\0';
	res[0] = c->MODE + '0';
	if (!write_line(fp, "game_mode", res, 1))
		return 0;
	if (c->MODE == 2) {
		res[0] = c->DEPTH + '0';
		if (!write_line(fp, "difficulty", c->BEST == 1 ? "best" : res,
				1)) {
			return 0;
		}
		if (!write_line(fp, "user_color",
				c->USER_COLOR == WHITE ? "White" : "Black", 1)) {
			return 0;
		}

	} else {
		if (!write_line(fp, "difficulty", "e", 1)) {
			return 0;
		}
		if (!write_line(fp, "user_color", "e", 1)) {
			return 0;
		}
	}

	fprintf(fp, "\t<board>\n");
	int i;
	char field[6] = "row_0";
	field[5] = '\0';
	for (i = BOARD_SIZE - 1; i > -1; i--) {
		field[4] = i + 1 + '0';
		if (!write_line(fp, field, c->board[i], 2)) {
			return 0;
		}
	}
	if (fprintf(fp, "\t</board>\n") < 0) {
		perror_message("fprintf");
		return 0;
	}

	if (fprintf(fp, "</game>") < 0) {
		perror_message("fprintf");
		return 0;
	}
	fclose(fp);

	return 1;
}

