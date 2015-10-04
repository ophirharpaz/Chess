# include "Gui.h"

/** GUI Flow **/
int main_gui(Config c) {

	SDL_Event event;

	/* Initialize the SDL library (starts the event loop) */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	/* Clean up on exit, exit on window close and interrupt */
	atexit(SDL_Quit);
	window w;
	draw_main_window(&w, &c);
	SDL_Event mouseMotion;
	while (SDL_WaitEvent(&event) >= 0) {
		if(w.id=='g'&&c.MODE==ONE_PLAYER && c.TURN!=c.USER_COLOR && c.END_GAME==0){
			List * PC_moves=init_list();
			if (PC_moves==NULL){
				exit(0);
				return 0;
			}
			if (!generate_legal_moves(&c, PC_moves)) { // malloc failure...
				free_list(PC_moves);
				exit(0);
				return 0;
			}
			if (!alphabeta(c, PC_moves)) { // malloc error
				c.END_GAME = 1;
				exit(0);
				return 0;
			}
			print_list(PC_moves);
			Move comp_move = best_move(PC_moves);
			int src_index, dst_index;
			src_index = loc_to_index(comp_move.src);
			dst_index = loc_to_index(comp_move.dst);
			button comp_b = w.children[0].children[src_index];
			make_move(&c, PC_moves, comp_move);
			draw_move_made(&w, &c, src_index, dst_index, comp_move.src, comp_move.dst, &comp_b, BOARD_RECT);
			c.TURN = 1 - c.TURN;
			draw_curr_state(&w, &c);
			free_list(PC_moves);
		}
		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN: {
			Uint8 *keys;
			keys = SDL_GetKeyState(NULL);
			if (keys[SDLK_ESCAPE] == SDL_PRESSED) {
				exit(0);
			} else {
				int pressed_button = find_panel(mouseMotion, w);
				onClick(pressed_button, &w, &c);
			}
		}
		break;
		case SDL_MOUSEMOTION: {
			mouseMotion = event;

			break;
		}
		case SDL_QUIT: {
			exit(0);
		}
			break;
		}
	}
	printf("SDL_WaitEvent error: %s\n", SDL_GetError());
	exit(1);
}

/** GUI widgets creators **/
SDL_Rect create_rect(int x, int y, int width, int height) {
	SDL_Rect rect = { x, y, width, height };
	return rect;
}

int create_button(button* b, SDL_Rect parent_rect, int x_pos, int y_pos,
		int width, int height, image image) {
	b->rect = create_rect(parent_rect.x + x_pos, parent_rect.y + y_pos, width,
			height);
	b->image = image;
	b->button_window = IMG_Load(image.pathToImage);
	if (b->button_window == NULL) {
		fprintf(stderr, "Couldn't set 800x600x8 video mode: %s\n",
				SDL_GetError());
		return 0;
	}
	return 1;
}

int edit_button(button* b, char* pathToImage) {
	b->image = create_image(pathToImage);
	SDL_FreeSurface(b->button_window);
	b->button_window = IMG_Load(b->image.pathToImage);
	if (b->button_window == NULL) {
		fprintf(stderr, "Couldn't set 800x600x8 video mode: %s\n",
				SDL_GetError());
		return 0;
	}
	return 1;
}

image create_image(char pathToImage[100]) {
	image image;
	strcpy(image.pathToImage, pathToImage);
	return image;
}

int create_panel(SDL_Rect parent_rect, panel *p, int children_len, int x_pos,
		int y_pos, int width, int height, image image,
		button children[MAX_BUTTONS]) {
	p->rect = create_rect(parent_rect.x + x_pos, parent_rect.y + y_pos, width,
			height);
	int i;
	for (i = 0; i < MAX_BUTTONS; i++) {
		p->children[i] = children[i];
	}
	p->children_len = children_len;
	p->image = image;
	if (strncmp(image.pathToImage, "empty", 5) != 0) {
		p->panel_window = IMG_Load(image.pathToImage);
		if (p->panel_window == NULL) {
			fprintf(stderr, "Couldn't set 800x600x8 video mode: %s\n",
					SDL_GetError());
			return 0;
		}
	}
	return 1;
}

int create_window(window* w, int children_len, char header[20],
		panel children[10], image bgImage, char id) {
	w->children_len = children_len;
	strcpy(w->header, header);
	int i;
	for (i = 0; i < 10; i++) {
		w->children[i] = children[i];
	}
	w->bgImage = bgImage;
	w->id = id;
	w->main_window = SDL_SetVideoMode(800, 600, 32, 0);
	if (w->main_window == NULL) {
		fprintf(stderr, "Couldn't set 800x600x8 video mode: %s\n",
				SDL_GetError());
		return 0;
	}
	return 1;
}

/** widget-drawing functions **/
int draw_window(window w) {

	SDL_WM_SetCaption(w.header, 0); // NULL can be some icon in the future
//	SDL_WM_SetIcon(SDL_LoadBMP("mainIcon.bmp"), NULL); // Doesn't work

	// Set Background

	SDL_Surface* bg_temp = SDL_LoadBMP(w.bgImage.pathToImage);
	SDL_BlitSurface(bg_temp, NULL, w.main_window, NULL);
	SDL_Flip(w.main_window);

	// Draw panels
	panel child;
	int i = 0;
	for (child = w.children[i]; i < w.children_len; child = w.children[++i]) {
		draw_panel(w.main_window, child);
	}
	return 1;
}

int draw_panel(SDL_Surface* mainWindow, panel p) {
	if (strncmp(p.image.pathToImage, "empty", 5 != 0)) {
		SDL_BlitSurface(p.panel_window, NULL, mainWindow, &(p.rect));
		SDL_Flip(mainWindow);
	}
	button child;
	int i = 0;
	for (child = p.children[i]; i < p.children_len; child = p.children[++i]) {

		draw_button(mainWindow, child);
	}
	return 1;
}

int draw_button(SDL_Surface* mainWindow, button b) {
	SDL_BlitSurface(b.button_window, NULL, mainWindow, &(b.rect));
	SDL_Flip(mainWindow);
	return 1;
}

/** Program's windows drawing functions **/
int draw_main_window(window * w, PtrConfig c) {
	//initalize c
	c->DEPTH = 1;
	c->BEST = 0;
	c->MODE = 1;
	c->TURN = 1;
	c->USER_COLOR = 1;
	c->END_GAME = 0;
	init_board(c);
//	print_config(c);
	//initialize variables
	button panel_children[MAX_BUTTONS];
	button panel_children_title[MAX_BUTTONS];
	button panel_children_quit[MAX_BUTTONS];
	panel p1;
	panel p2;
	panel quit_panel;
	panel slots;
	button newGame;
	button loadGame;
	button quit;

	//create_rects
	SDL_Rect p1_rect = create_rect(0, 0, 80, 200);

	//create_images
	image newGameImage = create_image("graphics/NewGame.png");
	image loadGameImage = create_image("graphics/LoadGame.png");
	image quitImage = create_image("graphics/Quit.png");
	image window_image = create_image("graphics/bg.bmp");
	image logo_image = create_image("graphics/Logo.png");
	image panel_image = create_image("graphics/transparent.png");

	//create_buttons
	create_button(&newGame, p1_rect, 30, 150, BUTTON_W, BUTTON_H, newGameImage);
	create_button(&loadGame, p1_rect, 30, 225, BUTTON_W, BUTTON_H,
			loadGameImage);

	panel_children[0] = newGame;
	panel_children[1] = loadGame;

	create_button(&quit, create_rect(600, 500, 182, 54), 0, 0, BUTTON_W,
			BUTTON_H, quitImage);

	panel_children_quit[0] = quit;

	panel window_children[10];

	draw_empty_panel(&slots, ZERO_RECT, c);
	create_panel(ZERO_RECT, &p1, 2, 0, 0, 212, 300, panel_image,
			panel_children);
	create_panel(ZERO_RECT, &p2, 0, 240, 30, 600, 200, logo_image,
			panel_children_title);

	create_panel(ZERO_RECT, &quit_panel, 1, 600, 500, 182, 54, panel_image,
			panel_children_quit);

	window_children[0] = p1;
	window_children[1] = p2;
	window_children[2] = quit_panel;
	window_children[3] = slots;
	create_window(w, 4, "Chess", window_children, window_image, 'm');
	return draw_window(*w);
}

int draw_player_selection_window(window *w, Config* c) {
	free_window(*w);
	// initialize variables
	button panel_children[MAX_BUTTONS];
	panel window_children[10];
	panel p;
	panel header;
	button header_children[MAX_BUTTONS];
	button playerVsPlayer;
	button playerVsPC;
	button white;
	button black;
	button set_board;
	button cancel;
	button next;

	// create_rects;
	SDL_Rect p_rect = create_rect(0, 0, 80, 400);

	image panel_image = create_image("graphics/transparent.png");
	image bg_image = create_image("graphics/bg.bmp");
	image header_image = create_image("graphics/PlayerSelection.png");
	image playerVsPlayerImage =
			(c->MODE == TWO_PLAYERS) ?
					create_image("graphics/PlayerVsPlayerPressed.png") :
					create_image("graphics/PlayerVsPlayer.png");
	image playerVsPCImage =
			(c->MODE == ONE_PLAYER) ?
					create_image("graphics/PlayerVsPCPressed.png") :
					create_image("graphics/PlayerVsPC.png");
	image whiteBeginsImage =
			(c->TURN == WHITE) ?
					create_image("graphics/WhiteBeginsPressed.png") :
					create_image("graphics/WhiteBegins.png");
	image blackBeginsImage =
			(c->TURN == BLACK) ?
					create_image("graphics/BlackBeginsPressed.png") :
					create_image("graphics/BlackBegins.png");
	image setBoardImage = create_image("graphics/SetTheBoard.png");
	image cancelImage = create_image("graphics/Cancel.png");
	image nextImage = create_image("graphics/Next.png");

	create_button(&playerVsPlayer, p_rect, 30, 150, BUTTON_W, BUTTON_H,
			playerVsPlayerImage);
	create_button(&playerVsPC, p_rect, 250, 150, BUTTON_W, BUTTON_H,
			playerVsPCImage);
	create_button(&white, p_rect, 30, 225, BUTTON_W, BUTTON_H,
			whiteBeginsImage);
	create_button(&black, p_rect, 250, 225, BUTTON_W, BUTTON_H,
			blackBeginsImage);
	create_button(&set_board, p_rect, 30, 300, BUTTON_W, BUTTON_H,
			setBoardImage);
	create_button(&cancel, p_rect, 30, 450, BUTTON_W, BUTTON_H, cancelImage);
	create_button(&next, p_rect, 585, 450, BUTTON_W, BUTTON_H, nextImage);

	panel_children[0] = playerVsPlayer;
	panel_children[1] = playerVsPC;
	panel_children[2] = white;
	panel_children[3] = black;
	panel_children[4] = set_board;
	panel_children[5] = cancel;
	panel_children[6] = next;
	create_panel(ZERO_RECT, &header, 0, 164, 30, 472, 97, header_image,
			header_children);
	create_panel(ZERO_RECT, &p, 7, 0, 0, 800, 600, panel_image, panel_children);
	window_children[0] = p;
	window_children[1] = header;
	create_window(w, 2, "Chess", window_children, bg_image, 'p');
	return draw_window(*w);
}

int draw_settings_window(window* w, Config* c) {
	free_window(*w);

	// initialize variables
	panel window_children[10];
	panel header;
	button header_children[MAX_BUTTONS];
	panel difficulty;
	button difficulty_children[MAX_BUTTONS];
	panel panel;
	button panel_children[MAX_BUTTONS];

	// initialize buttons
	button diff1;
	button diff2;
	button diff3;
	button diff4;
	button diffBest;
	button IPlayWhite;
	button IPlayBlack;
	button cancel;
	button next;

	image panel_image = create_image("graphics/transparent.png");
	image bg_image = create_image("graphics/bg.bmp");
	image header_image = create_image("graphics/Settings.png");
	image difficultyImage = create_image("graphics/Difficulty.png");

	image diff1Image =
			(c->DEPTH == 1) ?
					create_image("graphics/1Pressed.png") :
					create_image("graphics/1.png");
	image diff2Image =
			(c->DEPTH == 2) ?
					create_image("graphics/2Pressed.png") :
					create_image("graphics/2.png");
	image diff3Image =
			(c->DEPTH == 3) ?
					create_image("graphics/3Pressed.png") :
					create_image("graphics/3.png");
	image diff4Image =
			(c->DEPTH == 4 && c->BEST == 0) ?
					create_image("graphics/4Pressed.png") :
					create_image("graphics/4.png");
	image diffBestImage =
			(c->BEST) ?
					create_image("graphics/BestPressed.png") :
					create_image("graphics/Best.png");

	image IPlayWhiteImage =
			(c->USER_COLOR == WHITE) ?
					create_image("graphics/IPlayWhitePressed.png") :
					create_image("graphics/IPlayWhite.png");
	image IPlayBlackImage =
			(c->USER_COLOR == BLACK) ?
					create_image("graphics/IPlayBlackPressed.png") :
					create_image("graphics/IPlayBlack.png");

	image cancelImage = create_image("graphics/Cancel.png");
	image nextImage = create_image("graphics/Next.png");

	// create rects;
	SDL_Rect p_rect = create_rect(0, 0, 80, 400);

	create_button(&diff1, p_rect, 30, 160, 100, BUTTON_H, diff1Image);
	create_button(&diff2, p_rect, 140, 160, 100, BUTTON_H, diff2Image);
	create_button(&diff3, p_rect, 250, 160, 100, BUTTON_H, diff3Image);
	create_button(&diff4, p_rect, 360, 160, 100, BUTTON_H, diff4Image);
	create_button(&diffBest, p_rect, 470, 160, 100, BUTTON_H, diffBestImage);

	create_button(&IPlayWhite, p_rect, 30, 280, BUTTON_W, BUTTON_H,
			IPlayWhiteImage);
	create_button(&IPlayBlack, p_rect, 250, 280, BUTTON_W, BUTTON_H,
			IPlayBlackImage);
	create_button(&cancel, p_rect, 30, 450, BUTTON_W, BUTTON_H, cancelImage);
	create_button(&next, p_rect, 585, 450, BUTTON_W, BUTTON_H, nextImage);

	panel_children[0] = diff1;
	panel_children[1] = diff2;
	panel_children[2] = diff3;
	panel_children[3] = diff4;
	panel_children[4] = diffBest;
	panel_children[5] = IPlayWhite;
	panel_children[6] = IPlayBlack;
	panel_children[7] = cancel;
	panel_children[8] = next;

	create_panel(ZERO_RECT, &header, 0, 269, 30, 261, 97, header_image,
			header_children);
	create_panel(ZERO_RECT, &difficulty, 0, 13, 100, 214, 70, difficultyImage,
			difficulty_children);
	create_panel(ZERO_RECT, &panel, 9, 0, 0, 800, 600, panel_image,
			panel_children);
	window_children[0] = header;
	window_children[1] = difficulty;
	window_children[2] = panel;
	create_window(w, 3, "Chess", window_children, bg_image, 's');
	return draw_window(*w);
}

int draw_set_board_window(window* w, Config* c) {
	free_window(*w);
	// initialize variables

	panel window_children[10];
	panel board_panel;
	panel menu;

	// create images
	image bg_image = create_image("graphics/transparent.png");

	// create rects;
	SDL_Rect board_rect = BOARD_RECT;
	SDL_Rect menu_rect = create_rect(600, 0, 200, 600);

	create_board_panel(&board_panel, board_rect, c);
	create_set_board_menu(&menu, menu_rect, c);
	window_children[0] = board_panel;
	window_children[1] = menu;
	create_window(w, 2, "Chess", window_children, bg_image, 'b');
	return draw_window(*w);
}

int create_set_board_menu(panel* menu_panel, SDL_Rect menu_rect, Config * c) {
	button menu_children[MAX_BUTTONS];
	char pieces[6] = { 'm', 'n', 'b', 'r', 'q', 'k' };
	char button_path[23];
	strcpy(button_path, "graphics/pieces/ u.png");
	int i;
	for (i = 0; i < 6; i++) {
		button_path[16] = pieces[i];
		image button_image = create_image(button_path);
		button b1;
		create_button(&b1, menu_rect, 20, SPACE + (SQUARE_SIZE + SPACE - 4) * i,
		SQUARE_SIZE,
		SQUARE_SIZE, button_image);
		menu_children[2 * i] = b1;
		button_path[16] = toupper(pieces[i]);
		button_image = create_image(button_path);
		button b2;
		create_button(&b2, menu_rect, 120,
		SPACE + (SQUARE_SIZE + SPACE - 4) * i, SQUARE_SIZE,
		SQUARE_SIZE, button_image);
		menu_children[2 * i + 1] = b2;
	}

	image bin_image = create_image("graphics/pieces/tu.png");
	image cancel_image = create_image("graphics/SmallCancel.png");
	image cont_image =
			(c->MODE == ONE_PLAYER) ?
					create_image("graphics/SmallNext.png") :
					create_image("graphics/Play.png");

	button bin, cancel, cont;
	create_button(&bin, menu_rect, 72, SPACE + (SQUARE_SIZE + SPACE - 4) * 6,
	SQUARE_SIZE, SQUARE_SIZE, bin_image);
	create_button(&cancel, menu_rect, 8, SPACE + (SQUARE_SIZE + SPACE - 4) * 7,
			92, BUTTON_H, cancel_image);
	create_button(&cont, menu_rect, 103, SPACE + (SQUARE_SIZE + SPACE - 4) * 7,
			92, BUTTON_H, cont_image);

	menu_children[12] = bin;
	menu_children[13] = cancel;
	menu_children[14] = cont;

	image bg_image = create_image("graphics/whitebgPanel.bmp");
	create_panel(ZERO_RECT, menu_panel, 15, 600, 0, 200, 600, bg_image,
			menu_children);

	return 1;
}

int create_board_panel(panel* board_panel, SDL_Rect board_rect, Config * c) {
	button board_children[MAX_BUTTONS];
	image bg_image = create_image("graphics/Board.png");
	// Fill board_children with 64 buttons
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			Location loc = create_location(i, j);
			board_children[i * (BOARD_SIZE) + j] = custom_button(loc,
					BOARD(i, j), 0, board_rect);
		}
	}

	create_panel(ZERO_RECT, board_panel, MAX_BUTTONS, 0, 0, 600, 600, bg_image,
			board_children);
	return 1;
}

int draw_game_window(window* w, Config* c) {
	free_window(*w);

	// initialize panels
	panel window_children[10];
	panel board_panel;
	panel menu;
	panel toBeFilled;

	// create images
	image bg_image = create_image("graphics/transparent.png");

	// create rects;
	SDL_Rect board_rect = BOARD_RECT;
	SDL_Rect menu_rect = create_rect(600, 0, 200, 600);

	create_board_panel(&board_panel, board_rect, c);
	create_game_menu(&menu, menu_rect, c);
	draw_empty_panel(&toBeFilled, ZERO_RECT, c);
	window_children[0] = board_panel;
	window_children[1] = menu;
	window_children[2] = toBeFilled;
	create_window(w, 3, "Chess", window_children, bg_image, 'g');
	draw_window(*w);
	return draw_curr_state(w, c);
}

int create_game_menu(panel* menu_panel, SDL_Rect menu_rect, Config* c) {
	button menu_children[MAX_BUTTONS];

	// initialize buttons
	button getMove;
	button saveGame;
	button mainMenu;
	button quit;

	// initialize images
	image bg_image = create_image("graphics/whitebgPanel.bmp");
	image getMove_image = create_image("graphics/GetMove.png");
	image saveGame_image = create_image("graphics/SaveGame.png");
	image mainMenu_image = create_image("graphics/MainMenu.png");
	image quit_image = create_image("graphics/Quit.png");

	create_button(&getMove, menu_rect, 8, 13, BUTTON_W, BUTTON_H,
			getMove_image);
	create_button(&saveGame, menu_rect, 8, 17 + BUTTON_H + SPACE, BUTTON_W,
			BUTTON_H, saveGame_image);
	create_button(&mainMenu, menu_rect, 8, 470, BUTTON_W, BUTTON_H,
			mainMenu_image);
	create_button(&quit, menu_rect, 8, 470 + BUTTON_H + SPACE, BUTTON_W,
			BUTTON_H, quit_image);

	menu_children[0] = getMove;
	menu_children[1] = saveGame;
	menu_children[2] = mainMenu;
	menu_children[3] = quit;

	create_panel(ZERO_RECT, menu_panel, 4, 600, 0, 200, 600, bg_image,
			menu_children);

	return 1;
}

int draw_empty_panel(panel* toBeFilled, SDL_Rect slots_rect, Config* c) {
	button toBeFilled_children[MAX_BUTTONS];
	image bg_image = create_image("graphics/transparent.png");
	create_panel(ZERO_RECT, toBeFilled, 0, 0, 0, 0, 0, bg_image,
			toBeFilled_children);
	return 1;
}

int create_slots_panel(panel* slots_panel, SDL_Rect slots_rect, Config* c) {
	button slots_children[MAX_BUTTONS];

	// initialize buttons
	int endOfLoop = ((SLOTS_NUM % 2) == 0)? SLOTS_NUM : SLOTS_NUM - 1;
	int i;
	char path[19] = "graphics/slotx.png";
	for (i = 0; i < endOfLoop; i++) {
		button b;
		path[13] = '0' + i;
		image button_image = create_image(path);
		create_button(&b, slots_rect, 32 + (54 + 3 * SPACE) * (i % 2),
				((i - (i % 2)) / 2) * (54 + SPACE), 54, 54,
				button_image);
		slots_children[i] = b;
	}
	if ((SLOTS_NUM % 2) == 1) { // odd number of slots
		button b;
		path[13] = '0' + i;
		image button_image = create_image(path);
		create_button(&b, slots_rect, 73, ((i - (i % 2)) / 2) * (54 + SPACE), 54, 54,
				button_image);
		slots_children[i] = b;
	}

	image SlotsPanelBG = create_image("graphics/transparent.png");
	create_panel(ZERO_RECT, slots_panel, SLOTS_NUM, slots_rect.x, slots_rect.y,
			slots_rect.w, slots_rect.h, SlotsPanelBG, slots_children);
	return 1;
}

int draw_depths_panel(panel* slots_panel, SDL_Rect slots_rect, Config* c) {
	button depths_children[MAX_BUTTONS];
	// initialize buttons
	int i;
	char path[20] = "graphics/depthx.png";
	for (i = 0; i < 4; i++) {
		button b;
		path[14] = '0' + (i + 1);
		image button_image = create_image(path);
		create_button(&b, slots_rect, 32 + (54 + 3 * SPACE) * (i % 2),
				((i - (i % 2)) / 2) * (54 + SPACE) + 9 * SPACE, 54, 54,
				button_image);
		depths_children[i] = b;
	}
	button b;
	path[14] = 'b';
	image button_image = create_image(path);
	create_button(&b, slots_rect, 73, 2 * (54 + SPACE) + 9 * SPACE, 54, 54,
			button_image);
	depths_children[4] = b;

	image depth_image = create_image("graphics/Depth.png");
	button depth;
	create_button(&depth, slots_rect, 40, 22, 150, 38, depth_image);
	depths_children[5] = depth;

	image DepthsPanelBG = create_image("graphics/transparent.png");

	create_panel(ZERO_RECT, slots_panel, 6, 600, 137, 198, 310, DepthsPanelBG,
			depths_children);
	return 1;
}

int draw_promote_panel(panel* promote_panel, SDL_Rect slots_rect, Config* c) {
	button promote_children[MAX_BUTTONS];
	// initialize buttons
	char pieces[4] = { 'q', 'n', 'b', 'r' };
	char path[23] = "graphics/pieces/xu.png";
	int i;
	for (i = 0; i < 4; i++) {
		button b;
		path[16] = (c->TURN == BLACK) ? toupper(pieces[i]) : pieces[i];
		image button_image = create_image(path);
		create_button(&b, slots_rect, 20 + (SQUARE_SIZE + 3 * SPACE) * (i % 2),
				((i - (i % 2)) / 2) * (SQUARE_SIZE + SPACE) + 9 * SPACE,
				SQUARE_SIZE, SQUARE_SIZE, button_image);
		promote_children[i] = b;
	}
	image DepthsPanelBG = create_image("graphics/transparent.png");
	create_panel(ZERO_RECT, promote_panel, 4, 600, 137, 198, 310, DepthsPanelBG,
			promote_children);
	return 1;
}

int draw_state(window* w, Config *c, int state, SDL_Rect rect) {
	panel p;
	button p_children[MAX_BUTTONS];
	image PanelBG;
	switch (state) {
	case 0:	//mate
		PanelBG = create_image("graphics/Mate.png");
		break;
	case 1:	//tie
		PanelBG = create_image("graphics/Tie.png");
		break;
	case 2:	//check
		PanelBG = create_image("graphics/Check.png");
		break;
	case 3:
		PanelBG = create_image("graphics/transparent.png");
	}
	create_panel(ZERO_RECT, &p, 0, rect.x + 40, rect.y + 300, rect.w, rect.h,
			PanelBG, p_children);
	free_panel(w->children[2]);
	w->children[2] = p;
	draw_panel(w->main_window, w->children[1]);
	draw_panel(w->main_window, w->children[2]);
	return 1;
}

int draw_curr_state(window* w, Config *c) {
	SDL_Rect menu_rect = create_rect(600, 0, 200, 600);
	List* opponent_moves = init_list();
	if (opponent_moves == NULL) { // malloc failure...
		exit(0);
		return 0;
	}
	if (!generate_legal_moves(c, opponent_moves)) { // malloc failure...
		free_list(opponent_moves);
		exit(0);
		return 0;
	}
	int ch = check(c);
	switch (ch) {
	case 2: //malloc failure
		printf("check function failed!");
		c->END_GAME = 1;
		free_list(opponent_moves);
		exit(0);
		return 0;
	case 0: //no check

		if (opponent_moves->size == 0) { //tie
			draw_state(w, c, 1, menu_rect);
			c->END_GAME = 1;
		} else if (w->children[2].image.pathToImage[9] != 't') { //it was check before

			draw_state(w, c, 3, menu_rect);
		}
		break;
	case 1: //check
		if (opponent_moves->size == 0) { //no moves
			draw_state(w, c, 0, menu_rect);
			printf("Mate! %s player wins!\n",
					c->TURN == BLACK ? "white" : "black");
			c->END_GAME = 1;
		} else { //display check
			draw_state(w, c, 2, menu_rect);
			print_message(CHECK);
		}
		break;
	}
	free_list(opponent_moves);
	return 1;

}

/** OnClick functions **/
button custom_button(Location loc, char piece, int button_selected,
		SDL_Rect p_rect) {
	char button_path[23];
	strcpy(button_path, "graphics/pieces/");
	button_path[16] = (piece == EMPTY) ? 'e' : piece;
	button_path[17] = (button_selected) ? 's' : 'u';
	strcpy(&(button_path[18]), ".png");
	button b;
	image button_image = create_image(button_path);
	create_button(&b, p_rect, 28 + loc.col * SQUARE_SIZE,
			28 + (7 - loc.row) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE,
			button_image);
	return b;
}

int onClick_main_window(int event, window* w, Config* c) {
	panel p;
	switch (event) {
	case NEW_GAME_CASE: {
		draw_player_selection_window(w, c);
		break;
	}
	case LOAD_GAME_CASE: {
		create_slots_panel(&p, create_rect(20, 287, 200, 310), c);
		free_panel(w->children[3]);
		w->children[3] = p;
		draw_panel(w->main_window, w->children[3]);
		break;
	}
	case QUIT_CASE: {
		free_window(*w);
		exit(0);
		break;
	}
	}
	if (8 <= event && event <= (8 + SLOTS_NUM - 1)) {
		char path[18] = "SavedGames/x.xml\n";
		path[11] = '0' + event - 8;
		load_file(path, c);
//		print_config(c);
		draw_player_selection_window(w, c);
	}

	return 0;
}

int onClick_player_selection_window(int event, window* w, Config* c) {
	switch (event) {
	case PLAYER_VS_PLAYER_CASE: {
		c->MODE = TWO_PLAYERS;
		toggle_buttons(w, 0, "graphics/PlayerVsPlayerPressed.png",
				"graphics/PlayerVsPC.png", 0);
		break;
	}
	case PLAYER_VS_PC_CASE: {
		c->MODE = ONE_PLAYER;
		toggle_buttons(w, 0, "graphics/PlayerVsPlayer.png",
				"graphics/PlayerVsPCPressed.png", 0);
		break;
	}
	case WHITE_BEGINS_CASE: {
		c->TURN = WHITE;
		toggle_buttons(w, 0, "graphics/WhiteBeginsPressed.png",
				"graphics/BlackBegins.png", 2);
		break;
	}
	case BLACK_BEGINS_CASE: {
		c->TURN = BLACK;
		toggle_buttons(w, 0, "graphics/WhiteBegins.png",
				"graphics/BlackBeginsPressed.png", 2);
		break;
	}
	case SET_BOARD_CASE: {
		draw_set_board_window(w, c);
		break;
	}
	case CANCEL_PS_CASE: {
		free_window(*w);
		draw_main_window(w, c);
		break;
	}
	case NEXT_PS_CASE: {
		if (c->MODE == TWO_PLAYERS) {
			return draw_game_window(w, c);
		} else {
			return draw_settings_window(w, c);
		}
	}
	}
	return 0;
}

int onClick_settings_window(int event, window* w, Config* c) {
	switch (event) {
	case DIFF_1: {
		c->BEST = 0;
		c->DEPTH = 1;
		char paths[5][30] = { "graphics/1Pressed.png", "graphics/2.png",
				"graphics/3.png", "graphics/4.png", "graphics/Best.png" };
		toggle_diffs(w, paths, 0);
		break;
	}
	case DIFF_2: {
		c->BEST = 0;
		c->DEPTH = 2;
		char paths[5][30] = { "graphics/1.png", "graphics/2Pressed.png",
				"graphics/3.png", "graphics/4.png", "graphics/Best.png" };
		toggle_diffs(w, paths, 0);
		break;
	}
	case DIFF_3: {
		c->BEST = 0;
		c->DEPTH = 3;
		char paths[5][30] = { "graphics/1.png", "graphics/2.png",
				"graphics/3Pressed.png", "graphics/4.png", "graphics/Best.png" };
		toggle_diffs(w, paths, 0);
		break;
	}
	case DIFF_4: {
		c->BEST = 0;
		c->DEPTH = 4;
		char paths[5][30] = { "graphics/1.png", "graphics/2.png",
				"graphics/3.png", "graphics/4Pressed.png", "graphics/Best.png" };
		toggle_diffs(w, paths, 0);
		break;
	}
	case DIFF_BEST: {
		c->BEST = 1;
		c->DEPTH = 4;
		char paths[5][30] = { "graphics/1.png", "graphics/2.png",
				"graphics/3.png", "graphics/4.png", "graphics/BestPressed.png" };
		toggle_diffs(w, paths, 0);
		break;
	}
	case USER_WHITE_CASE: {
		c->USER_COLOR = WHITE;
		toggle_buttons(w, 2, "graphics/IPlayWhitePressed.png",
				"graphics/IPlayBlack.png", 5);
		break;
	}
	case USER_BLACK_CASE: {
		c->USER_COLOR = BLACK;
		toggle_buttons(w, 2, "graphics/IPlayWhite.png",
				"graphics/IPlayBlackPressed.png", 5);
		break;
	}
	case CANCEL_S_CASE: {
		free_window(*w);
		return draw_main_window(w, c);
	}
	case PLAY_S_CASE: {
		return draw_game_window(w, c);
	}
	}
	return 0;
}

int onClick_set_board_window(int event, window* w, Config* c) {
	switch (event) {
	case 80:  // Cancel
		free_window(*w);
		return draw_main_window(w, c);
	case 81:  // Next / Play
		if (ok_to_start(c)) {
			if (c->MODE == TWO_PLAYERS) {
				free_panel(w->children[1]);
				panel menu;
				SDL_Rect menu_rect = create_rect(600, 0, 200, 600);
				create_game_menu(&menu, menu_rect, c);
				w->children[1] = menu;
				panel toBeFilled;
				draw_empty_panel(&toBeFilled, ZERO_RECT, c);
				w->children[2] = toBeFilled;
				w->children_len = 3;
				w->id = 'g';
				draw_panel(w->main_window, w->children[1]);
				return draw_curr_state(w, c);
			} else {
				return draw_settings_window(w, c);
			}
		}
	}
	if (67 <= event && event <= 79) { // Piece is chosen
		int i;
		for (i = 67; i <= 79; i++) { //go over pieces, see if anyone is marked
			char new_char = 'u';
			button b = w->children[1].children[i - 67];
			if (i == event) {
				new_char = 's';
				if (b.image.pathToImage[17] == 's') {
					break;
				}
			}
			if (b.image.pathToImage[17] == 's' || i == event) {
				char new_path[23];
				strcpy(new_path, b.image.pathToImage);
				new_path[17] = new_char;
				edit_button(&b, new_path);
				w->children[1].children[i - 67] = b;
				draw_button(w->main_window, w->children[1].children[i - 67]);
			}
		}
	} else if (3 <= event && event <= 66) {
		int pressed = -1;
		int i;
		button b;
		for (i = 0; i <= 12; i++) {
			b = w->children[1].children[i];
			if (b.image.pathToImage[17] == 's') {
				pressed = i;
				break;
			}
		}
		if (pressed == -1) {
			return 0;
		}
		b = w->children[1].children[pressed];
		char add_to_board = b.image.pathToImage[16];
		int color = islower(add_to_board) ? WHITE : BLACK;
		int index = event - 3;
		int col = index % BOARD_SIZE;
		int row = (index - col) / BOARD_SIZE;
		Location loc = create_location(row, col);
		if (add_to_board == 't' && b.image.pathToImage[17] == 's') {
			if (remove_by_pos(c, loc)) {
				button new_b = w->children[0].children[index];
				char new_path[23];
				return remove_piece(w, &new_b, &b, index, loc, new_path);
			}
		} else {
			int res = set_by_pos(c, loc, color, add_to_board);
			if (res == 2) {
				button new_b = w->children[0].children[index];
				char new_path[23];
				remove_piece(w, &new_b, &b, index, loc, new_path);
				new_path[16] = add_to_board;
				new_path[17] = 'u';
				edit_button(&new_b, new_path);
				w->children[0].children[index] = new_b;
				draw_button(w->main_window, w->children[0].children[index]);
			}
		}
	}
	return 0;
}

int onClick_game_window(int event, window* w, Config* c) {
	if (4 <= event && event <= 67) { // Click on board
		if (c->END_GAME == 1) {
			return 0;
		}
		SDL_Rect rect = BOARD_RECT;
		int dst_index = event - 4;
		int src_index = -1;
		int marked_index = -1;
		Location dst_loc = index_to_loc(dst_index);
		Location src_loc = create_location(-1, -1);
		Location marked_loc = create_location(-1, -1);
		int i;
		button b;
		for (i = 0; i < MAX_BUTTONS; i++) {
			b = w->children[0].children[i];
			if (b.image.pathToImage[17] == 's') { // The square's selected
				if (src_index != -1) {
					marked_loc = index_to_loc(i);
					marked_index = i;
					break;
				} else src_index = i;
				src_loc = index_to_loc(i);
			}
		}
		if (marked_index != -1) { // best_move is currently shown
			if (!is_piece_turn(BOARD(dst_loc.row,dst_loc.col),c->TURN)){
				return 0;
			}
			update_square(w, c, src_loc, BOARD(src_loc.row, src_loc.col),
					w->children[0].children[src_index]);
			update_square(w, c, marked_loc, BOARD(marked_loc.row, marked_loc.col),
					w->children[0].children[marked_index]);
			change_piece_marking(dst_loc, c, rect, w, dst_index, 1);
		} else if (src_index != -1) { // a single piece is marked now
			if (dst_index == src_index) { // a marked piece is pressed. unmark it.
				change_piece_marking(src_loc, c, rect, w, src_index, 0);
				return 0;
			}
			List* moves = init_list();
			if (moves == NULL) { // malloc failure...
				return 0;
			}
			if (!generate_legal_moves(c, moves)) { // malloc failure...
				free_list(moves);
				return 0;
			}
			Move move = create_move(src_loc, dst_loc, 'o', 0, c, 0);

			char piece = BOARD(src_loc.row, src_loc.col);
			int sign = islower(piece) ? 1 : -1;
			int promote = ((islower(piece) && src_loc.row + sign == 7)
					|| (isupper(piece) && src_loc.row + sign == 0));

			if (make_move(c, moves, move)) { // make move
				if (promote) { // Show promote panel
					panel promote_panel;
					draw_promote_panel(&promote_panel, create_rect(600, 137, 200, 309), c);
					free_panel(w->children[2]);
					w->children[2] = promote_panel;
					draw_panel(w->main_window, w->children[1]);
					draw_panel(w->main_window, w->children[2]);
					c->END_GAME = 1;
				}
				c->TURN = 1 - c->TURN;
				draw_move_made(w, c, src_index, dst_index, src_loc, dst_loc, &b,
						rect);
				draw_curr_state(w, c);
			}

		} else if (src_index == -1) { // no piece is marked right now
									  // the user is choosing a piece to make a move with
			if (is_piece_turn(BOARD(dst_loc.row, dst_loc.col), c->TURN)) {
				change_piece_marking(dst_loc, c, rect, w, dst_index, 1);
			}

		}
	}
	panel p;
	switch (event) {
	case 68: // Get Move
		if (curr_panel(w->children[2]) == 'p'){ // promote is showing
			return 0;
		} if (c->END_GAME==1){
			return 0;
		} if (c->MODE == TWO_PLAYERS) {
			draw_depths_panel(&p, create_rect(600, 137, 200, 309), c);
			free_panel(w->children[2]);
			w->children[2] = p;
			draw_panel(w->main_window, w->children[1]);
			draw_panel(w->main_window, w->children[2]);
		} else {
			display_best_move(c, w, p, 71 + c->DEPTH);
		}
		break;
	case 69: // Save Game
		if (curr_panel(w->children[2]) == 'p'){ // promote is showing
			return 0;
		}
		create_slots_panel(&p, create_rect(600, 150, 200, 310), c);
		free_panel(w->children[2]);
		w->children[2] = p;
		draw_panel(w->main_window, w->children[1]);
		draw_panel(w->main_window, w->children[2]);
		break;
	case 70: // Main Menu
		free_window(*w);
		return draw_main_window(w, c);
	case 71: // Quit
		free_window(*w);
		exit(0);
		break;
	}
	if (72 <= event && event <= 75 && curr_panel(w->children[2]) == 'p') { // Promote panel showing
		char pieces[4] = {'q', 'n', 'b', 'r'};
		char piece = (c->TURN == BLACK)? pieces[event-72] : toupper(pieces[event-72]);
		char searched_man = (c->TURN == BLACK)? 'm' : 'M';
		Location man_loc = create_location(-1, -1);
		int i;
		int row = (c->TURN == WHITE)? 0 : BOARD_SIZE - 1;
		for (i = 0; i < BOARD_SIZE; i++) {
			if (BOARD(row, i) == searched_man) {
				man_loc = create_location(row, i);
				break;
			}
		}
		BOARD(man_loc.row, man_loc.col) = (c->TURN == BLACK)? piece : toupper(piece);
		// promote man
		button man_button = w->children[0].children[loc_to_index(man_loc)];
		update_square(w, c, man_loc, piece, man_button);

		c->END_GAME = 0;
		panel empty;
		draw_empty_panel(&empty, create_rect(600, 137, 200, 309), c);
		free_panel(w->children[2]);
		w->children[2] = empty;
		draw_panel(w->main_window, w->children[1]); // Behind empty
		draw_panel(w->main_window, w->children[2]); // empty
		draw_curr_state(w, c);
	}
	if (72 <= event && event <= (72 + SLOTS_NUM - 1) && (curr_panel(w->children[2]) == 's')) { // saved games panel showing
		char path[18] = "SavedGames/x.xml\n";
		path[11] = '0' + event - 72;
		save_file(path, c);
		draw_empty_panel(&p, create_rect(600, 137, 200, 309), c);
		free_panel(w->children[2]);
		w->children[2] = p;
		draw_panel(w->main_window, w->children[1]);
		draw_panel(w->main_window, w->children[2]);
	} else if (72 <= event && event <= 76 && (curr_panel(w->children[2]) == 'd')) { // depth panel showing
		if (event == 76) {
			event--;
		}
		display_best_move(c, w, p, event);
	}
	return 1;
}

/** Show best move on the board **/
int display_best_move(Config* c, window* w, panel p, int event) {
	int first_marked_index = -1;
	int second_marked_index = -1;
	int piece_selected_index = -1;
	int i;
	button b;
	for (i = 0; i < MAX_BUTTONS; i++) {
		b = w->children[0].children[i];
		if (b.image.pathToImage[17] == 's') {
			if (b.image.pathToImage[16] == 's') {
				second_marked_index = first_marked_index != -1 ? i : -1;
				first_marked_index =
						first_marked_index != -1 ? first_marked_index : i;
			} else {
				piece_selected_index = i;
			}
		}
	}
	if (first_marked_index != -1) {
		Location loc1 = index_to_loc(first_marked_index);
		update_square(w, c, loc1, BOARD(loc1.row, loc1.col),
				w->children[0].children[first_marked_index]);
	}
	if (second_marked_index != -1) {
		Location loc2 = index_to_loc(second_marked_index);
		update_square(w, c, loc2, BOARD(loc2.row, loc2.col),
				w->children[0].children[second_marked_index]);
	}
	if (piece_selected_index != -1) {
		change_piece_marking(index_to_loc(piece_selected_index), c,
				BOARD_RECT, w, piece_selected_index, 0);
	}

	int depth = event - 71;
	List* moves = init_list();
	if (moves == NULL) { // malloc failure...
		return 0;
	}
	if (!generate_legal_moves(c, moves)) { // malloc failure...
		free_list(moves);
		return 0;
	}
	get_best_moves(c, moves, depth);
	Move move = best_move(moves);
	// Disappearing of get_move buttons
	draw_empty_panel(&p, create_rect(600, 137, 200, 309), c);
	free_panel(w->children[2]);
	w->children[2] = p;
	draw_panel(w->main_window, w->children[1]);
	draw_panel(w->main_window, w->children[2]);
	// Showing best_move on the board
	int src_index = loc_to_index(move.src);
	int dst_index = loc_to_index(move.dst);
	image opaque = create_image("graphics/pieces/ss.png");
	button src_opaque, dst_opaque;
	create_button(&src_opaque, BOARD_RECT,
			28 + move.src.col * SQUARE_SIZE,
			28 + (7 - move.src.row) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE,
			opaque);
	create_button(&dst_opaque, BOARD_RECT,
			28 + move.dst.col * SQUARE_SIZE,
			28 + (7 - move.dst.row) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE,
			opaque);
	free_button(w->children[0].children[src_index]);
	w->children[0].children[src_index] = src_opaque;
	free_button(w->children[0].children[dst_index]);
	w->children[0].children[dst_index] = dst_opaque;
	draw_button(w->main_window, w->children[0].children[src_index]);
	draw_button(w->main_window, w->children[0].children[dst_index]);
	free_list(moves);
	printf("finished display best move\n");
	return 1;
}

int onClick(int event, window *w, Config* c) {
	//printf("button_pressed = %d\n", event);
	if (event == -1) {
		return -1;
	}
	switch (w->id) {
	case 'm':
		return onClick_main_window(event, w, c);
	case 'p':
		return onClick_player_selection_window(event, w, c);
	case 's':
		return onClick_settings_window(event, w, c);
	case 'b':
		return onClick_set_board_window(event, w, c);
	case 'g':
		return onClick_game_window(event, w, c);
	}

	return 0;
}

int update_square(window* w,Config* c, Location loc, char piece, button b) {
	char path[23];
	strcpy(path, b.image.pathToImage);
	path[16] = (loc.row + loc.col) % 2 == 1 ? 'l' : 'd';
	path[17] = 'u';
	edit_button(&b, path);
	draw_button(w->main_window, b);
	free_button(b);
	button new_b = custom_button(loc, piece, 0, BOARD_RECT);
	int index = loc_to_index(loc);
	w->children[0].children[index] = new_b;
	return draw_button(w->main_window, w->children[0].children[index]);
}

Location index_to_loc(int index) {
	int col = index % BOARD_SIZE;
	int row = (index - col) / BOARD_SIZE;
	return create_location(row, col);
}

int loc_to_index(Location loc) {
	return BOARD_SIZE * loc.row + loc.col;
}

int draw_move_made(window* w, Config * c, int src_index, int dst_index,
		Location src, Location dst, button* b, SDL_Rect rect) {
	char new_path[23];
	strcpy(new_path, b->image.pathToImage);
	new_path[16] = (src.row + src.col) % 2 == 0 ? 'd' : 'l';
	new_path[17] = 'u';
	edit_button(&(w->children[0].children[src_index]), new_path);
	new_path[16] = (dst.row + dst.col) % 2 == 0 ? 'd' : 'l';
	edit_button(&(w->children[0].children[dst_index]), new_path);

	//draw empty buttons
	draw_button(w->main_window, w->children[0].children[src_index]);
	draw_button(w->main_window, w->children[0].children[dst_index]);

	//create_new location with piece button
	button b1 = custom_button(create_location(dst.row, dst.col),
			BOARD(dst.row, dst.col), 0, rect);
	free_button(w->children[0].children[dst_index]);
	w->children[0].children[dst_index] = b1;
	draw_button(w->main_window, w->children[0].children[dst_index]);
	return 0;
}

int change_piece_marking(Location dst_loc, Config * c, SDL_Rect rect,
		window * w, int index, int mark) {
	button b1 = custom_button(dst_loc, BOARD(dst_loc.row, dst_loc.col), mark,
			rect);
	free_button(w->children[0].children[index]);
	w->children[0].children[index] = b1;
	draw_button(w->main_window, w->children[0].children[index]);
	return 1;

}

int remove_piece(window * w, button * new_b, button * b, int index,
		Location loc, char new_path[23]) {
	strcpy(new_path, b->image.pathToImage);
	new_path[16] = (loc.row + loc.col) % 2 == 0 ? 'd' : 'l';
	new_path[17] = 'u';
	edit_button(new_b, new_path);
	w->children[0].children[index] = *new_b;
	return draw_button(w->main_window, w->children[0].children[index]);
}

int toggle_buttons(window* w, int panel, char* path1, char* path2,
		int child_index) {
	button left_button = w->children[panel].children[child_index];
	edit_button(&left_button, path1);
	w->children[panel].children[child_index] = left_button;

	button right_button = w->children[panel].children[child_index + 1];
	edit_button(&right_button, path2);
	w->children[panel].children[child_index + 1] = right_button;

	draw_button(w->main_window, w->children[panel].children[child_index]);
	draw_button(w->main_window, w->children[panel].children[child_index + 1]);
	return 1;
}

int toggle_diffs(window* w, char paths[5][30], int child_index) {
	int j = 0;
	while (j < 5) {
		button new_button = w->children[2].children[child_index + j];
		edit_button(&new_button, paths[j]);
		w->children[2].children[child_index + j] = new_button;
		draw_button(w->main_window, w->children[2].children[child_index + j]);
		j++;
	}
	return 1;
}

int panel_pressed(SDL_Event mouseMotion, panel p) {
	return (p.rect.x <= mouseMotion.motion.x)
			&& (mouseMotion.motion.x <= p.rect.x + p.rect.w)
			&& (p.rect.y <= mouseMotion.motion.y)
			&& (mouseMotion.motion.y <= p.rect.y + p.rect.h);
}

int button_pressed(SDL_Event mouseMotion, button b) {
	return (b.rect.x <= mouseMotion.motion.x)
			&& (mouseMotion.motion.x <= b.rect.x + b.rect.w)
			&& (b.rect.y <= mouseMotion.motion.y)
			&& (mouseMotion.motion.y <= b.rect.y + b.rect.h);
}

int find_panel(SDL_Event mouseMotion, window w) {
	panel child;
	int i = 0;
	int j = w.children_len + 1;
	for (child = w.children[i]; i < w.children_len; child = w.children[++i]) {
		if (panel_pressed(mouseMotion, child)) {
//			printf("panel pressed is x=%d,y=%d,h=%d,w=%d\n",child.rect.x,child.rect.y,child.rect.h,child.rect.w);
			int res = find_button(mouseMotion, child, j);
			if (res != -1) {
				return res;
			} else {
				j += child.children_len;
			}
		} else {
			j += child.children_len;
		}

	}
	return -1;
}

int find_button(SDL_Event mouseMotion, panel p, int j) {
	button child;
	int i = 0;
	for (child = p.children[i]; i < p.children_len; child = p.children[++i]) {
		if (button_pressed(mouseMotion, child)) {
			return j + i;
		}
	}
	return -1;

}

char curr_panel(panel p) {
	if (p.children_len == 0) return 'r';
	if (tolower(p.children[0].image.pathToImage[16]) == 'q') return 'p';
	return tolower(p.children[0].image.pathToImage[9]); // First char of imagePath is a unique identifier of its parent panel p;
}

/* This function may run in a separate event thread */
int FilterEvents(const SDL_Event *event) {

	/* This quit event signals the closing of the window */
	if (event->type == SDL_MOUSEMOTION) {
//		printf("Mouse moved to (%d,%d)\n", event->motion.x,
//				event->motion.y);
		return (0); /* Drop it, we've handled it */
	}
	return (1);
}

/** Free Functions **/
int free_window(window w) {
	SDL_FreeSurface(w.main_window);
	panel child;
	int i = 0;
	for (child = w.children[i]; i < w.children_len; child = w.children[++i]) {
		free_panel(child);
	}
	return 1;
}

int free_panel(panel p) {
	SDL_FreeSurface(p.panel_window);
	button child;
	int i = 0;
	for (child = p.children[i]; i < p.children_len; child = p.children[++i]) {
		free_button(child);
	}
	return 1;
}

int free_button(button b) {
	SDL_FreeSurface(b.button_window);
	return 1;
}
