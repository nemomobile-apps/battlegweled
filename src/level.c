#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "level.h"
#include "images.h"
#include "socket.h"

int single_timer, timer_delay = 1000, score = 10;
int nb_of_tiles[DIAMONDS];
int matrix[BOARD_HEIGHT][BOARD_WIDTH];
int total_score, combo_score, enemy_score;
int diamond_bit[DIAMONDS+1] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

void time_tick() {
	int time;
	static int time_prev = 0;

	while (SDL_GetTicks() - time_prev < 10) SDL_Delay(1);
	time_prev = SDL_GetTicks();
}

/**
**********************************************************
* @brief Selects a Gem to put in the matrix
* 
* This function selects a Gem, choosing the less user gem
* in order to equaly distribute
* 
* @return           The Id of the Gem
* 
**********************************************************/
int get_new_tile (void)
{
    int i;
    int min, max, min_index, max_index;

    min_index = 0;
    max_index = 0;
    min = nb_of_tiles[0];
    max = nb_of_tiles[0];
    for (i = 0; i < DIAMONDS; i++) {
        if (nb_of_tiles[i] < min) {
            min = nb_of_tiles[i];
            min_index = i;
        }
        if (nb_of_tiles[i] > max) {
            max = nb_of_tiles[i];
            max_index = i;
        }
    }
    if (rand() & 1) { 
        return ((rand() % (DIAMONDS-1)) + max_index + 1) % DIAMONDS;
    } else {
        return (min_index);
    }
}

// Initialize board
void initialize_board(void) {
    int i, j;
    
    // Create a board with solution
    do {

        // Clear board
        memset(nb_of_tiles, 0, DIAMONDS*sizeof(int));
        for (j = 0; j < BOARD_HEIGHT; j++)  {
            for (i = 0; i < BOARD_WIDTH; i++) {
                matrix[j][i] =  DIAMONDS;  
            }
        }    
        
        // Board initialization 
        for (j = 0; j < BOARD_HEIGHT; j++)  {
            for (i = 0; i < BOARD_WIDTH; i++) {
                do {
                    matrix[j][i] = get_new_tile();
                } while (gweled_is_part_of_an_alignment(i,j));           
                nb_of_tiles[matrix[j][i]]++;
            }
        }
        
    } while (!gweled_check_for_moves_left(NULL, NULL));
}

/**
**********************************************************
* @brief Checks for alignment
* 
* This function verifies if the gem at position x,y is part
* of an alignment, if there is at least two identical gems
* at it side
* 
* @param x          x of the tested position
* @param y          y of the tested position
* 
* @return           0 if there is no alignment
*                   HORIZONTAL (1) if there is an horizontal alignment
*                   VERTICAL (2) if there is a vertical alignment
**********************************************************/
int gweled_is_part_of_an_alignment(int x, int y) {
	int i, result = 0;
	
	for (i = x - 2; i <= x; i++) {
		if (i >= 0 && i + 2 < BOARD_WIDTH && matrix[y][i]!=DIAMONDS) {
			if (diamond_bit[matrix[y][i]] &
			     diamond_bit[matrix[y][i + 1]] &
			     diamond_bit[matrix[y][i + 2]]) {
				result |= HORIZONTAL;	// is part of an horizontal alignment
				break;
			}
        }
    }
	for (i = y - 2; i <= y; i++) {
		if (i >= 0 && i + 2 < BOARD_HEIGHT && matrix[i][x]!=DIAMONDS) {
    		if (diamond_bit[matrix[i][x]] &
    		     diamond_bit[matrix[i + 1][x]] &
    		     diamond_bit[matrix[i + 2][x]]) {
    				result |= VERTICAL;	// is part of a vertical alignment
    				break;
    		}
        }        
    }
	return result;
}

/**
**********************************************************
* @brief Swap two gems
* 
* This function swap two gem positions
* 
* @param x1         x of the first gem
* @param y1         y of the first gem
* 
* @param x2          x of the second gem
* @param y2          y of the second gem

**********************************************************/
void swap_gems(int x1, int y1, int x2, int y2){
    int temp;

    temp = matrix[y1][x1];
    matrix[y1][x1] = matrix[y2][x2];
    matrix[y2][x2] = temp;
}

/**
**********************************************************
* @brief Check if there is any possible move
* 
* This function verifies if the player can still do a move
* 
* @param x          x of the possible move position
* @param y          y of the possible move position
* 
* @return           true if there is at least one move
*                   false if there is no move
**********************************************************/
int gweled_check_for_moves_left(int *pi, int *pj) {

	int i, j;
    int move_found = false;
    	
	for (j = BOARD_HEIGHT - 1; j >= 0 && move_found != true; j--) {
		for (i = BOARD_WIDTH - 1; i >= 0 && move_found != true; i--) {
			if (i > 0) {
                swap_gems(i - 1, j, i, j);
				if (gweled_is_part_of_an_alignment(i, j)) {
					move_found = true;
				}
                swap_gems(i - 1, j, i, j);
			}
			if (i < 7 && move_found != true) {
                swap_gems(i + 1, j, i, j);
				if (gweled_is_part_of_an_alignment(i, j)) {
					move_found = true;
				}
                swap_gems(i + 1, j, i, j);
			}
			if (j > 0 && move_found != true) {
                swap_gems(i, j - 1, i, j);
				if (gweled_is_part_of_an_alignment(i, j)) {
					move_found = true;
				}
                swap_gems (i, j - 1, i, j);
			}
			if (j < 7 && move_found != true) {
                swap_gems(i, j + 1, i, j);
				if (gweled_is_part_of_an_alignment(i, j)) {
					move_found = true;
				}
                swap_gems(i, j + 1, i, j);
			}
		}
    }
    if (move_found == true && pi && pj) {
        *pi = i;
        *pj = j;
    }

	return move_found;
}

/**
*********************************************************************
* @brief Check if there is alignment
* 
* This function verifies if the player's move made an alignment
* 
* @param alignment  the found alignment
* 
* @return           true if there is an alignment
*                   false if there is no alignment
*
* @todo fix if we have the following pattern: xxoxoo and swap 
* the 2 central gems: xxxooo <- this is counted as 1 alignment of 6
********************************************************************/
int gweled_check_for_alignment (Alignment *alignment) 
{
    int i, j, i_nb_aligned, start_x, start_y;
    
    // find a vertical alignment
    i_nb_aligned = 0;

    for (i = 0; i < BOARD_WIDTH; i++) { 
        for (j = 0; j < BOARD_HEIGHT; j++)
            if ((gweled_is_part_of_an_alignment (i, j) & 2) == 2) {
                // record the origin of the alignment
                if (i_nb_aligned == 0) {
                    start_x = i;
                    start_y = j;
                }
                i_nb_aligned++;
            } else {
                // we found one
                if (i_nb_aligned > 2) {
                    alignment->x = start_x;
                    alignment->y = start_y;
                    alignment->direction = 2;
                    alignment->length = i_nb_aligned;
                    return true;
                }
                i_nb_aligned = 0;
            }

        // end of column
        if (i_nb_aligned > 2) {
            alignment->x = start_x;
            alignment->y = start_y;
            alignment->direction = 2;
            alignment->length = i_nb_aligned;
            return true;
        }
        i_nb_aligned = 0;
    }
    
// find an horizontal alignment
    for (j = 0; j < BOARD_HEIGHT; j++) {
        for (i = 0; i < BOARD_WIDTH; i++)
            if ((gweled_is_part_of_an_alignment (i, j) & 1) == 1) {
                // record the origin of the alignment
                if (i_nb_aligned == 0) {
                    start_x = i;
                    start_y = j;
                }
                i_nb_aligned++;
            } else {                            
                if (i_nb_aligned > 2) {
                    alignment->x = start_x;
                    alignment->y = start_y;
                    alignment->direction = 1;
                    alignment->length = i_nb_aligned;
                    return true;
                }
                i_nb_aligned = 0;
            }

        // end of row
        if (i_nb_aligned > 2) {
            alignment->x = start_x;
            alignment->y = start_y;
            alignment->direction = 1;
            alignment->length = i_nb_aligned;
            return true;
        }
        i_nb_aligned = 0;
    }
    return false;
}

/**
*********************************************************************
* @brief Remove gems and update score
* 
* This function removes the aligneds gems and updates the total score
* 
* @param alignment  the found alignment
*
********************************************************************/
void gweled_remove_gems_and_update_score(Alignment *alignment, int bonus){
    int i, gems_removed_per_move = 0;
 
    //Horizontal
     if (alignment->direction == 1) {
        for(i=alignment->x; i<alignment->x+alignment->length; i++) {
            gems_removed_per_move++;
            nb_of_tiles[matrix[alignment->y][i]]--;
            matrix[alignment->y][i]= DIAMONDS;
        }
    }
    //Vertical
    if (alignment->direction == 2) {
        for(i=alignment->y; i<alignment->y+alignment->length; i++) {
            gems_removed_per_move++;
            nb_of_tiles[matrix[i][alignment->x]]--;
            matrix[i][alignment->x]= DIAMONDS;
        }
    }
    //compute score
    i = score * (gems_removed_per_move - 2) * bonus;    
    if (gm == GM_MULTIPLAYER) {
        combo_score += i;
        update_powerbar();
    } else {
        total_score += i;
        draw_score(true);
	i /= 5;
	if (i > 6) i = 6;
	single_timer += i;
	if (single_timer >= 310) {
		score += 5;
		timer_delay -= 100;
		if (timer_delay < 0) timer_delay = 0;
		single_timer = 155;
	}
	draw_timer_bar(true);
    }    
}

/**
************************************************************************
* @brief Complete the empty spaces
* 
* This function made the gems above the empty spaces to fall in them
* and creates new gems to put in the spaces that remais empty
* 
* @param alignment  the found alignment which created the empty spaces
*
************************************************************************/
void gweled_refill_board(Alignment *alignment) {
	
    int i, j;

    if (alignment->direction == 1) {
    	for (i = alignment->x; i < alignment->x + alignment->length; i++){
    		for (j=alignment->y; j > 0; j--) {
    			matrix[j][i] = matrix[j-1][i];
    		}
    		matrix[0][i] = get_new_tile();
    		nb_of_tiles[matrix[0][i]]++;
       }
    }
    else {
        for (j=alignment->y-1; j >= 0; j--) {
            matrix[j+alignment->length][alignment->x] = matrix[j][alignment->x];
        }
        for(j=0; j<alignment->length; j++) {
            matrix[j][alignment->x] = get_new_tile();
            nb_of_tiles[matrix[j][alignment->x]]++;
        }
    }
}

/**
************************************************************************
* @brief Complete the empty spaces - drawing the gems
* 
* This function made the gems above the empty spaces to fall in them
* 
* @param alignment  the found alignment which created the empty spaces
*
************************************************************************/
void gweled_gems_fall_into_place(Alignment *alignment){
    
    int i;
    SDL_Event event;
    
    if (alignment->direction == 1) {
        for (i=DIAMOND_HEIGHT; i>=0; i-=6) {
            draw_board(alignment->x, alignment->length, alignment->y+1, i, true);
	    update_player();
	    time_tick();
        }
    } else {
        for (i=alignment->length*DIAMOND_HEIGHT; i >=0; i-=6) {
            draw_board(alignment->x, 1, alignment->y+alignment->length, i, true);
	    update_player();
	    time_tick();
        }
    }
    while (SDL_PollEvent(&event));
}

// Start a new game
void new_game(int reset_score, GAMEMODE mode, int load) {
    int i;
    SDL_Rect dest;
    SDL_Event event;

    // Initialize variables
    gm = mode;
    ss = SS_IDLE;    
    game_state.state = IDLE;
    game_state.attack_cnt = 32;
    game_state.left_img = 0;
    game_state.right_img = 0;
    if (!load) {
        initialize_board();
    }

    // Initialize board
    if (reset_score) {
        if (!load) {
            total_score = 0;
	    single_timer = 155;
        }
        combo_score = enemy_score = 0;
        SDL_BlitSurface(bg, NULL, screen, NULL);
        if (mode == GM_SINGLE) {
            dest.x = SINGLEBGX;
            dest.y = SINGLEBGY;
            dest.w = dest.h = 0;
            SDL_BlitSurface(bg_single, NULL, screen, &dest);
            draw_score(false);
	    draw_timer_bar(false);
        }
        if (!load) {
            SDL_Flip(screen);
        }
    }

    // Fall the board
    if (mode == GM_MULTIPLAYER) draw_player();
    if (!load) {
        for (i = BOARD_HEIGHT * DIAMOND_HEIGHT; i >= 0; i -= 9) {
            draw_board(0, BOARD_WIDTH, BOARD_HEIGHT, i, true);
	    time_tick();
        }
	while (SDL_PollEvent(&event));
    } else {
        SDL_Flip(screen);
    }
}
