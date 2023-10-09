#include "game.h"
#include "block.h"
#include "plane.h"

int totalPlayerNum = 2;

vector<vector<vector<Block*>>> blockVec;
vector<vector<Plane*>> planeVec;

Block* cursorBlock;
Plane* curPlaneBlock;

vector<Coordinate> lastCursorMapCoord;

bool gameQuit;
GameStage gameStage = INIT;
vector<int> hitPlanes;
vector<int> attempts;

bool attemptsUpdated = true;
int attemptsNumTotalWidth = 0;
vector<SDL_Texture*> attemptsTextureVec;
vector<Coordinate> attemptsNumInfoVec;

int playerTurn = 0;

bool showInfo = false;

void initialize_game() {

	if (gameStage == INIT) {
		playerTurn = 0;
		if (blockVec.size() != 0) {
			for (auto onePlayerBlockVec : blockVec) {
				for (auto rowVec : onePlayerBlockVec) {
					for (auto block : rowVec) {
						block->reset();
					}
				}
			}
		}
		if (planeVec.size() != 0) {
			for (auto onePlayerPlaneVec : planeVec) {
				for (auto plane : onePlayerPlaneVec) {
					plane->reset();
				}
			}
		}

		if (lastCursorMapCoord.size() != 0) {
			for (auto& onePlayerLastCursorMapCoord : lastCursorMapCoord) {
				onePlayerLastCursorMapCoord = {MAP_WIDTH / 2, MAP_HEIGHT / 2};
			}
		}

		if (cursorBlock) {
			cursorBlock->set_mapCoord({MAP_WIDTH / 2, MAP_HEIGHT / 2});
			cursorBlock->set_offset({SCREEN_X_MARGIN + playerTurn * SCREEN_WIDTH_SET, SCREEN_Y_MARGIN});
		}

		if (blockVec.size() == 0 || planeVec.size() == 0) {
			for (int player = 0; player < totalPlayerNum; player++) {
				hitPlanes.push_back(0);
				attempts.push_back(0);
				vector<vector<Block*>> onePlayerBlockVec;
				for (int row = 0; row < MAP_HEIGHT; row++) {
					vector<Block*> rowVec;
					for (int col = 0; col < MAP_WIDTH; col++) {
						Block* newBlock = new Block(blockTexture, {col, row}, 
													{SCREEN_X_MARGIN + player * SCREEN_WIDTH_SET, SCREEN_Y_MARGIN});
						rowVec.push_back(newBlock);
					}
					onePlayerBlockVec.push_back(rowVec);
				}
				blockVec.push_back(onePlayerBlockVec);

				vector<Plane*> onePlayerPlaneVec;
				for (int i = 0; i < planeTextureVec.size(); i++) {
					Plane* newPlane = new Plane(planeTextureVec[i], planeSelectTextureVec[i], planePosVec[i], planeValidBlockVec[i], planeHeadVec[i], 
												{SCREEN_X_MARGIN + player * SCREEN_WIDTH_SET, SCREEN_Y_MARGIN});	
					onePlayerPlaneVec.push_back(newPlane);
				}
				planeVec.push_back(onePlayerPlaneVec);

				lastCursorMapCoord.push_back({MAP_WIDTH / 2, MAP_HEIGHT / 2});
			}
			cursorBlock = new Block(cursorTexture, {MAP_WIDTH / 2, MAP_HEIGHT / 2},
									{SCREEN_X_MARGIN, SCREEN_Y_MARGIN});	
			cursorBlock->set_status(true);
		}

		gameStage = SET;
	} else {
		for (int player = 0; player < totalPlayerNum; player++) {
			if (hitPlanes[player] == planeVec[player].size()) {
				for (auto& hitPlane : hitPlanes) {
					hitPlane = 0;
				}
				gameStage = OVER;
				for (auto onePlayerBlockVec : blockVec) {
					for (auto rowVec : onePlayerBlockVec) {
						for (auto block : rowVec) {
							block->set_status(false);
						}
					}
				}
				break;
			}
		}
	}

}

int action;

Command collect_command() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) gameQuit = true;
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_q) gameQuit = true;
			if (e.key.keysym.sym == SDLK_UP) return CMD_UP;
			if (e.key.keysym.sym == SDLK_DOWN) return CMD_DOWN;
			if (e.key.keysym.sym == SDLK_LEFT) return CMD_LEFT;
			if (e.key.keysym.sym == SDLK_RIGHT) return CMD_RIGHT;
			if (e.key.keysym.sym == SDLK_SPACE) return CMD_ROTATE;
			if (e.key.keysym.sym == SDLK_RETURN) return CMD_ENTER;				
			if (e.key.keysym.sym == SDLK_y) return CMD_NEXTSTAGE;
		}
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			showInfo = true;
			return CMD_MOUSE_CLICK; 
		} else if (e.type == SDL_MOUSEBUTTONUP) {
			showInfo = false;
		}
	}
	return CMD_NONE;
}

void cursor_control(Command cmd) {

	if (!cursorBlock) {
		ERROR_MSG("GAME: cursorBlock not initialized");
	}

	switch (cmd) { 
		case CMD_UP:
		case CMD_DOWN:
		case CMD_LEFT:
		case CMD_RIGHT:
			cursorBlock->move(cmd);
			break;
	};
}

void plane_control(Command cmd) {

	if (gameStage == SET) {
		switch (cmd) { 
			case CMD_UP:
			case CMD_DOWN:
			case CMD_LEFT:
			case CMD_RIGHT:
				if (!curPlaneBlock) return;
				curPlaneBlock->move(cmd);
				break;
			case CMD_ROTATE:
				if (!curPlaneBlock) return;
				curPlaneBlock->rotate();
				break;
			case CMD_ENTER:
				if (curPlaneBlock)	{
					bool hasOverlap = false;
					vector<Plane*> overlapVec;
					for (auto refPlane : planeVec[playerTurn]) {
						if (refPlane != curPlaneBlock) {
							if (curPlaneBlock->overlap(refPlane)) {
								hasOverlap = true;
								overlapVec.push_back(refPlane);
							}
						}
					}

					if (!hasOverlap) {
						curPlaneBlock->set_status(false);
						curPlaneBlock = NULL;
						cursorBlock->set_status(true);
					} else {
						for (auto overlap : overlapVec) {
							overlap->start_vib();
						}
					}
				} else {
					Coordinate curSelectBlockCoord = cursorBlock->get_mapCoord();
					for (auto plane : planeVec[playerTurn]) {
						if (plane->inside(curSelectBlockCoord) && plane->get_status() == false) {
							curPlaneBlock = plane;
							curPlaneBlock->set_status(true);
							cursorBlock->set_status(false);
							break;
						}
					}
				}

				break;
		};
	}

}

void stage_control(Command cmd) {
	if (cmd == CMD_NEXTSTAGE) {
		switch (gameStage) {
			case SET:
				if (curPlaneBlock) break;
				for (auto blockRow : blockVec[playerTurn]) {
					for (auto block : blockRow) {
						block->start_ani_1();
					}
				}
				if (playerTurn == 0) {
					playerTurn = 1;
					cursorBlock->set_offset({SCREEN_X_MARGIN + playerTurn * SCREEN_WIDTH_SET, SCREEN_Y_MARGIN});
				} else {
					cout << "set to select" << endl;
					gameStage = SELECT;
				}
				break;
			case OVER:
				cout << "over to init" << endl;
				gameStage = INIT;
				break;
		}
	}
}

void check_reveal(Command cmd) {
	if (gameStage == SELECT) { 
		if (cmd == CMD_ENTER) {
			
			Coordinate curSelectBlockCoord = cursorBlock->get_mapCoord();

			if (blockVec[playerTurn][curSelectBlockCoord.y][curSelectBlockCoord.x]->get_status()) {

				attempts[playerTurn]++;
				attemptsUpdated = true;
				blockVec[playerTurn][curSelectBlockCoord.y][curSelectBlockCoord.x]->start_ani_2();
				for (auto plane : planeVec[playerTurn]) {
					if (plane->is_head(curSelectBlockCoord)) {
						plane->hit_head();
						hitPlanes[playerTurn]++;
					}
				}
				lastCursorMapCoord[playerTurn] = curSelectBlockCoord;
				playerTurn = (playerTurn == 0) ? 1 : 0;
				cursorBlock->set_mapCoord(lastCursorMapCoord[playerTurn]);
				cursorBlock->set_offset({SCREEN_X_MARGIN + playerTurn * SCREEN_WIDTH_SET, SCREEN_Y_MARGIN});
			}
		}
	}
}

void display() {
	for (int player = 0; player < totalPlayerNum; player++) {
		for (auto plane : planeVec[player]) {
			if (plane != curPlaneBlock) plane->draw();
		}
		for (int row = 0; row < MAP_HEIGHT; row++) {
			for (int col = 0; col < MAP_WIDTH; col++) {
				blockVec[player][row][col]->draw();
			}
		}
	}

	if (gameStage == SET && curPlaneBlock) curPlaneBlock->draw();

	if (!cursorBlock) {
		ERROR_MSG("Game: selectBlock uninitilized");
		return;
	}
	cursorBlock->draw();

	for (int player = 0; player < totalPlayerNum; player++) {

		SDL_Rect screenRect {SCREEN_X_MARGIN + player * SCREEN_WIDTH_SET, SCREEN_Y_MARGIN, MAP_WIDTH * BLOCK_WIDTH, MAP_HEIGHT * BLOCK_WIDTH};
		SDL_RenderCopy(gRenderer, bgTexture, NULL, &screenRect);
	}

	SDL_Rect screenRect {SCREEN_X_MARGIN + SCREEN_WIDTH_P, SCREEN_Y_MARGIN, SCREEN_WIDTH_INFO, MAP_HEIGHT * BLOCK_WIDTH};
	SDL_RenderCopy(gRenderer, infoTexture, NULL, &screenRect);

	if (gameStage != SET && gameStage != INIT) {
		for (int i = 0; i < planeVec.size(); i++) {
			SDL_Rect planeIconRect {SCREEN_X_MARGIN + SCREEN_WIDTH_P + 37, 200 + i * 150, planeVec[playerTurn][i]->get_width() / 2, planeVec[playerTurn][i]->get_height() / 2};
			SDL_RenderCopy(gRenderer, planeTextureVec[i], NULL, &planeIconRect);	
		}


		if (attemptsUpdated) {
			attemptsNumTotalWidth = 0;
			attemptsTextureVec.clear();
			attemptsNumInfoVec.clear();

			for (int player = 0; player < totalPlayerNum; player++) {
				int attemptsNumW, attemptsNumH;
				SDL_Texture* attemptsTexture = load_text(to_string(attempts[player]).c_str());
				attemptsTextureVec.push_back(attemptsTexture);
				SDL_QueryTexture(attemptsTexture, NULL, NULL, &attemptsNumW, &attemptsNumH); 
				attemptsNumInfoVec.push_back({attemptsNumW, attemptsNumH});
				attemptsNumTotalWidth += attemptsNumW;	
			}

			attemptsUpdated = false;
		}
		int margin = (SCREEN_WIDTH_INFO - attemptsNumTotalWidth - 60) / 2;
		for (int player = 0; player < totalPlayerNum; player++) {
			SDL_Rect attemptsNumRect {SCREEN_X_MARGIN + SCREEN_WIDTH_P + margin + 80 * player, 100, attemptsNumInfoVec[player].x, attemptsNumInfoVec[player].y};
			SDL_RenderCopy(gRenderer, attemptsTextureVec[player], NULL, &attemptsNumRect);
		}
	}
}



