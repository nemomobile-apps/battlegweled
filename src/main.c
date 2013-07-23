#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <libosso.h>
#include "level.h"
#include "images.h"
#include "socket.h"

#if HGW_FUNC
#include <hgw/hgw.h>
HgwContext *hgw_context = NULL;
#endif
#include "callbacks.h"

osso_context_t * osso_context = NULL;
GAMEMODE gm;
SDL_mutex *mutex;
GameState game_state;

// Main loop the game
int game_loop() {
    SDL_Rect dest;
    int x, y, aux;
    SDL_Event event;
    unsigned long t1, t2;
    int no_unselect = 0;

    // Main loop
    t1 = SDL_GetTicks();
    while (1) {

        // Restart the game
        if (game_state.state == NEW_GAME){
            if (gm == GM_MULTIPLAYER) {
                aux = BOARD_RESTARTED;
                write_socket(&aux, sizeof(int));
                new_game(false, GM_MULTIPLAYER, false);
            } else {
                new_game(false, GM_SINGLE, false);
            }
        } else if (game_state.state == GAME_LOSE || game_state.state == GAME_WIN || game_state.state == GAME_EXIT) {
	    if (gm == GM_MULTIPLAYER) {
		if (game_state.state == GAME_EXIT) {
			aux = END_GAME; 
        	        write_socket(&aux, sizeof(int));
		}
		finish_attack(game_state.state == GAME_WIN);
	    }
            dest.x = 386;
            dest.y = 151;
            dest.w = dest.h = 0;
            if (game_state.state != GAME_EXIT) {
                SDL_BlitSurface(game_state.state == GAME_WIN ? win : lose, NULL, screen, &dest);
                SDL_Update(386, 151, 365, 178);
                SDL_Delay(4000);
            }
            return (game_state.state == GAME_LOSE);
        }
        
        if (SDL_PollEvent(&event)) {

            
            switch (event.type) { 
                // Key pressed
                case SDL_KEYDOWN:
                    if (event.key.state == SDL_PRESSED) {

                        // Quit
                        if (event.key.keysym.sym == SDLK_ESCAPE
                                || event.key.keysym.sym == SDLK_BACKSPACE
                                || event.key.keysym.sym == SDLK_F4
                                || event.key.keysym.sym == SDLK_F5 
                                || event.key.keysym.sym == SDLK_F6) {
		            game_state.state = GAME_EXIT;
                        }
                    }
                
                // Quit the game
                case SDL_QUIT:
		    game_state.state = GAME_EXIT;
                    break;

		// Window focus change
		case SDL_ACTIVEEVENT:
		    if (event.active.gain == 0 && gm != GM_MULTIPLAYER) {
		        game_state.state = GAME_EXIT;
		    }
		    break;
                    
                // Button pressed
                case SDL_MOUSEBUTTONUP:
                    no_unselect = 1;
                    if (game_state.state == UNSELECTED_FIRST) {
                        game_state.state = IDLE;
                        break;
                    }
                case SDL_MOUSEBUTTONDOWN:
    
                    // Diamond area
                    if (event.button.x >= BOARD_OFFSETX 
                       && event.button.x < BOARD_WIDTH * DIAMOND_WIDTH + BOARD_OFFSETX 
                       && event.button.y >= BOARD_OFFSETY 
                       && event.button.y < BOARD_HEIGHT * DIAMOND_HEIGHT + BOARD_OFFSETY) {
                        
                        x = (event.button.x - BOARD_OFFSETX) / DIAMOND_WIDTH;
                        y = (event.button.y - BOARD_OFFSETY) / DIAMOND_HEIGHT;
                        
                        if (game_state.state == IDLE) {
                            game_state.state = SELECTED_FIRST;
                            game_state.x_first = x;
                            game_state.y_first = y;
                            draw = true;
                        }
                        else if (game_state.state == SELECTED_FIRST) {
                            if(game_state.x_first == x && game_state.y_first == y && !no_unselect) {
                                game_state.state = UNSELECTED_FIRST;
                                draw = true;
                            } else if ((abs(game_state.x_first - x) == 1
                                     && (game_state.y_first == y))
                                     || ((game_state.x_first == x)
                                     && (abs(game_state.y_first - y) == 1))){
                                game_state.state = SELECTED_SECOND;
                                game_state.x_second = x;
                                game_state.y_second = y;
                                draw = true;
                            } else  {
                                game_state.x_first = x;
                                game_state.y_first = y;
                                draw = true;
                            }
                        }
                    } else if (event.button.x >= BACK2_OFFSETX
                            && event.button.y >= BACK2_OFFSETY
                            && event.button.x < BACK2_OFFSETX2
                            && event.button.y < BACK2_OFFSETY2) {
			game_state.state = GAME_EXIT;
                        break;
                    }
                    no_unselect = 0;
                    break;
            }
        }
        
        // Update screen and reset timer
	time_tick();
        if (draw_screen()) {
            t1 = SDL_GetTicks();
        }

        // Calculate time and send attack for multiplayer
        if (gm == GM_MULTIPLAYER) {
		update_player();
            if (ss == SS_CLOSE) {
                return 0;
            } else {
                t2 = SDL_GetTicks();
    	        if (t2 - t1 >= 3500 || combo_score > 310) {
    	            t1 = t2;
                    total_score += combo_score;

                    // Update life
		    if (combo_score) {
			    if (total_score >= 1400) {
				    total_score = 1400;
				    game_state.state = GAME_WIN;
			    } else start_attack(1);
	                    dest.x = LIFE_ENEMYX - total_score / 10;
	                    dest.y = LIFEY;
        	            dest.w = total_score / 10;
                	    dest.h = LIFE_HEIGHT;
	                    SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 255, 0, 0));
        	            SDL_Update(LIFE_ENEMYX - LIFE_WIDTH, LIFEY, LIFE_WIDTH, LIFE_HEIGHT);
			    write_socket(&combo_score, sizeof(int));
	    	            combo_score = 0;

        	            // Erase progress bar
                	    dest.x = 21;
	                    dest.y = 335;
        	            dest.w = 311;
                	    dest.h = 36;
	                    SDL_BlitSurface(bg, &dest, screen, &dest);
        	            SDL_Update(dest.x, dest.y, dest.w, dest.h);
		    }
    	        }
            }

	// Timer for single player
	} else {
		t2 = SDL_GetTicks();
		if (t2 - t1 >= timer_delay) {
			t1 = t2;
			single_timer -= 2;
			if (single_timer <= 0) game_state.state = GAME_LOSE;
			draw_timer_bar(true);
		}
	}

    }
}

// Show and loop main menu
void show_menu_loop() {
    char *ip;
    FILE *han;
    SDL_Event event;
    int createserver;
    SDL_Thread *ReadThread;
    GConfClient *gcc = NULL;

    // Init GConf
    g_type_init();
    gcc = gconf_client_get_default();
    createserver = gconf_client_get_bool(gcc, BATTLEGWELED_CREATESERVER, NULL);
    ip = gconf_client_get_string(gcc, BATTLEGWELED_SERVERIP, NULL);

    // Single player
    if (!createserver && !strcmp(ip, "")) {
	han = fopen("/tmp/.battlegweled-save", "rb");
	if (han) {
		fread(&total_score, sizeof(int), 1, han);
		fread(&single_timer, sizeof(int), 1, han);
		fread(&timer_delay, sizeof(int), 1, han);
		fread(&score, sizeof(int), 1, han);
       		fread(nb_of_tiles, sizeof(nb_of_tiles), 1, han);
       		fread(matrix, BOARD_WIDTH * BOARD_HEIGHT, sizeof(int), han);
       		fclose(han);
       		new_game(true, GM_SINGLE, true);
    	} else new_game(true, GM_SINGLE, false); 
       	if (game_loop()) {
		flush_callback(0);
		quit_callback(0);
	} else exit_callback(0);
    } else {
	    int joined;
	    if (createserver) create_game();
	    else joined = join_game(ip);

            while (1) {

	        // If connected then start the game
	        if (ss == SS_CONNECTED) {
	            new_game(true, GM_MULTIPLAYER, false);
	            ReadThread = SDL_CreateThread(multi_player_loop, NULL);
	            game_loop();
	            SDL_KillThread(ReadThread);
		    break;
                }

		if (ss == SS_ERROR || (SDL_PollEvent(&event) && ((event.key.state == SDL_PRESSED && (event.key.keysym.sym == SDLK_ESCAPE
			|| event.key.keysym.sym == SDLK_F4 || event.key.keysym.sym == SDLK_F5 || event.key.keysym.sym == SDLK_F6))
			|| event.type == SDL_QUIT || (event.type == SDL_MOUSEBUTTONDOWN && event.button.x >= BACK_OFFSETX
			&& event.button.y >= BACK_OFFSETY && event.button.x < BACK_OFFSETX2 && event.button.y < BACK_OFFSETY2)))) {
			if (ThreadConnect) SDL_KillThread(ThreadConnect);
                        if (ThreadAccept) SDL_KillThread(ThreadAccept);
			break;
                }

		// Update screen
        	draw_waiting_screen();
        }
	quit_callback(0);
    }
}

// Main function
int main(int argc, char **argv) {

#if HGW_FUNC
	hgw_context = hgw_context_compat_init(argc, argv);
	if (!hgw_context) {
		fprintf(stderr, "Cannot init hildon-games-startup!\n");
		return 0;
	}
	hgw_compat_set_cb_exit(hgw_context, exit_callback);
	hgw_compat_set_cb_quit(hgw_context, quit_callback);
	hgw_compat_set_cb_flush(hgw_context, flush_callback);
    if(!hgw_context_compat_check(hgw_context)) return 0;

	/* hildon-games-wrapper part */
	hgw_msg_compat_receive(hgw_context, 0);
	usleep(100);
#endif

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
	    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    	return 0;
  	}

	// Initialize video
    screen = SDL_SetVideoMode(800, 480, 16, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN);
	if (screen == NULL) {
		fprintf(stderr, "Unable to set 800x480 video: %s\n", SDL_GetError());
		return 0;
	}
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Battle Gweled", NULL);

	// Load images
	if (!load_surfaces()) {
		free_surfaces();
		SDL_Quit();
		fprintf(stderr, "Unable to load images!\n");
		return 0;
	}

	// Initialize game
    srand(time(NULL));
    mutex = SDL_CreateMutex();
    show_menu_loop();
    SDL_DestroyMutex(mutex);

#if HGW_FUNC
        if (game_state.state == GAME_LOSE)
		hgw_context_destroy(hgw_context, HGW_BYE_INACTIVE);
	else
		hgw_context_destroy(hgw_context, HGW_BYE_PAUSED);

#endif

	// Finalize game
	free_surfaces();
	SDL_Quit();
	return 0;
}
