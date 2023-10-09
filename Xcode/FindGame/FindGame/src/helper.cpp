#include "helper.h"

SDL_Renderer* gRenderer;
SDL_Window* gWindow;
TTF_Font* gFont = NULL;

SDL_Texture* blockTexture;
SDL_Texture* cursorTexture;

SDL_Texture* bgTexture;
SDL_Texture* infoTexture;

vector<SDL_Texture*> planeTextureVec;
vector<SDL_Texture*> planeSelectTextureVec;
vector<set<Coordinate>> planeValidBlockVec;
vector<Coordinate> planeHeadVec;
vector<Coordinate> planePosVec;

SDL_Texture* ani1Textures[ANI1_FRAMES];
SDL_Texture* ani2Textures[ANI1_FRAMES];

int initialize_sdl() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_ERROR_MSG("SDL: Failed to init");
		return -1;
	}

	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	if (IMG_Init(flags) & flags != flags) {
		SDL_ERROR_MSG("IMG INIT: Failed to init IMG");
		return -1;
	}

	gWindow = SDL_CreateWindow("寻机头", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN/* | SDL_WINDOW_BORDERLESS*/);
	
	if (!gWindow) {
		SDL_ERROR_MSG("SDL: Failed to create window");
		return -1;
	}

	SDL_DisplayMode current;

	SDL_GetCurrentDisplayMode(0, &current);

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

	if (!gRenderer) {
		SDL_ERROR_MSG("SDL: Failed to create renderer");
		return -1;
	}

	SDL_SetRenderDrawColor(gRenderer, 0xFF ,0xFF, 0xFF, 0x00);


	if (TTF_Init() == -1) {
		SDL_ERROR_MSG("SDL: Failed to initialize ttf");
		return -1;
	}

	gFont = TTF_OpenFont("font/Ubuntu-L.ttf", 40);

	if (!gFont) {
		SDL_ERROR_MSG("TTF: Failed to load font");
		return -1;
	}

	return 0;
}

int initialize_img() {
	blockTexture = load_img("img/ani1-4.png");	
	if (!blockTexture) return -1;

	cursorTexture = load_img("img/cursor.png");
	if (!cursorTexture) return -1;

	bgTexture = load_img("img/bg.png");
	if (!bgTexture) return -1;


	string planeConfig, tempString;
	tempString += "plane0.png planeS0.png 0 0  2 0  0 1  1 1  2 1  3 1  4 1  2 2  1 3  2 3  3 3\n";
	tempString += "plane2.png planeS2.png 5 3  2 0  1 1  2 1  3 1  0 2  2 2  4 2  2 3  1 4  3 4\n";
	stringstream configStream(tempString);
	while (getline(configStream, planeConfig)) {
		stringstream ss(planeConfig);
		string planeImgName, planeSImgName;
		ss >> planeImgName >> planeSImgName;
			
		planeImgName = "img/" + planeImgName;
		planeSImgName = "img/" + planeSImgName;

		SDL_Texture* newPlaneTexture = load_img(planeImgName.c_str());
		SDL_Texture* newPlaneSelectTexture = load_img(planeSImgName.c_str());

		if (!newPlaneTexture || !newPlaneSelectTexture) return -1;

		planeTextureVec.push_back(newPlaneTexture);
		planeSelectTextureVec.push_back(newPlaneSelectTexture);

		set<Coordinate> newPlaneValidBlocks;
		int x, y;
		ss >> x >> y;
		planePosVec.push_back({x, y});
		bool head = true;
		while(ss) {
			ss >> x >> y;
			if (head) {
				planeHeadVec.push_back({x, y});
				head = false;
			}
			newPlaneValidBlocks.insert({x, y});
		}
		planeValidBlockVec.push_back(newPlaneValidBlocks);
	}

	for (int i = 0; i < ANI1_FRAMES; i++) {
		char ani1ImgName[20];
		sprintf(ani1ImgName, "img/ani1-%d.png", i);
		ani1Textures[i] = load_img(ani1ImgName);
		char ani2ImgName[20];
		sprintf(ani2ImgName, "img/ani2-%d.png", i);
		ani2Textures[i] = load_img(ani2ImgName);
	}

	infoTexture = load_img("img/info.png");

	return 0;
}

SDL_Texture* load_img(const char* name) {
	SDL_Surface* surface = IMG_Load(name);

	if (!surface) {
		SDL_ERROR_MSG("IMG: Failed to load image to surface");
		return NULL;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);

	if (!texture) {
		SDL_ERROR_MSG("SDL: Failed to create texture from surface");
		return NULL;
	}

	SDL_FreeSurface(surface);

	return texture;
}

SDL_Texture* load_text(const char* text) {

    SDL_Surface* surface = TTF_RenderText_Solid(gFont, text, {0, 0, 0});
    
    if (surface == NULL) {
        SDL_ERROR_MSG("TTF: Failed to load number to surface");
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);

    if (texture == NULL) {
        SDL_ERROR_MSG("SDL: Failed to crate texture from surface");
    }

    SDL_FreeSurface(surface);

    return texture;
}

void quit() {
	IMG_Quit();
	TTF_CloseFont(gFont);
	TTF_Quit();
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}
