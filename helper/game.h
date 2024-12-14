#ifndef _GAME_
#define _GAME_

void init_game();
void print_board();
int update_board(int, int);
bool check_move(int);
bool check_victory();
bool check_draw();

#endif