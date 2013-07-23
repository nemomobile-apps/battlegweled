#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <SDL_thread.h>

#define BUFFERSIZE                16
#define PORT                      10248
#define END_GAME                  -1
#define BOARD_RESTARTED           -2
#define CONNECTION_REQUEST	  -3
#define CONNECTION_APPROVED       -4   
#define CONNECTION_REJECTED       -5
#define GAME_OVER2                -6
#define BATTLEGWELED_CREATESERVER "/apps/indt/battlegweled/createserver"
#define BATTLEGWELED_SERVERIP     "/apps/indt/battlegweled/serverip"

typedef enum {
    SS_IDLE, SS_CLOSE, SS_CONNECTING, SS_WAITING, SS_CONNECTED, SS_CANCELLED, SS_ERROR
} SOCKET_STATE;

extern SOCKET_STATE ss;
extern SDL_Thread *ThreadAccept, *ThreadConnect;
//extern int SocketServer, sock;
extern int connected;

int create_game();
int join_game(char *hostname);
void write_socket(void *buffer, int len);
int read_socket(void *buffer, int len);
int multi_player_loop(void *param);

#endif //_SOCKET_H_
