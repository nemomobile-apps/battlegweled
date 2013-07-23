#ifndef _IMAGES_H_
#define _IMAGES_H_

#include <SDL.h>
#include <SDL_thread.h>
#include "sdl12compat.h"

#define BACK_OFFSETX        216
#define BACK_OFFSETY        394
#define BACK_OFFSETX2       BACK_OFFSETX + 365
#define BACK_OFFSETY2       BACK_OFFSETY + 70

#define BACK2_OFFSETX       8
#define BACK2_OFFSETY       394
#define BACK2_OFFSETX2      BACK2_OFFSETX + 365
#define BACK2_OFFSETY2      BACK2_OFFSETY + 70

#define TIMER_OFFSETX       20
#define TIMER_OFFSETY       310
#define TIMER_WIDTH         311
#define TIMER_HEIGHT        36

extern int draw;
extern SDL_mutex *mutex;
extern SDL_Surface *p1l[5], *p1r[5], *p2l[5], *p2r[5];
extern SDL_Surface *back, *waiting, *diamond[DIAMONDS], *number;
extern SDL_Surface *screen, *bg, *logo, *selection, *bar, *win, *lose, *bg_single;

void SDL_Update(int x, int y, int w, int h);
SDL_Surface *load_image(char *filename, int transparent);
int load_surfaces();
void free_surfaces();
void draw_board(int x, int w, int h, int pos, int update_screen);
int draw_screen();
void draw_waiting_screen();
void changing_location();
void update_powerbar();
void draw_score(int update);
void draw_timer_bar(int update);
void update_player();
void start_attack(int left);

#endif //_IMAGES_H_
