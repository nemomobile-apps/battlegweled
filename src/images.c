#include <stdlib.h>
#include <time.h>

#include "level.h"
#include "images.h"

int draw = true;
SDL_Surface *p1l[5], *p1r[5], *p2l[5], *p2r[5];
SDL_Surface *back, *waiting, *diamond[DIAMONDS], *number;
SDL_Surface *screen, *bg, *logo, *selection, *bar, *win, *lose, *bg_single;

// Update sync screen
void SDL_Update(int x, int y, int w, int h) {
    if (!SDL_mutexP(mutex)) {
        SDL_UpdateRect(screen, x, y, w, h);
        SDL_mutexV(mutex);
    }
}

// Load image in video memory
SDL_Surface *load_image(char *filename, int transparent) {
    char filepath[256];
    SDL_Surface *img, *img2;
    
    // Load file image
    sprintf(filepath, "%s%s", PIXMAPSPREFIX, filename);
    img = SDL_LoadBMP(filepath);
    if (!img) {
        fprintf(stderr, "File not found: %s\n", filepath);
        return 0;
    }
    
    // Create hardware surface
    img2 = SDL_CreateRGBSurface(SDL_HWSURFACE | (transparent ? SDL_SRCCOLORKEY :
        0), img->w, img->h, 16, 0xF800, 0x7E0, 0x1F, 0);
    if (!img2) {
        SDL_FreeSurface(img);
        fprintf(stderr, "Error creating surface!\n");
        return 0;
    }
    
    // Set color key
    if (transparent) {
        SDL_SetColorKey(img2, SDL_SRCCOLORKEY, 0xF81F);
    }
    SDL_SetAlpha(img2, 0, 0);
    
    // Copy surface
    SDL_BlitSurface(img, NULL, img2, NULL);
    SDL_FreeSurface(img);
    
    return img2;    
}

/**
**********************************************************
* @brief Load surfaces
* 
* This function loads all the game surfaces
* 
* @return 			1 if the images are loaded
* 					0 if some problem occurs
* @todo     change the selection image name
**********************************************************/
int load_surfaces() {
	int i;
	char filename[128];
	
    // Load game logo
    logo = load_image("logo.bmp", true);
    if (!logo) {
        return false;
    }

	// Load background
    bg = load_image("bg_board.bmp", false);
	if (!bg)  {
		return false;
	}
	
    // Load selection
    selection = load_image("selection.bmp", true);
    if (!selection)  {
        return false;
    }

	// Load diamonds
    for (i = 0; i < DIAMONDS; i++) {
		sprintf(filename, "diamond%d.bmp", i);
        diamond[i] = load_image(filename, true);
		if (!diamond[i]) {
			return false;
		}
	}

    // Load back button
    back = load_image("back.bmp", true);
    if (!back) {
        return false;
    }
    
    // Load selection
    bar = load_image("bar_full.bmp", true);
    if (!bar) {
        return false;
    }

    // Load message waiting
    waiting = load_image("waiting.bmp", true);
    if (!waiting) {
        return false;
    }

    // Load message win
    win = load_image("win.bmp", true);
    if (!win) {
        return false;
    }

    // Load message lose
    lose = load_image("lose.bmp", true);
    if (!lose) {
        return false;
    }

    // Load numbers
    number = load_image("num.bmp", true);
    if (!number) {
        return false;
    }

    // Load single background
    bg_single = load_image("bg_singleplayer.bmp", false);
    if (!bg_single) {
        return false;
    }

    // Load players
    for (i = 0; i < 5; i++) {
	    sprintf(filename, "p1l%d.bmp", i);
	    p1l[i] = load_image(filename, true);
	    if (!p1l[i]) return false;
	    sprintf(filename, "p1r%d.bmp", i);
	    p1r[i] = load_image(filename, true);
	    if (!p1r[i]) return false;
	    sprintf(filename, "p2l%d.bmp", i);
	    p2l[i] = load_image(filename, true);
	    if (!p2l[i]) return false;
	    sprintf(filename, "p2r%d.bmp", i);
	    p2r[i] = load_image(filename, true);
	    if (!p2r[i]) return false;
    }

    return true;
}

/**
**********************************************************
* @brief Free surfaces
* 
* This function frees all the game surfaces
* 
**********************************************************/
void free_surfaces() {
	int i;
	
    for (i = 0; i < 5; i++) {
	    SDL_FreeSurface(p1l[i]);
	    SDL_FreeSurface(p1r[i]);
	    SDL_FreeSurface(p2l[i]);
	    SDL_FreeSurface(p2r[i]);
    }
    SDL_FreeSurface(bg_single);
    SDL_FreeSurface(number);
    SDL_FreeSurface(lose);
    SDL_FreeSurface(win);
    SDL_FreeSurface(bar);
    SDL_FreeSurface(back);
    SDL_FreeSurface(waiting);
    for (i = 0; i < DIAMONDS; i++) {
        SDL_FreeSurface(diamond[i]);
    }
    SDL_FreeSurface(selection);
    SDL_FreeSurface(bg);
    SDL_FreeSurface(logo);
}

// Update power bar
void update_powerbar() {
    SDL_Rect src, dest;
    
    src.w = combo_score;
    src.h = 36;
    dest.x = 21;
    dest.y = 335;
    src.x = src.y = dest.w = dest.h = 0;
    SDL_BlitSurface(bar, &src, screen, &dest);
    SDL_Update(dest.x, dest.y, 311, 36);
}

// Draw animate background
void draw_background() {
    SDL_Rect dest;
    int i, j, color;
    static int posx = 0, posy = 0;
    
    // Draw each tile
    color = SDL_MapRGB(screen->format, 73, 57, 54);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 36, 19, 17));
    for (j = 0; j < 10; j++) {
        for (i = 0; i < 13; i++) {
            if ((i + j) & 1) {
                dest.y = j * 70 + posy;
                dest.x = i * 70 + posx;
                dest.w = dest.h = 70;
                SDL_FillRect(screen, &dest, color);
            }
        }
    }

    // Update positions
    posy -= 4;
    if (posy <= -70) posy = 0;
    posx -= 4;
    if (posx <= -70) posx = 0;
}

// Draw screen menu
void draw_waiting_screen() {
    SDL_Rect dest;

    // Draw background
    draw_background();
    
    // Draw logo
    dest.w = dest.h = 0;
    dest.x = 51;
    dest.y = 20;
    SDL_BlitSurface(logo, NULL, screen, &dest);

    dest.x = 236;
    dest.y = 217;
    SDL_BlitSurface(waiting, NULL, screen, &dest);
    
    // Draw back button
    dest.x = BACK_OFFSETX;
    dest.y = BACK_OFFSETY;
    SDL_BlitSurface(back, NULL, screen, &dest);

    // Update screen
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/**
**********************************************************
* @brief Draws the board game
* 
* This function draws the board game beginig in column x
* until x+w column and beginig in row h until pos
* 
* @param x 			     Initial column
* @param w 			     Column width
* @param h 			     Row height
* @param pos 		     Offset Position y
* @param update_screen   Informs if the screen should be updated   
* 
**********************************************************/
void draw_board(int x, int w, int h, int pos, int update_screen) {
	int i, j;
	SDL_Rect dest, dest2;
	
	// Draw background
	dest.w = w * DIAMOND_WIDTH;
	dest.h = h * DIAMOND_HEIGHT;
	dest.x = x * DIAMOND_WIDTH + BOARD_OFFSETX;
	dest.y = BOARD_OFFSETY;
	SDL_BlitSurface(bg, &dest, screen, &dest);

	// Draw diamonds
	dest2.w = dest2.h = 0;
	for (j = 0; j < h; j++) {
		for (i = x; i < x + w; i++) {
			dest2.x = i * DIAMOND_WIDTH + BOARD_OFFSETX;
            dest2.y = j * DIAMOND_HEIGHT + BOARD_OFFSETY - pos;
			SDL_BlitSurface(diamond[matrix[j][i]], NULL, screen, &dest2);
		}
	}

	// Draw background
	dest2.x = BOARD_OFFSETX;
	dest2.y = 0;
	dest2.w = 8 * DIAMOND_WIDTH;
	dest2.h = BOARD_OFFSETY;
	SDL_BlitSurface(bg, &dest2, screen, &dest2);
	
	// Update screen
    if (update_screen) {
	SDL_Update(dest2.x, dest2.y, dest2.w, dest2.h);
    	SDL_Update(dest.x, dest.y, dest.w, dest.h);
    }
}

/**
**********************************************************
* @brief Draws the screen
* 
* This function draws the game screen
* 
* 
**********************************************************/
int draw_screen(void) {
    SDL_Rect dest;
    Alignment alignment;
    int moved = false, combo;

    dest.w = dest.h = 0;
    if (draw) {
        draw = false;
        switch (game_state.state) {
            case IDLE:
            case UNSELECTED_FIRST:
                draw_board(0, BOARD_WIDTH, BOARD_HEIGHT, 0, true);
                break;
    
            case SELECTED_FIRST:
                draw_board(0, BOARD_WIDTH, BOARD_HEIGHT, 0, false);     
                dest.x = game_state.x_first * DIAMOND_WIDTH + BOARD_OFFSETX;
                dest.y = game_state.y_first * DIAMOND_HEIGHT + BOARD_OFFSETY;
                SDL_BlitSurface(selection, NULL, screen, &dest);
                SDL_Update(BOARD_OFFSETX, BOARD_OFFSETY, 
                               DIAMOND_WIDTH*BOARD_WIDTH, DIAMOND_HEIGHT*BOARD_HEIGHT);
                break;
    
            case SELECTED_SECOND:
                changing_location();
                //moving vertically
                if (game_state.x_first == game_state.x_second) {
                    if(!(gweled_is_part_of_an_alignment(game_state.x_first, game_state.y_first)
                        || gweled_is_part_of_an_alignment(game_state.x_second, game_state.y_second))){
                        changing_location();
                        game_state.state=IDLE;
                        draw = true;
                    } else {
                        game_state.state = MOVING;
                        draw = true;
                    }
                }
                //moving horizontally
                else {
                    if(!(gweled_is_part_of_an_alignment(game_state.x_first, game_state.y_first)
                        || gweled_is_part_of_an_alignment(game_state.x_second, game_state.y_second))){
                        changing_location();
                        game_state.state=IDLE;
                        draw = true;
                    } else {
                        game_state.state = MOVING;
                        draw = true;
                    }
                }
                break;
    
            case MOVING:
                combo = 1;
                while (gweled_check_for_alignment(&alignment)){
                    gweled_remove_gems_and_update_score(&alignment, combo);
                    gweled_refill_board(&alignment);
                    gweled_gems_fall_into_place(&alignment);
                    combo++;
                }
                if (!gweled_check_for_moves_left(NULL, NULL)) {
                    game_state.state = NEW_GAME;
                    draw = true;
                } else {
                    game_state.state=IDLE;
                    draw = true;
                }
                moved = true;
                break;
        }
    }/* else {
        SDL_Delay(5);
    } */
    
    return moved;
}
/**
**********************************************************
* @brief Draws the gems changing location
* 
* This function draws gems changing location
* 
* 
**********************************************************/
void changing_location(){

    SDL_Event event;
    int i, min, max,temp;
    SDL_Rect dest, dest_gem;
   //moving vertically
   if (game_state.x_first == game_state.x_second) {
        min = game_state.y_first > game_state.y_second ? game_state.y_second : game_state.y_first;
        max = game_state.y_first < game_state.y_second ? game_state.y_second : game_state.y_first;
        dest.x = game_state.x_first*DIAMOND_WIDTH +BOARD_OFFSETX;
        dest.w = DIAMOND_WIDTH;
        dest.y = min*DIAMOND_HEIGHT+BOARD_OFFSETY;
        dest.h = 2*DIAMOND_HEIGHT;
        dest_gem.x = dest.x;
        dest_gem.w = DIAMOND_WIDTH;
        dest_gem.h = DIAMOND_HEIGHT;
        for(i=0; i<=DIAMOND_HEIGHT; i += 3){
            SDL_BlitSurface(bg, &dest, screen, &dest);
            dest_gem.y = i+min*DIAMOND_HEIGHT+BOARD_OFFSETY;
            SDL_BlitSurface(diamond[matrix[min][game_state.x_second]], NULL, screen, &dest_gem);
            dest_gem.y = max*DIAMOND_HEIGHT+BOARD_OFFSETY-i;
            SDL_BlitSurface(diamond[matrix[max][game_state.x_first]], NULL, screen, &dest_gem);

            //drawing the selection
            dest_gem.w = 0;
            dest_gem.h = 0;
            dest_gem.x = game_state.x_first * DIAMOND_WIDTH + BOARD_OFFSETX;
            dest_gem.y = game_state.y_first * DIAMOND_HEIGHT + BOARD_OFFSETY;
            SDL_BlitSurface(selection, NULL, screen, &dest_gem);

            dest_gem.y = game_state.y_second * DIAMOND_HEIGHT + BOARD_OFFSETY;
            SDL_BlitSurface(selection, NULL, screen, &dest_gem);
            SDL_Update(dest.x, dest.y,dest.w,dest.h);
	    update_player();
	    time_tick();
        }
        temp=matrix[max][game_state.x_first];
        matrix[max][game_state.x_first]=matrix[min][game_state.x_second];
        matrix[min][game_state.x_second]=temp;
    }
    //moving horizontally
    else {
        min = game_state.x_first > game_state.x_second ? game_state.x_second : game_state.x_first;
        max = game_state.x_first < game_state.x_second ? game_state.x_second : game_state.x_first;
        dest.x = min*DIAMOND_WIDTH +BOARD_OFFSETX;
        dest.w = 2*DIAMOND_WIDTH;
        dest.y = game_state.y_first*DIAMOND_HEIGHT+BOARD_OFFSETY;
        dest.h = DIAMOND_HEIGHT;
        dest_gem.y = dest.y;
        dest_gem.w = DIAMOND_WIDTH;
        dest_gem.h = DIAMOND_HEIGHT;
        for(i=0; i<=DIAMOND_HEIGHT; i += 3){
            SDL_BlitSurface(bg, &dest, screen, &dest);
            dest_gem.x = i+min*DIAMOND_WIDTH+BOARD_OFFSETX;
            SDL_BlitSurface(diamond[matrix[game_state.y_second][min]], NULL, screen, &dest_gem);
            dest_gem.x = max*DIAMOND_WIDTH+BOARD_OFFSETX-i;
            SDL_BlitSurface(diamond[matrix[game_state.y_first][max]], NULL, screen, &dest_gem);

            //drawing the selection
            dest_gem.w = 0;
            dest_gem.h = 0;
            dest_gem.x = game_state.x_first * DIAMOND_WIDTH + BOARD_OFFSETX;

            dest_gem.y = game_state.y_first * DIAMOND_HEIGHT + BOARD_OFFSETY;
            SDL_BlitSurface(selection, NULL, screen, &dest_gem);

            dest_gem.x = game_state.x_second * DIAMOND_WIDTH + BOARD_OFFSETX;
            SDL_BlitSurface(selection, NULL, screen, &dest_gem);
            SDL_Update(dest.x, dest.y,dest.w,dest.h);
	    update_player();
	    time_tick();
        }
        temp=matrix[game_state.y_first][max];
        matrix[game_state.y_first][max]=matrix[game_state.y_second][min];
        matrix[game_state.y_second][min]=temp;
    }
    while (SDL_PollEvent(&event));
}

// Draw score in single player
void draw_score(int update) {
    SDL_Rect src, dest;
    char num[16], *b = num;
    
    // Update background
    src.x = SCOREX - SINGLEBGX;
    src.y = SCOREY - SINGLEBGY;
    src.w = SCORE_WIDTH * 6;
    src.h = SCORE_HEIGHT;
    dest.x = SCOREX;
    dest.y = SCOREY;
    dest.w = dest.h = 0;
    SDL_BlitSurface(bg_single, &src, screen, &dest);

    // Offsets
    src.y = 0;
    src.w = SCORE_WIDTH;
    src.h = SCORE_HEIGHT;
    dest.x = SCOREX;
    dest.y = SCOREY;
    dest.h = dest.w = 0;
    sprintf(num, "%d", total_score);

    // Draw each number
    while (*b) {
        src.x = (*b - '0') * SCORE_WIDTH;
        if (*b == '1') dest.x -= 4;
        SDL_BlitSurface(number, &src, screen, &dest);
        dest.x += SCORE_WIDTH - (*b == '1' ? 4 : 0);
        b++;
    }
    if (update) SDL_Update(SCOREX, SCOREY, SCORE_WIDTH * 6, SCORE_HEIGHT);
}

// Draw single timer bar
void draw_timer_bar(int update) {
	SDL_Rect src, dest;
       
	// Update background
	src.x = TIMER_OFFSETX - SINGLEBGX;
	src.y = TIMER_OFFSETY - SINGLEBGY;
	src.w = TIMER_WIDTH;
	src.h = TIMER_HEIGHT;
	dest.x = TIMER_OFFSETX;
	dest.y = TIMER_OFFSETY;
	dest.w = dest.h = 0;
	SDL_BlitSurface(bg_single, &src, screen, &dest);

	// Draw timer bar
	src.x = src.y = 0;
	src.w = single_timer;
	SDL_BlitSurface(bar, &src, screen, &dest);
	if (update) SDL_Update(TIMER_OFFSETX, TIMER_OFFSETY, TIMER_WIDTH, TIMER_HEIGHT);
}

SDL_Rect p1lr[5] = {{60,204,0,0},{60,203,0,0},{62,224,0,0},{78,204,0,0},{48,271,0,0}};
SDL_Rect p1rr[5] = {{226,204,0,0},{225,203,0,0},{228,224,0,0},{186,204,0,0},{214,271,0,0}};
SDL_Rect p2lr[5] = {{71,216,0,0},{71,217,0,0},{66,216,0,0},{91,218,0,0},{56,271,0,0}};
SDL_Rect p2rr[5] = {{237,216,0,0},{236,217,0,0},{232,216,0,0},{186,218,0,0},{222,271,0,0}};

// Draw players
void draw_player() {
	SDL_Rect src; //, dest, dest2;

	if (gm != GM_MULTIPLAYER) return;

	src.x = 48;
	src.y = 203;
	src.w = 252;
	src.h = 97;
	SDL_BlitSurface(bg, &src, screen, &src);
	if (game_state.server) {
		SDL_BlitSurface(p1l[game_state.left_img], NULL, screen, &p1lr[game_state.left_img]);
		SDL_BlitSurface(p2r[game_state.right_img], NULL, screen, &p2rr[game_state.right_img]);
	} else {
		SDL_BlitSurface(p2l[game_state.left_img], NULL, screen, &p2lr[game_state.left_img]);
		SDL_BlitSurface(p1r[game_state.right_img], NULL, screen, &p1rr[game_state.right_img]);
	}
	SDL_Update(src.x, src.y, src.w, src.h);
}

void update_player() {
	if (gm != GM_MULTIPLAYER) return;
	game_state.attack_cnt--;
	if (!game_state.attack_cnt) {
		game_state.attack_cnt = 32;
		if (!game_state.left_img) game_state.left_img = game_state.right_img = 1;
		else game_state.left_img = game_state.right_img = 0;
		draw_player();
	}
}

void start_attack(int left) {
	game_state.attack_cnt = 75;
	if (left) {
		game_state.left_img = 3;
		game_state.right_img = 2;
	} else {
		game_state.left_img = 2;
		game_state.right_img = 3;
	}
	draw_player();
}

void finish_attack(int left) {
	if (left) {
		game_state.left_img = 3;
		game_state.right_img = 4;
	} else {
		game_state.left_img = 4;
		game_state.right_img = 3;
	}
	draw_player();
}
