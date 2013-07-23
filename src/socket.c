#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
//#include <SDL_net.h>
#include "level.h"
#include "images.h"
#include "socket.h"

SOCKET_STATE ss;

SDL_Thread *ThreadAccept = NULL, *ThreadConnect = NULL;

void write_socket(void *buffer, int len) {}
int read_socket(void *buffer, int len) {}
int accept_client(void *param) {}
int create_game() {}
int connect_server(void *param) {}
int join_game(char *hostname) {}
int multi_player_loop(void *param) {}

#if 0

int connected = false;
IPaddress ipa;
// struct sockaddr_in pin;
TCPsocket SocketServer, sock;

// Send a buffer
void write_socket(void *buffer, int len) {
	SDLNet_TCP_Send(sock, buffer, len);
/*	int pos = 0, write;
	
	while (pos < len) {
		write = send(sock, buffer + pos, len - pos, 0);
		if (write <= 0) return 0;
		else pos += write;
	}
	
	return 1; */
}

// Read a buffer
int read_socket(void *buffer, int len) {
	int pos = 0, read;
	
	while (pos < len) {
//		read = recv(sock, buffer + pos, len - pos, 0);
		read = SDLNet_TCP_Recv(sock, buffer + pos, len - pos);
		if (read <= 0) return 0;
		else pos += read;
	}
	
	return 1;
}

// Waiting a client
int accept_client(void *param) {
    int status;

    while (true) {

/*        sock = accept(SocketServer, NULL, 0);
        if (sock < 0) {
            ss = SS_ERROR;
            return 0;
        } */
	usleep(100000);
	sock = SDLNet_TCP_Accept(SocketServer);
	if (!sock) continue;

	read_socket(&status, sizeof(int));
	if (status == CONNECTION_REQUEST) {
            status = CONNECTION_APPROVED;
            write_socket(&status, sizeof(int));
            ss = SS_CONNECTED;
	    break;
	} else {
            status = CONNECTION_REJECTED;
	    write_socket(&status, sizeof(int));
	    SDLNet_TCP_Close(sock); // close(sock);
	}
    }
    return 0;
}

// Creates a room for the game
int create_game() {
	IPaddress ip;
//    int opt = 1;
//    struct sockaddr_in sin;
 
    // get an internet domain socket 
/*    SocketServer = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketServer < 0) {
        ss = SS_ERROR;
        return false;
    }
    
    // complete the socket structure 
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT);
    
    // bind the socket to the port number
    setsockopt(SocketServer, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(SocketServer, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        close(SocketServer);
        ss = SS_ERROR;
        return false;
    }
    
    // show that we are willing to listen 
    if (listen(SocketServer, 1) < 0) {
        close(SocketServer);
        ss = SS_ERROR;
        return false;
    } */
	if (SDLNet_ResolveHost(&ip, NULL, PORT)) {
		ss = SS_ERROR;
		return false;
	}
	SocketServer = SDLNet_TCP_Open(&ip);
	if (!SocketServer) {
		ss = SS_ERROR;
		return false;
	}
	

    // Creates thread for waiting client
    ss = SS_WAITING;
    game_state.server = true;
    ThreadAccept = SDL_CreateThread(accept_client, NULL);

    return true;
}

static void connect_alarm(int signo) {
	SDLNet_TCP_Close(sock); //    close(sock);
}

// Connect in server
int connect_server(void *param) {
    int permission;
    __sighandler_t sigfunc;

    sigfunc = signal(SIGALRM, connect_alarm);
    while (1) {
    
        /* grab an Internet domain socket */
/*        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            ss = SS_ERROR;
	    break;
        }

        // connect to PORT on HOST
	alarm(4);
        if (connect(sock, (struct sockaddr *)&pin, sizeof(pin)) < 0) {
	    alarm(0);
            close(sock);
	    usleep(100000);
	    continue;
        }
	alarm(0); */

	alarm(4);
	sock = SDLNet_TCP_Open(&ipa);
	if (!sock) {
	    alarm(0);
	    usleep(100000);
	    continue;
	}
	alarm(0);
    
	permission = CONNECTION_REQUEST;
	write_socket(&permission, sizeof(int));
	read_socket(&permission, sizeof(int));
        if (permission == CONNECTION_APPROVED) {
    
            // Connected
            ss = SS_CONNECTED;
	    break;
        } else SDLNet_TCP_Close(sock); // close(sock);
    }

    alarm(0);
    signal(SIGALRM, sigfunc);
    return 0;
}

// Join in game
int join_game(char *hostname) {
//    struct hostent *hp;

    // go find out about the desired host machine
/*    hp = gethostbyname(hostname);
    if (!hp) {
        ss = SS_ERROR;
        return false;
    }

    // fill in the socket structure with host information 
    memset(&pin, 0, sizeof(pin));
    pin.sin_family = AF_INET;
    pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    pin.sin_port = htons(PORT); */

	if (SDLNet_ResolveHost(&ipa, hostname, PORT)) {
	        ss = SS_ERROR;
        	return false;
	}

    // Create thread for connect in server
    ss = SS_CONNECTING;
    game_state.server = false;
    ThreadConnect = SDL_CreateThread(connect_server, NULL);
    return true;
}

// Loop for read sock
int multi_player_loop(void *param) {
    int combo;
    SDL_Rect dest;
    
    while (read_socket(&combo , sizeof(int)) > 0) {
        if (combo == END_GAME) {
            game_state.state = GAME_WIN;

        } else if (combo == BOARD_RESTARTED) {
            combo_score <<= 1;
            update_powerbar();

        } else {
            enemy_score += combo;
            if (enemy_score >= MAX_LIFE) {
                enemy_score = MAX_LIFE;
                game_state.state = GAME_LOSE;
            } else start_attack(0);
            
            // Update life bar
            dest.x = LIFE_YOUX;
            dest.y = LIFEY;
            dest.w = enemy_score / 10;
            dest.h = LIFE_HEIGHT;
            SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 255, 0, 0));
            SDL_Update(LIFE_YOUX, LIFEY, LIFE_WIDTH, LIFE_HEIGHT);
        }
    }
    ss = SS_CLOSE;
    return 0;
}
#endif
