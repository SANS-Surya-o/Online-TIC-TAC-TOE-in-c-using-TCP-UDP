#include "headers.h"


#ifndef _UDP_H
#define _UDP_H

//Server stuff
extern int num_players;
int setup_listener(int portno);
int get_players(int socket_fd, int *clients);
int write_client_msg(int client_fd, char *msg);
bool rematch(int* clients );
int get_move(int sock_fd);
int send_update(int* clients, int move, int turn);


// Client functions -- strict constraint : client should only send integers
int connect_to_server(char* hostanme, int portno);
void read_server_msg(int sock_fd, char* msg);
void write_server_int(int sock_fd, int msg);
int read_server_int(int useless);


#endif

