#include <iostream>
#include <vector>
#include "defines.h"
#include "helper.h"
#include "game.h"

int main(void) {
	if (initialize_sdl() || initialize_img()) {
		ERROR_MSG("MAIN: SDL init failed. Exiting...");
		return 0;
	}
	
	while (!gameQuit) {
		Command cmd = collect_command();

		SDL_RenderClear(gRenderer);
		
		initialize_game();

		cursor_control(cmd);

		plane_control(cmd);

		stage_control(cmd);

		check_reveal(cmd);

		display();

		SDL_RenderPresent(gRenderer);

		SDL_Delay(10);
	}	


	quit();

}

