#ifndef PLANE_H
#define PLANE_H

#include "block.h"

#define VIB_TIME 30
#define VIB_CNT 5 

class Plane : public Block {
	private:
		int mapWidth;
		int mapHeight;
		bool selected = false;
		bool headGotHit = false;
		int angle;
		Coordinate adjOnRotation;
		SDL_Texture* selectTexture;
		set<Coordinate> validBlocks;
		Coordinate headBlock;
		int vibCnt = VIB_TIME * VIB_CNT;

	public:
		Plane(SDL_Texture* texture_, SDL_Texture* selectTexture_, Coordinate mapCoord_, set<Coordinate> validBlocks_, Coordinate headBlock_, Coordinate offset_);
		void reset();
		void move(Command cmd);
		void rotate();
		void set_status(bool selected_);
		bool get_status();
		int get_width();
		int get_height();
		bool inside(Coordinate coord);
		bool overlap(const Plane* refPlane) const;
		void draw();
		void start_vib();
		bool is_head(Coordinate coord) const;
		void hit_head();
};


#endif
