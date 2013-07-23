#ifndef _LEVEL_H_
#define _LEVEL_H_

#define BOARD_WIDTH	8
#define BOARD_HEIGHT	8
#define BOARD_OFFSETX	352
#define BOARD_OFFSETY	24
#define DIAMOND_WIDTH	54
#define DIAMOND_HEIGHT	54
#define LIFE_YOUX       21
#define LIFE_ENEMYX     327
#define LIFEY           144
#define LIFE_WIDTH      140
#define LIFE_HEIGHT     15
#define SINGLEBGX       13
#define SINGLEBGY       101
#define SCOREX          28
#define SCOREY          176
#define SCORE_WIDTH     44
#define SCORE_HEIGHT    64
#define DIAMONDS	7
#define HORIZONTAL      1
#define VERTICAL        2
#define MAX_LIFE	1400

#define true 		1
#define false		0

extern int single_timer, timer_delay, score;
extern int matrix[BOARD_HEIGHT][BOARD_WIDTH];
extern int diamond_bit[DIAMONDS+1];
extern int nb_of_tiles[DIAMONDS];

typedef enum {
    SELECTED_FIRST, SELECTED_SECOND, IDLE, MOVING, NEW_GAME, GAME_WIN, GAME_LOSE, GAME_EXIT, UNSELECTED_FIRST
} State;

typedef enum {
    GM_SINGLE, GM_MULTIPLAYER
} GAMEMODE;

typedef struct {
	State state;
	int x_first, y_first;
	int x_second, y_second;
	int server, attack_cnt;	
	int left_img, right_img;
} GameState;

typedef struct {
    int x;
    int y;
    int direction;
    int length;
} Alignment;

extern GAMEMODE gm;
extern GameState game_state;
extern int total_score, combo_score, enemy_score;

int gweled_is_part_of_an_alignment(int x, int y);
int gweled_check_for_alignment(Alignment *alignment);
void gweled_remove_gems_and_update_score(Alignment *alignment, int bonus);
void gweled_refill_board(Alignment *alignment);
void gweled_gems_fall_into_place(Alignment *alignment);
int gweled_check_for_moves_left(int *pi, int *pj);
int get_new_tile (void);
void initialize_board(void);
void new_game(int reset_score, GAMEMODE mode, int load);
void time_tick();

#endif //_LEVEL_H_
