#include "headers.h"




#ifdef USE_TCP

#define MAX_PLAYERS_QUEUE 5
const int msg_size = 3;


int write_client_msg(int client_socket_fd, char *msg)
{
    int n = write(client_socket_fd, msg, strlen(msg));
    if (n < 0) 
        {
            perror("write_client_msg :");
            printf("Client %d likely disconnected", client_socket_fd);
            return -1;
        }
    return 0;
}



int read_client_msg(int socket_fd)
{
    int msg;
    int n = recv(socket_fd,&msg,sizeof(msg),0);
    if (n < 0){ 
        perror("get_move : recv :");
        printf("Client likely disconnected\n");
        return -1;}
    return msg;
}   


void read_server_msg(int sock_fd, char *msg) {
    memset(msg, 0, 4);
    int n = read(sock_fd, msg, 3);
    if (n < 0 || n != 3)
        {
            printError("Server or the other player disconnected");
            msg[0] = '\0';
        }
    else
    msg[3] = '\0';
}

void write_server_int(int sock_fd, int msg)
{
    int n = write(sock_fd, &msg, sizeof(msg));
    if (n < 0) 
        {
            perror("write_server_int :");
            printf("Server likely disconnected\n");
            exit(EXIT_FAILURE);
        }
}

int read_server_int(int sock_fd)
{
    int msg;
    int n = read(sock_fd, &msg, sizeof(msg));
    if (n < 0) 
        {
            perror("read_server_int :");
            printf("Server likely disconnected\n");
            exit(EXIT_FAILURE);
        }
    return msg;
}
    




/////////////////////////////////////    GOD-TIER FUNCTIONS /////////////////////////////////////////////////////////

// Cretae a listening socket and bind it to the given port number. Return the socket file descriptor.
int setup_listener(int portno)
{


    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        {perror("setup_listener : socket :");
        return -1; }
    
        // Forcefully attaching socket to the port 8080 using SO_REUSEADDR
        int opt = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;	
    serv_addr.sin_addr.s_addr = INADDR_ANY;	
    serv_addr.sin_port = htons(portno);		

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("setup_listener : bind :");
            return -1;    
        }
            

    

    #ifdef DEBUG
    printf("[DEBUG] Listener set.\n");    
    #endif 

    return sockfd;
}



int connect_to_server(char* hostname, int portno)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        {
            perror("connect_to_server : socket :");
            return -1; 
        }
    
    server = gethostbyname(hostname);
	
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
     memmove(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        {
            perror("connect_to_server : connect");
            return -1;
        }
    
    #ifdef DEBUG
    printf("[DEBUG] Connected to server.\n");
    #endif
    return sockfd;
}




// DO ERROR HANDLING LATER
int get_players(int socket_fd, int *clients)
{
    socklen_t clilen;
    struct sockaddr_in cli_addr;

   
    int num_players = 0;

    while (num_players < 2)
    {
        if (listen(socket_fd, MAX_PLAYERS_QUEUE) < 0)
        {
            perror("get_players : listen :");
            return -1;
        }

        #ifdef DEBUG
            printf("[DEBUG] Listening for clients...\n");
        #endif 

        clilen = sizeof(cli_addr);
        clients[num_players] = accept(socket_fd, (struct sockaddr *) &cli_addr, &clilen);
        if (clients[num_players] < 0) 
            {
                perror("get_players : accept :");
                continue;
            }

        if (num_players == 0)
        {
            printf("WAITING");
            write_client_msg(clients[num_players],HOLD);
        }

        num_players++;

        #ifdef DEBUG
        printf("[DEBUG] Player %d connected.\n", num_players);
        #endif
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int send_update(int* clients, int move, int turn)
{
    write_client_msg(clients[0],UPDATE);
    write_client_msg(clients[1],UPDATE);


    int n = send(clients[0], &move, sizeof(move), 0);
    if (n < 0) 
        {
            perror("send_update : send :");
            printf("Client likely disconnected\n");
            return -1;
        }
    
    n = send(clients[0], &turn, sizeof(turn), 0);
     if (n < 0) 
        {
            perror("send_update : send :");
            printf("Client likely disconnected\n");
            return -1;
        }


    // Send to second player
    n = send(clients[1], &move, sizeof(move), 0);
    if (n < 0) 
        {
            perror("send_update : send :");
            printf("Client likely disconnected\n");
            return -1;
        }

    n = send(clients[1], &turn, sizeof(turn), 0);
    if (n < 0) 
        {
            perror("send_update : send :");
            printf("Client likely disconnected\n");
            return -1;
        }
    
}


// This function eis proof that Im very dumb and suck at programming
int get_move(int sock_fd)
{
    return read_client_msg(sock_fd);
}


bool rematch(int* clients)
{
    write_client_msg(clients[0],REMATCH);
    write_client_msg(clients[1],REMATCH);

    int response = read_client_msg(clients[0]);
    if (response == 1)
    {
        response = read_client_msg(clients[1]);
        if (response == 1)
        {
            return true;
        }
    }
    return false;
}

#endif


