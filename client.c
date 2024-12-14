#include "helper/headers.h"


#define SINGLE_MSG_SIZE 4


int main()
{
    printf("TIC-TAC-TOE\n");
    printf("Enter a valid hostname or a valid IP address\n");
    char hostname[1024];
    scanf("%s",hostname);
    int sock_fd = connect_to_server(hostname, 55000);
    if (sock_fd<0)
    {
        printf("Error connecting to server\n");
        return 0;
    }
    char msg[SINGLE_MSG_SIZE];
    init_game();
    

    while(1)
    {
        
        read_server_msg(sock_fd,msg);
        // printf("%s",msg);
        if (strcmp(msg,HOLD)==0)
        {
            printf("Waiting for the other player to join...\n");
        }
        else if (strcmp(msg,PLR1)==0)
        {
            init_game();
          
            printf("\nNew Game started\n");
            printf("You are PLAYER 1 --> Play with X\n");
            print_board();
        }
        else if (strcmp(msg,PLR2)==0)
        {
             init_game();
            
            printf("\nNew Game started\n");
            printf("You are PLAYER 2  --> Play with O\n");
            print_board();
        }

        else if (strcmp(msg,WAIT)==0)
        {
            printf("Opponents turn\n");
        }
        else if (strcmp(msg,TURN)==0)
        {
            printf("Your turn\n");
            int a,b;
            while(1)
            {
                scanf("%d%d",&a,&b);
                if (a<1 || a> 3 || b<1 || b>3)
                {
                    printf("Invalid! Try again\n");
                }
                else break;
            }
            int move = (a-1)*3+(b-1);
            // send(sock_fd, &move, sizeof(move), 0);
            write_server_int(sock_fd,move);
              
        }
            
        else if (strcmp(msg,INVALID_MOVE)==0)
        {
            printf("Invalid move. Try again\n");
        }
        else if (strcmp(msg,UPDATE)==0)
        {
            
            int move,turn;
            move = read_server_int(sock_fd);
            turn = read_server_int(sock_fd);
            // printf("%d %d",move, turn);
            update_board(move,turn);


            print_board();
        }
        else if (strcmp(msg,WIN)==0)
        {
            printf("You win\n");
            print_board();
        }
        else if (strcmp(msg,LOSE)==0)
        {
            printf("You lose\n");
            print_board();
        }
        else if (strcmp(msg,DRAW)==0)
        {
            print_board();
            printf("Draw\n");
        }
        else if (strcmp(msg,REMATCH)==0)
        {
            printf("Rematch? (Y/N)\n");
            char c;
            while(1)
            {
                scanf("%c",&c);
                if (c=='Y' || c=='N')
                {
                    break;
                }
                printf("Only \"Y\" or \"N\" accepted\n");
                printf("If you select Y and the other player selects N, it still ends\n");

            }
            int s = (c=='Y')?1:0;
            // send(sock_fd, &s, sizeof(s), 0);
            write_server_int(sock_fd,s);
            init_game();
        }
        else if (strcmp(msg,END)==0)
        {
            printf("Game ended\n");
            break;
        }
        else if (strcmp(msg,ERROR)==0)
        {
            printf("ERROR from the other side\n");
            break;
        }
        else
        {
            if (msg[0]!='\0')
            printf("UNEXPECTED MSG: %s\n",msg);
            break;
        }
    }
    close(sock_fd);
}
            




