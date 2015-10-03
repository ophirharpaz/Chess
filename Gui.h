/*
 * gui.h
 *
 *  Created on: Sep 12, 2015
 *      Author: linesh
 */

#ifndef GUI_H_
#define GUI_H_

# include "SDL/SDL.h"
# include "SDL/SDL_image.h"
# include "Flow.h"

#define MAX_BUTTONS 64
#define BUTTON_W 182
#define BUTTON_H 54
#define ZERO_RECT create_rect(0, 0, 0, 0)
#define BOARD_RECT create_rect(0, 0, 600, 600)
#define SQUARE_SIZE 68
#define SPACE 8

#define NEW_GAME_CASE 5
#define LOAD_GAME_CASE 6
#define QUIT_CASE 7
#define PLAYER_VS_PLAYER_CASE 3
#define PLAYER_VS_PC_CASE 4
#define WHITE_BEGINS_CASE 5
#define BLACK_BEGINS_CASE 6
#define SET_BOARD_CASE 7
#define CANCEL_PS_CASE 8
#define NEXT_PS_CASE 9
#define DIFF_1 4
#define DIFF_2 5
#define DIFF_3 6
#define DIFF_4 7
#define DIFF_BEST 8
#define USER_WHITE_CASE 9
#define USER_BLACK_CASE 10
#define CANCEL_S_CASE 11
#define PLAY_S_CASE 12
#define CANCEL_BS_CASE 80 // need to think of name

/* Structures and Factories */

typedef struct label {
	int x_pos;
	int y_pos;
	char text[100];
} label;

typedef struct image {
	char pathToImage[100];
} image;

typedef struct button {
	SDL_Rect rect;
	image image;
	label label;
	SDL_Surface* button_window;
} button;

typedef struct panel {
	SDL_Rect rect;
	image image;
	SDL_Surface* panel_window;
	button children[MAX_BUTTONS];
	int children_len;
} panel;

typedef struct window {
	char header[20];
	panel children[10];
	image bgImage;
	SDL_Surface* main_window;
	int children_len;
	char id;
} window;

/* Declarations */
//control create_button(char* text, int pos_x, int pos_y);
int FilterEvents(const SDL_Event *event);
int main_gui(Config c);

/** Constructors for GUI controls **/
int create_button(button * b,SDL_Rect parent_rect, int x_pos, int y_pos, int width, int height, image image);
label create_label(int x_pos, int y_pos, char text[100]);
image create_image(char pathToImage[100]);
int create_panel(SDL_Rect parent_rect,panel *p,int children_len,int x_pos, int y_pos, int width, int height,image image, button children[10]);
int create_window(window* w, int children_len, char header[20], panel children[10], image bgImage, char id);
SDL_Rect create_rect(int x, int y, int width, int height);
int edit_button(button* b, char* pathToImage);
button custom_button(Location loc, char piece, int button_selected,SDL_Rect p_rect);

/** Drawing functions for all GUI controls **/
int draw_window(window window);
int draw_panel(SDL_Surface* mainWindow,panel p);
int draw_label(label label, int parent_x, int parent_y);
int draw_image(image image, int parent_x, int parent_y);
int draw_button(SDL_Surface* mainWindow,button b);


int draw_main_window(window * w,PtrConfig c);
int draw_player_selection_window(window* w, Config* c);
int draw_settings_window(window* w, Config* c);
int draw_set_board_window(window* w, Config* c);
int create_set_board_menu(panel* menu_panel, SDL_Rect menu_rect,Config * c);
int create_board_panel(panel* board_panel, SDL_Rect board_rect,Config * c);
button custom_button(Location loc, char piece, int button_selected, SDL_Rect p_rect);
int draw_game_window(window* w, Config* c);
int remove_piece(window * w,button * new_b,button * b,int index, Location loc,char new_path[23]);
int create_slots_panel(panel* slots_panel, SDL_Rect slots_rect, Config* c);
int draw_empty_panel(panel* toBeFilled, SDL_Rect slots_rect, Config* c);
int draw_promote_panel(panel* promote_panel, SDL_Rect slots_rect, Config* c);

int draw_move_made(window* w,Config * c,int src_index,int dst_index,Location src,Location dst,button* b,SDL_Rect rect);
int draw_curr_state(window* w,Config *c);
int change_piece_marking(Location dst_loc,Config * c,SDL_Rect rect,window * w,int index,int mark);
int update_square(window* w,Config* c, Location loc, char piece, button b);

int loc_to_index(Location loc);
Location index_to_loc(int index);

/* On-Click functions */
int onClick(int event, window *w, Config* c);
int onClick_main_window(int event, window* w, Config* c);
int onClick_player_selection_window(int event, window* w, Config* c);
int onClick_settings_window(int event, window* w, Config* c);
int onClick_game_window(int event, window* w, Config* c);
int create_game_menu(panel* menu_panel, SDL_Rect menu_rect, Config* c);
int onClick_set_board_window(int event, window* w, Config* c);
int find_panel(SDL_Event mouseMotion,window w);
int find_button(SDL_Event mouseMotion,panel p,int j);

/* Toggle functions */
int toggle_buttons(window* w, int panel, char* path1, char* path2, int child_index);
int toggle_diffs(window* w, char paths[5][30], int child_index);

/* Free functions */
int free_window(window w);
int free_panel(panel p);
int free_button(button b);

#endif /* GUI_H_ */
