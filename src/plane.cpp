#include "plane.h" 

Plane::Plane(SDL_Texture* texture_, SDL_Texture* selectTexture_, Coordinate mapCoord_, set<Coordinate> validBlocks_, Coordinate headBlock_, Coordinate offset_ = {0, 0}) : Block(texture_, mapCoord_, offset_) {
	mapWidth = rect.w / BLOCK_WIDTH;
	mapHeight = rect.h / BLOCK_WIDTH;
	selectTexture = selectTexture_;
	validBlocks = validBlocks_;
	headBlock = headBlock_;
	selected = false;
	angle = 0;
	adjOnRotation = {0, 0};
}

void Plane::reset() {
	selected = false;
	headGotHit = false;
}

void Plane::move(Command cmd) {
	switch (cmd) {
		case CMD_UP: 
			if (mapCoord.y > 0) {
				mapCoord.y--;
				rect.y -= BLOCK_WIDTH;
			}
			break;
		case CMD_DOWN:
			if (mapCoord.y + mapHeight < MAP_HEIGHT) {
				mapCoord.y++;
				rect.y += BLOCK_WIDTH;
			}
			break;
		case CMD_LEFT:
			if (mapCoord.x > 0) {
				mapCoord.x--;
				rect.x -= BLOCK_WIDTH;
			}
			break;
		case CMD_RIGHT:
			if (mapCoord.x + mapWidth < MAP_WIDTH) {
				mapCoord.x++;
				rect.x += BLOCK_WIDTH;
			}
			break;
	};
}

void Plane::rotate() {
	angle = (angle == 270) ? 0 : (angle + 90);

	set<Coordinate> newValidBlocks;
	for (auto validBlock : validBlocks) {
		Coordinate newCoord = {mapHeight - 1 - validBlock.y,
							   validBlock.x};
		newValidBlocks.insert(newCoord);
	}

	validBlocks = newValidBlocks;

	headBlock = {mapHeight - 1 - headBlock.y,
				 headBlock.x};

	mapHeight = mapHeight + mapWidth;
	mapWidth = mapHeight - mapWidth;
	mapHeight = mapHeight - mapWidth;

	int adj = abs(rect.h - rect.w) / 2;

	switch (angle) {
		case 90:
		case 270:
			adjOnRotation.x = -adj;
			adjOnRotation.y = adj;
			break;
		case 0:
		case 180:
			adjOnRotation.x = 0;
			adjOnRotation.y = 0;
			break;
	}

	if (mapCoord.x + mapWidth > MAP_WIDTH) {
		mapCoord.x--;
	}
	if (mapCoord.y + mapHeight > MAP_HEIGHT) {
		mapCoord.y--;
	}
}

void Plane::set_status(bool selected_) {selected = selected_;}

bool Plane::get_status() {return selected;}

int Plane::get_width() {return rect.w;}

int Plane::get_height() {return rect.h;}

bool Plane::inside(Coordinate coord) {
	set<Coordinate> validBlockSet;
	Coordinate relativeCoord = coord - mapCoord;

	if (validBlocks.find(relativeCoord) != validBlocks.end()) {
		return true;
	}

	return false;
}

bool Plane::overlap(const Plane* refPlane) const{
	Coordinate refPlaneCoord = refPlane->mapCoord;
	set<Coordinate> refValidBlockMapCoordSet;
	for (auto& refValidBlock : refPlane->validBlocks) {
		refValidBlockMapCoordSet.insert(refPlaneCoord + refValidBlock);
	}

	for (auto& validBlock : validBlocks) {
		if (refValidBlockMapCoordSet.find(mapCoord + validBlock) != refValidBlockMapCoordSet.end()) {
			return true;
		}
	}

	return false;
}

int vibSeq[] = {-5, 5, -5, 5, -5};

void Plane::draw() {
	rect.x = mapCoord.x * 50 + offset.x + adjOnRotation.x;
	rect.y = mapCoord.y * 50 + offset.y + adjOnRotation.y;

	if (SDL_RenderCopyEx(gRenderer, texture, NULL, &rect, angle, NULL, SDL_FLIP_NONE)) {
		SDL_ERROR_MSG("Block: SDL Failed to copy teture to renderer");
	}
	if (selected == true) {
		if (SDL_RenderCopyEx(gRenderer, selectTexture, NULL, &rect, angle, NULL, SDL_FLIP_NONE)) {
			SDL_ERROR_MSG("Block: SDL Failed to copy select teture to renderer");
		}
	} else if (vibCnt != VIB_TIME * VIB_CNT) {
		SDL_Rect newRect = rect;
		newRect.x += vibSeq[vibCnt / VIB_TIME];
		vibCnt += VIB_TIME;
		if (SDL_RenderCopyEx(gRenderer, selectTexture, NULL, &newRect, angle, NULL, SDL_FLIP_NONE)) {
			SDL_ERROR_MSG("Block: SDL Failed to copy select teture to renderer");
		}
	}

}

void Plane::start_vib() {
	vibCnt = 0;
}

bool Plane::is_head(Coordinate coord) const {
	return coord == headBlock + mapCoord;
}

void Plane::hit_head() {
	if (!headGotHit) {
		headGotHit = true;
		cout << "GOT HIT" << endl;
	}
}

void Plane::set_mapCoord(Coordinate mapCoord_) {
	mapCoord = mapCoord_;
	if (mapCoord_.x < 0) mapCoord.x = 0;
	if (mapCoord_.y < 0) mapCoord.y = 0;
	if (mapCoord_.x + mapWidth > MAP_WIDTH) mapCoord.x = MAP_WIDTH - mapWidth;
	if (mapCoord_.y + mapHeight > MAP_HEIGHT) mapCoord.y = MAP_HEIGHT - mapHeight;
}
