#include "headers.h"

#ifdef USE_UDP

#define MAX_PLAYERS_QUEUE 5



const int msg_size = 3;
struct sockaddr_in client_data[2];
struct sockaddr_in serv_addr;
int server_sockfd = -1;
int client_sockfd = -1;


int write_client_msg(int turn, char *msg)
{
    struct sockaddr_in* client_addr = &client_data[turn];
    socklen_t client_len = sizeof(*client_addr);

    int n = sendto(server_sockfd, msg, strlen(msg), 0, (struct sockaddr *)client_addr, client_len);
    if (n < 0)
    {
        perror("write_client_msg : sendto");
        printf("Client at %s:%d likely disconnected\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
        return -1;
    }
    return 0;
}





void read_server_msg(int useless, char *msg)
{
    struct sockaddr_in* server_addr = &serv_addr;
    socklen_t server_len = sizeof(*server_addr);

    memset(msg, 0, 4); // Clear the message buffer
    int n = recvfrom(client_sockfd, msg, 3, 0, (struct sockaddr *)server_addr, &server_len); // Receive 3 bytes
    if (n < 0 || n != 3)
    {
        perror("read_server_msg : recvfrom");
        printError("Server or the other player disconnected");
        msg[0] = '\0'; // Set to empty string if an error occurs
    }
    else
    {
        msg[3] = '\0'; // Null-terminate the string
    }
}


void write_server_int(int useless, int msg)
{
    struct sockaddr_in* server_addr = &serv_addr;
    socklen_t server_len = sizeof(*server_addr);

    int n = sendto(client_sockfd, &msg, sizeof(int), 0, (struct sockaddr *)server_addr, server_len);
    if (n < 0)
    {
        perror("write_server_int : sendto");
        printError("Server or the other player disconnected");
        exit(EXIT_FAILURE);
    }
   
}




int read_client_msg(int turn)
{
    struct sockaddr_in* client_addr = &client_data[turn];
    socklen_t client_len = sizeof(*client_addr);
    int msg;
    int n = recvfrom(server_sockfd, &msg, sizeof(int), 0, (struct sockaddr *)client_addr, &client_len);
    if (n < 0)
    {
        perror("read_client_msg : recvfrom");
        return -1;
    }
    return msg;
}

int read_server_int(int useless)
{
    struct sockaddr_in* server_addr = &serv_addr;
    socklen_t server_len = sizeof(*server_addr);
    int msg;
    int n = recvfrom(client_sockfd, &msg, sizeof(int), 0, (struct sockaddr *)server_addr, &server_len);
    if (n < 0)
    {
        perror("read_server_int : recvfrom");
        printError("Server or the other player disconnected");
        exit(EXIT_FAILURE);
    }
    return msg;
}




/////////////////////////////////////    GOD-TIER FUNCTIONS /////////////////////////////////////////////////////////

// Cretae a listening socket and bind it to the given port number. Return the socket file descriptor.
int setup_listener(int portno)
{
    struct sockaddr_in serv_addr;

    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sockfd < 0) 
        {perror("setup_listener : socket :");
        return -1; }
    
        // Forcefully attaching socket to the port 8080 using SO_REUSEADDR
        int opt = 1;

    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;	
    serv_addr.sin_addr.s_addr = INADDR_ANY;	
    serv_addr.sin_port = htons(portno);		

    if (bind(server_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("setup_listener : bind :");
        close(server_sockfd);
        return -1;
    }
    

    #ifdef DEBUG
    printf("[DEBUG] Listener set.\n");    
    #endif 

    return server_sockfd;
}



int connect_to_server(char* hostname, int portno)
{
    int n;
    struct hostent *server;

    client_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sockfd < 0) 
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


    if (sendto(client_sockfd, REQUEST, strlen(REQUEST), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Server not running");
        exit(EXIT_FAILURE);
    }
    
    #ifdef DEBUG
    printf("[DEBUG] Sending message to server at %s:%d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
    printf("[DEBUG] Waiting for response...\n");
    #endif
    return client_sockfd;
}



#define MAX_BUFFER_SIZE 4
// DO ERROR HANDLING LATER
int get_players(int sockettta_fd, int* clients)
{
    printf("Here");
    socklen_t clilen;
    char buffer[MAX_BUFFER_SIZE];  // Buffer for receiving messages
    int num_players = 0;

    while (num_players < 2)
    {
        clilen = sizeof(client_data[num_players]);
        memset(&client_data[num_players], 0 , sizeof(client_data[0])); // Clear the clients' data
        // Receive data from the client and get its address
        int recv_len = recvfrom(server_sockfd, buffer, MAX_BUFFER_SIZE, 0, 
                                (struct sockaddr *)&client_data[num_players], &clilen);
        if (recv_len < 0)
        {
            perror("get_players : recvfrom :");
            continue;
        }

        if (num_players == 0)
        {
            printf("WAITING\n");
            // Send HOLD message to the first player
            sendto(server_sockfd, HOLD, strlen(HOLD), 0, 
                   (struct sockaddr *)&client_data[num_players], clilen);
        }

        printf("[DEBUG] Player %d connected from %s:%d\n", num_players + 1,
               inet_ntoa(client_data[num_players].sin_addr), ntohs(client_data[num_players].sin_port));

        num_players++;
    }
    clients[0] = 0;
    clients[1] = 1;
    return num_players;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int send_update(int* clients, int move, int turn)
{
    // Send update message to both clients
    write_client_msg(clients[0], UPDATE);
    write_client_msg(clients[1], UPDATE);

    socklen_t client_len = sizeof(struct sockaddr_in);

    // Send move to the first client
    int n = sendto(server_sockfd, &move, sizeof(move), 0, (struct sockaddr *)&client_data[clients[0]], client_len);
    if (n < 0) 
    {
        perror("send_update : sendto :");
        printf("Client %d likely disconnected\n", clients[0]);
        return -1;
    }
    
    // Send turn to the first client
    n = sendto(server_sockfd, &turn, sizeof(turn), 0, (struct sockaddr *)&client_data[clients[0]], client_len);
    if (n < 0) 
    {
        perror("send_update : sendto :");
        printf("Client %d likely disconnected\n", clients[0]);
        return -1;
    }

    // Send move to the second client
    n = sendto(server_sockfd, &move, sizeof(move), 0, (struct sockaddr *)&client_data[clients[1]], client_len);
    if (n < 0) 
    {
        perror("send_update : sendto :");
        printf("Client %d likely disconnected\n", clients[1]);
        return -1;
    }

    // Send turn to the second client
    n = sendto(server_sockfd, &turn, sizeof(turn), 0, (struct sockaddr *)&client_data[clients[1]], client_len);
    if (n < 0) 
    {
        perror("send_update : sendto :");
        printf("Client %d likely disconnected\n", clients[1]);
        return -1;
    }

    return 0; // Return 0 on successful completion
}

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


