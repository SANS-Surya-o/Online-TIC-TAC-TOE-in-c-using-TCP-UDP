#include "helper/headers.h"

int listening_socket_fd;




int clients[2];






int run_game(int* clients)
{
    init_game();
    if (write_client_msg(clients[0],PLR1) < 0) return -1;
    if (write_client_msg(clients[1],PLR2) < 0) return -1;

    int turn = 0;
    while (1)
    {
        print_board();
        if (write_client_msg(clients[turn],TURN) < 0 ) return -1;
        if (write_client_msg(clients[1-turn],WAIT) < 0) return -1;
        int move = 0;

        while(1)
        {
            move = get_move(clients[turn]);
            if (move == -1)
            {
                if (write_client_msg(clients[1-turn],ERROR) < 0) return -1;
                printf("Client likely disconnected\n");
                return -1;
            }
            printf("PLAYER: %d MOVE: %d\n",turn,move);
            if (check_move(move))
            {
                break;
            }
            printf("Invalid! Asking him to try again");
            if (write_client_msg(clients[turn],INVALID_MOVE) < 0) return -1;
            if (write_client_msg(clients[turn],TURN) < 0) return -1;
        }
        update_board(move , turn);

        // SEND Update TO BOTH PLAYERS
        if (send_update(clients,move,turn) < 0) return -1;

        if (check_victory())
        {
            if (write_client_msg(clients[turn],WIN) < 0) return -1;
            if (write_client_msg(clients[1-turn],LOSE) < 0) return -1;
            printf("Player %d wins\n",turn);
            return 0;
        }
        if (check_draw())
        {
            if (write_client_msg(clients[0],DRAW) < 0) return -1;
            if (write_client_msg(clients[1],DRAW) < 0) return -1;
            printf("Draw\n");
            return 0;
        }

        turn = 1 - turn;
    }
    return 0;
}


void close_connection()
{
    printf("Client/s disconnected\n");
    close(clients[0]);
    close(clients[1]);
    close(listening_socket_fd);
}


const int portno = 55000;           
        


int main()
{
    #ifdef USE_TCP
    atexit(close_connection);
    // close_connection();
    #endif

    signal(SIGPIPE, SIG_IGN);
    // Set up listener that waits for 
    // 1. connections in case of TCP
    // 2. datagrams in case of UDP
    listening_socket_fd = setup_listener(portno);
    if (listening_socket_fd < 0)
    {
        printError("Error setting up listener");
        return -1;
    }

// REMINDER: implement function to graciously terminate all connections when exit is pressed
    get_players(listening_socket_fd, clients);

    while(1)
    {
        printf("New Game started\n");
        if (run_game(clients) < 0)
        {
            return 0;
        }
        if (!rematch(clients))
        {
            printf("Game ended\n");
            write_client_msg(clients[0],END);
            write_client_msg(clients[1],END);
            break;
        }

    }


}

    



