#ifndef GAME_H
#define GAME_H

#include "defines.h"

void initialize_game();

void display();

Command collect_command();

void cursor_control(Command cmd);

void plane_control(Command cmd);

void stage_control(Command cmd);

void check_reveal(Command cmd);

#endif
