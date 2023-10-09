#ifndef HELPER_H
#define HELPER_H

#include "defines.h"

int initialize_sdl();

int initialize_img();

SDL_Texture* load_img(const char* name);

SDL_Texture* load_text(const char* text);

void quit();

#endif
