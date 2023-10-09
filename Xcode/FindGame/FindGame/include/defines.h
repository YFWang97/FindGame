#ifndef DEFINE_H
#define DEFINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <set>

using namespace std;

#define SDL_ERROR_MSG(...) cout << __VA_ARGS__ << "! " << "SDL_Error: " << SDL_GetError() << endl;
#define ERROR_MSG(...) cout << __VA_ARGS__ << "! " << endl;
#define DEBUG_MSG(...) cout << "DEBUG: " << __VA_ARGS__ << "! " << endl

#define BLOCK_WIDTH 50
#define MAP_WIDTH 10
#define MAP_HEIGHT 10

#define SCREEN_WIDTH_INFO 200

#define SCREEN_WIDTH_P (MAP_WIDTH * BLOCK_WIDTH)

#define SCREEN_WIDTH_SET (SCREEN_WIDTH_P + SCREEN_WIDTH_INFO)

#define SCREEN_X_MARGIN 50
#define SCREEN_Y_MARGIN 50

#define SCREEN_WIDTH (SCREEN_WIDTH_P * 2 + SCREEN_WIDTH_INFO + SCREEN_X_MARGIN * 2)
#define SCREEN_HEIGHT (MAP_HEIGHT * BLOCK_WIDTH + SCREEN_Y_MARGIN * 2)


enum TileType {
	A,
	B,
	C,
	D
};

enum GameStage {
	INIT,
	SET,
	SELECT,
	OVER
};

enum Command {
	CMD_NONE,
	CMD_UP,
	CMD_DOWN,
	CMD_LEFT,
	CMD_RIGHT,
	CMD_ROTATE,
	CMD_ENTER,
	CMD_NEXTSTAGE.
	CMD_MOUSE_CLICK,
	CMD_MOUSE_MOTION
};

typedef struct coordinate {
	int x;
	int y;
	bool operator==(const struct coordinate& coord) const {
		return (x == coord.x && y == coord.y);
	}
	bool operator<(const struct coordinate& coord) const {
		if (y < coord.y) return true;
		if (y == coord.y && x < coord.x) return true;
		return false;
	}
	bool operator!=(const struct coordinate& coord) const {
		return (x != coord.x || y != coord.y);
	}
	struct coordinate operator-(const struct coordinate& coord) const {
		return {x - coord.x, y - coord.y};
	}
	struct coordinate operator+(const struct coordinate& coord) const {
		return {x + coord.x, y + coord.y};
	}
} Coordinate;

namespace std {
	template <>
	struct hash<struct coordinate> {
		size_t operator() (const struct coordinate& coord) const {
			return hash<int>()(coord.x) ^ hash<int>()(coord.y);
		}
	};
}

//Global Variables
extern bool gameQuit;

extern SDL_Renderer* gRenderer;
extern SDL_Window* gWindow;
extern TTF_Font* gFont;

extern SDL_Texture* blockTexture;
extern SDL_Texture* cursorTexture;

extern SDL_Texture* bgTexture;
extern SDL_Texture* infoTexture;

extern vector<SDL_Texture*> planeTextureVec;
extern vector<SDL_Texture*> planeSelectTextureVec;
extern vector<set<Coordinate>> planeValidBlockVec;
extern vector<Coordinate> planeHeadVec;
extern vector<Coordinate> planePosVec;

#define ANI1_FRAMES 5
#define ANI1_PERIOD 5

extern SDL_Texture* ani1Textures[ANI1_FRAMES];
extern SDL_Texture* ani2Textures[ANI1_FRAMES];

#endif
