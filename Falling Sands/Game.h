#include "SDL.h"
#include <vector> 
#include <stdlib.h>

using std::vector;

struct InputState {
	int x, y;
	bool leftMouseDown, rightMouseDown;
	InputState() {
		x = 0;
		y = 0;
		leftMouseDown = false;
		rightMouseDown = false;
	}
};

#pragma once
class Game
{	
private:
	bool running;
	InputState input;

	SDL_Renderer* renderer;

	const double frameRate = 0.4;
	const int window_width = 400;
	const int window_height = 400;

	const int width = 50;
	const int height = 50;

	vector<unsigned int> map;

	unsigned int activeColor;

	inline int CoordsToIndex(int x, int y);

public:
	const unsigned int sandColor = 0xfaf58e;
	const unsigned int waterColor = 0x4793c9;
	const unsigned int rockColor = 0x646b70;
	const unsigned int woodColor = 0x5c421c;
	const unsigned int fireColor = 0xff8121;
	const unsigned int steamColor = 0xebebeb;

	Game();
	int Execute();

	bool Init();
	void Update();
	void Render();

	void HandleEvent(SDL_Event* e);

	bool MovePixel(int src_x, int src_y, int target_x, int target_y);
	inline void ScreenPosToMapPos(int screen_x, int screen_y, int* map_x, int* map_y);
	inline float RandNextFloat();
};