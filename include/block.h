#ifndef BLOCK_H
#define BLOCK_H

#include "defines.h"
#include "helper.h"

class Block {
	protected:
		Coordinate mapCoord;

		Coordinate offset;	

		SDL_Texture* texture;
		SDL_Rect rect;
		bool status;
		bool inAni;

	public:
		Block(SDL_Texture* texture_, Coordinate mapCoord_, Coordinate offset_);
		
		void reset();

		void set_status(bool status_);
		bool get_status();
		void draw();

		void set_offset(Coordinate offset_);

		virtual void move(Command cmd);
		virtual void rotate();

		void start_ani_1();
		void start_ani_2();

		Coordinate get_mapCoord();
		void set_mapCoord(Coordinate mapCoord_);
	
	private:
		int ani1Cnt = ANI1_FRAMES * ANI1_PERIOD;
		int ani2Cnt = ANI1_FRAMES * ANI1_PERIOD;
};


#endif
