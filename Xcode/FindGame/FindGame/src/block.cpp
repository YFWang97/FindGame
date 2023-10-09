#include "block.h"

Block::Block(SDL_Texture* texture_, Coordinate mapCoord_, Coordinate offset_= {0, 0}) {
	if (!texture_) ERROR_MSG("Block: invalid texture");

	texture = texture_; 
	
	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));

	mapCoord = mapCoord_;

	if (mapCoord.x > MAP_WIDTH || mapCoord.y > MAP_HEIGHT) {
		ERROR_MSG("Tile: Invalid map coord");
	}

	offset = offset_;

	status = false;
	inAni = false;
}

void Block::reset() {
	status = false;
	inAni = false;	
}

void Block::set_status(bool status_) {status = status_;}

bool Block::get_status() {return status && (inAni == false);}

void Block::set_offset(Coordinate offset_) {offset = offset_;}

void Block::move(Command cmd) {
	switch (cmd) {
		case CMD_UP: 
			if (mapCoord.y > 0) mapCoord.y--;
			break;
		case CMD_DOWN:
			if (mapCoord.y < MAP_HEIGHT - 1) mapCoord.y++;
			break;
		case CMD_LEFT:
			if (mapCoord.x > 0) mapCoord.x--;
			break;
		case CMD_RIGHT:
			if (mapCoord.x < MAP_WIDTH - 1) mapCoord.x++;
			break;
	};
}

void Block::rotate() {

}

void Block::start_ani_1() {
	inAni = true;
	ani1Cnt = false;	
}

void Block::start_ani_2() {
	inAni = true;
	ani2Cnt = false;
}

Coordinate Block::get_mapCoord() {return mapCoord;}

void Block::set_mapCoord(Coordinate mapCoord_) {mapCoord = mapCoord_;}

void Block::draw() {
	//printf("Drawing to %d, %d, %d, %d\n", rect.x, rect.y, rect.h, rect.w);
	rect.x = mapCoord.x * BLOCK_WIDTH + offset.x;
	rect.y = mapCoord.y * BLOCK_WIDTH + offset.y;
	if (ani1Cnt != ANI1_FRAMES * ANI1_PERIOD) {
		if (SDL_RenderCopy(gRenderer, ani1Textures[ani1Cnt / ANI1_PERIOD], NULL, &rect)) {
			SDL_ERROR_MSG("Block: SDL Failed to coy teture to renderer");
		}
		ani1Cnt++;
		if (ani1Cnt == ANI1_FRAMES * ANI1_PERIOD - 1) {
			set_status(true);
			inAni = false;
		}
		return;
	}
	if (ani2Cnt != ANI1_FRAMES * ANI1_PERIOD) {
		if (SDL_RenderCopy(gRenderer, ani2Textures[ani2Cnt / ANI1_PERIOD], NULL, &rect)) {
			SDL_ERROR_MSG("Block: SDL Failed to coy teture to renderer");
		}
		ani2Cnt++;
		if (ani2Cnt == ANI1_FRAMES * ANI1_PERIOD - 1) {
			set_status(false);
			inAni = false;
		}
		return;
	}
	if (status == false) return;
	if (SDL_RenderCopy(gRenderer, texture, NULL, &rect)) {
		SDL_ERROR_MSG("Block: SDL Failed to coy teture to renderer");
	}
}
