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

struct ParticleType {
	unsigned int bitmask;
	unsigned int color;
	unsigned int density;

	ParticleType() {
		this->bitmask = 0;
		this->color = 0;
		this->density = 0;
	}

	ParticleType(unsigned int bitmask, unsigned int color, unsigned int density) {
		this->bitmask = bitmask;
		this->color = color;
		this->density = density;
	}
};

#pragma once
class Game
{
private:
	bool running;
	InputState input;

	SDL_Renderer* renderer;

	unsigned int turn;

	const double frameRate = 0.4;
	const int window_width = 400;
	const int window_height = 400;

	const int width = 50;
	const int height = 50;

	const int rockColor = 0x646b70;
	const int woodColor = 0x5c421c;
	const int fireColor = 0xff8121;

	ParticleType sand = { 1, 0xfaf58e, 2 };
	ParticleType water = { 2, 0x4793c9, 1 };
	ParticleType stone = { 4,  0x646b70, 5 };
	ParticleType steam = { 5,  0xebebeb, 0 };
	ParticleType lava = { 6,  0xff8121, 1 };

	vector<unsigned int> map;

	ParticleType selectedParticle;

public:
	Game();
	int Execute();

	bool Init();
	void Update();
	void Render();

	void HandleEvent(SDL_Event* e);

	bool MovePixel(int src_x, int src_y, int target_x, int target_y);
	inline void ScreenPosToMapPos(int screen_x, int screen_y, int* map_x, int* map_y);
	inline float RandNextFloat();

private:
	inline int CoordsToIndex(int x, int y);
	inline void SetBit(unsigned int& value, unsigned int bitValue, int index);
	bool ApplyGravity(int x, int y);
	bool Float(int x, int y);
	void CreateParticle(ParticleType particleType, int x, int y);
};