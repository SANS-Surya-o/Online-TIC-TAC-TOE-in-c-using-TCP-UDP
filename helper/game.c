#include "headers.h"

int board[3][3];

void init_game()
{
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
            board[i][j] = -1;
        }
    }
}

void print_board()
{
    printf("\n\n");
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
            if (board[i][j]==-1)
            {
                printf(". ");
            }
            else if (board[i][j]==1)
            {
                printf("O ");
            }
            else
            {
                printf("X ");
            }
            // if (j!=2)
            // {
            //     printf("|");
            // }
        }
        printf("\n");
        // if (i!=2)
        // {
        //     printf("-----\n");
        // }
    }
    printf("\n\n");
}

int update_board(int move, int turn)
{
    int i = move/3;
    int j = move%3;
    if (board[i][j]!=-1)
    {
        return 0;
    }
    board[i][j] = turn;
    return 1;
}

bool check_victory()
{
    for (int i=0;i<3;i++)
    {
        if (board[i][0]!=-1 && board[i][0]==board[i][1] && board[i][1]==board[i][2])
        {
            return true;
        }
        if (board[0][i]!=-1 && board[0][i]==board[1][i] && board[1][i]==board[2][i])
        {
            return true;
        }
    }
    if (board[0][0]!=-1 && board[0][0]==board[1][1] && board[1][1]==board[2][2])
    {
        return true;
    }
    if (board[0][2]!=-1 && board[0][2]==board[1][1] && board[1][1]==board[2][0])
    {
        return true;
    }
    return false;
}

bool check_draw()
{
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
            if (board[i][j]==-1)
            {
                return false;
            }
        }
    }
    return true;
}

bool check_move(int move)
{
    if (move < 0 || move > 8)
    {
        return false;
    }
    int i = move/3;
    int j = move%3;
    if (board[i][j]!=-1)
    {
        return false;
    }
    return true;
}