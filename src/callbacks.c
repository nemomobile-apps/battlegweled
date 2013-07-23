#include <stdio.h>
#if HGW_FUNC
#include <hgw/hgw.h>
#include "callbacks.h"
#endif
#include "level.h"
#include "images.h"

// Pause the game
int exit_callback(int errcode) {
	FILE *han;

	// Save state
	han = fopen("/tmp/.battlegweled-save", "wb");
	if (han) {
		fwrite(&total_score, sizeof(int), 1, han);
		fwrite(&single_timer, sizeof(int), 1, han);
		fwrite(&timer_delay, sizeof(int), 1, han);
		fwrite(&score, sizeof(int), 1, han);
	        fwrite(nb_of_tiles, sizeof(nb_of_tiles), 1, han);
	        fwrite(matrix, BOARD_WIDTH * BOARD_HEIGHT, sizeof(int), han);
		fclose(han);		
	} 

#if HGW_FUNC
//	hgw_notify_startup(hgw_context, HGW_SUN_PAUSE);
#endif
	return 0;
}

// Quit game
int quit_callback(int errcode) {
//	remove("/tmp/.battlegweled-save");
#if HGW_FUNC
//	hgw_context_compat_destroy_quit(hgw_context);
#endif

	return 0;
}

int flush_callback(int errcode) {
	remove("/tmp/.battlegweled-save");

	return 0;
}
