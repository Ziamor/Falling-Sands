#include "Game.h"

Game::Game() {
	running = true;
	renderer = NULL;
	selectedParticle = sand;
}

int Game::Execute() {
	if (!Init()) {
		return -1;
	}

	SDL_Event Event;

	Uint64  frameStart;
	Uint64  frameEnd;

	srand(0xdeadbeef);
	while (running) {
		while (SDL_PollEvent(&Event)) {
			HandleEvent(&Event);
		}
		frameStart = SDL_GetPerformanceCounter();

		double frameTime = 0;
		int i = 0;
		while (frameTime < frameRate) {
			i++;
			Update();
			frameEnd = SDL_GetPerformanceCounter();
			frameTime += (double)((frameEnd - frameStart) * 1000 / (double)SDL_GetPerformanceFrequency());
		}

		Render();
	}

	SDL_Quit();

	return 0;
}

bool Game::Init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return false;
	SDL_Window* window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN);

	if (window == NULL)
		return false;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	map.resize(width * height, 0);

	return true;
}

void Game::HandleEvent(SDL_Event* e) {
	if (e->type == SDL_QUIT)
		running = false;
	else if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			input.leftMouseDown = true;
		}
		else if (e->button.button == SDL_BUTTON_RIGHT) {
			input.rightMouseDown = true;
		}
	}
	else if (e->type == SDL_MOUSEBUTTONUP) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			input.leftMouseDown = false;
		}
		else if (e->button.button == SDL_BUTTON_RIGHT) {
			input.rightMouseDown = false;
		}
	}
	else if (e->type == SDL_MOUSEMOTION) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		ScreenPosToMapPos(x, y, &input.x, &input.y);
	}
	else if (e->type == SDL_KEYDOWN) {
		switch (e->key.keysym.sym) {
		case SDLK_1:
			selectedParticle = sand;
			break;
		case SDLK_2:
			selectedParticle = water;
			break;
		case SDLK_3:
			selectedParticle = stone;
			break;
		case SDLK_4:
			selectedParticle = lava;
			break;
		case SDLK_5:
			selectedParticle = steam;
			break;
		case SDLK_6:
			//selectedParticle = 6;
			break;
		}
	}
}

void Game::Update() {
	int index;
	float randFloat = RandNextFloat();
	unsigned int oldTurn = turn;
	turn ^= 1;

	for (int x = 0; x < width; x++) {
		for (int y = height - 1; y >= 0; y--) {
			index = CoordsToIndex(x, y);
			if (map[index] == 0) continue;

			unsigned int particleType = map[index] & 0xf;
			unsigned int particleTurn = map[index] >> 31;

			// Check if we already ran the simulation this round for the particle
			if (particleTurn == turn)
				continue;

			// Set the turn			
			SetBit(map[index], turn, 31);

			if (particleType == sand.bitmask) {
				ApplyGravity(x, y);
			}
			else if (particleType == water.bitmask || particleType == lava.bitmask) {
				if (!ApplyGravity(x, y)) {
					if (randFloat < 0.5) {
						if (!MovePixel(x, y, x + 1, y)) {
							MovePixel(x, y, x - 1, y);
						}
					}
					else {
						if (!MovePixel(x, y, x - 1, y)) {
							MovePixel(x, y, x + 1, y);
						}
					}
				}
			}
			else if (particleType == steam.bitmask) {
				if (!Float(x, y)) {
					if (randFloat < 0.5) {
						if (!MovePixel(x, y, x + 1, y)) {
							MovePixel(x, y, x - 1, y);
						}
					}
					else {
						if (!MovePixel(x, y, x - 1, y)) {
							MovePixel(x, y, x + 1, y);
						}
					}
				}
			}
		}
	}

	/*for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			index = CoordsToIndex(x, y);
			if (map[index] == steamColor) {
				if (!MovePixel(x, y, x, y - 1)) {
					if (!MovePixel(x, y, x - 1, y - 1)) {
						if (!MovePixel(x, y, x + 1, y - 1)) {
							if (randFloat < 0.5) {
								if (!MovePixel(x, y, x + 1, y)) {
									MovePixel(x, y, x - 1, y);
								}
							}
							else {
								if (!MovePixel(x, y, x - 1, y)) {
									MovePixel(x, y, x + 1, y);
								}
							}
						}
					}
				}
			}
		}
	}*/

	if (input.leftMouseDown) {
		CreateParticle(selectedParticle, input.x, input.y);
		/*map[CoordsToIndex(input.x, input.y)] = selectedParticle;
		SetBit(map[CoordsToIndex(input.x, input.y)], turn, 31);*/
	}
	else if (input.rightMouseDown) {
		map[CoordsToIndex(input.x, input.y)] = 0;
	}
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_RenderClear(renderer);

	for (int x = 0; x < window_width; x++) {
		for (int y = 0; y < window_height; y++)
		{
			int map_x;
			int map_y;
			ScreenPosToMapPos(x, y, &map_x, &map_y);
			int c = 0;
			int particleType = map[map_x + map_y * width] & 0xf;

			if (particleType == sand.bitmask) {
				c = sand.color;
			}
			else if (particleType == water.bitmask) {
				c = water.color;
			}
			else if (particleType == stone.bitmask) {
				c = stone.color;
			}
			else if (particleType == steam.bitmask) {
				c = steam.color;
			}
			else if (particleType == lava.bitmask) {
				c = lava.color;
			}

			int r = (c & 0xff0000) >> 16;
			int g = (c & 0xff00) >> 8;
			int b = c & 0xff;

			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderDrawPoint(renderer, x, y);
		}
	}

	SDL_RenderPresent(renderer);
}

inline int Game::CoordsToIndex(int x, int y)
{
	return x + y * width;
}

bool Game::MovePixel(int src_x, int src_y, int target_x, int target_y)
{
	if (target_y < 0 || target_y >= height || target_x < 0 || target_x >= width)
		return false;

	int src_index = CoordsToIndex(src_x, src_y);
	int target_index = CoordsToIndex(target_x, target_y);

	if (map[target_index] != 0) {

		unsigned int src_density = (map[src_index] & 0xff0000) >> 16;
		unsigned int target_density = (map[target_index] & 0xff0000) >> 16;

		if ((map[target_index] & 0xff) == lava.bitmask && (map[src_index] & 0xff) == water.bitmask) {
			CreateParticle(steam, src_x, src_y);
			CreateParticle(stone, target_x, target_y);
		}
		else if (src_density > target_density&& src_x == target_x && target_y > src_y) {
			unsigned int old_value = map[target_index];
			map[target_index] = map[src_index];
			map[src_index] = old_value;
		}
		else {
			return false;
		}
		/*if (map[target_index] == waterColor && map[src_index] != waterColor && src_x == target_x && target_y > src_y) {
			map[target_index] = map[src_index];
			map[src_index] = waterColor;
		}
		else if (map[target_index] == fireColor && map[src_index] == waterColor) {
			map[src_index] = steamColor;
		}
		else {
			return false;
		}*/
	}
	else {
		map[target_index] = map[src_index];
		map[src_index] = 0;
	}

	return true;
}


inline void Game::ScreenPosToMapPos(int screen_x, int screen_y, int* map_x, int* map_y)
{
	*map_x = width * (screen_x / (float)window_width);
	*map_y = height * (screen_y / (float)window_height);
}


inline float Game::RandNextFloat()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}


inline void Game::SetBit(unsigned int& value, unsigned int bitValue, int index)
{
	value ^= ((~(unsigned long)bitValue + 1) ^ value) & (1UL << index);
}


bool Game::ApplyGravity(int x, int y)
{
	if (MovePixel(x, y, x, y + 1))
		return true;

	float randFloat = RandNextFloat();

	if (randFloat < 0.5) {
		if (MovePixel(x, y, x - 1, y + 1))
			return true;
		return MovePixel(x, y, x + 1, y + 1);
	}
	else {
		if (MovePixel(x, y, x + 1, y + 1))
			return true;
		return MovePixel(x, y, x - 1, y + 1);
	}

	return false;
}

bool Game::Float(int x, int y)
{
	if (MovePixel(x, y, x, y - 1))
		return true;

	float randFloat = RandNextFloat();

	if (randFloat < 0.5) {
		if (MovePixel(x, y, x - 1, y - 1))
			return true;
		return MovePixel(x, y, x + 1, y - 1);
	}
	else {
		if (MovePixel(x, y, x + 1, y - 1))
			return true;
		return MovePixel(x, y, x - 1, y - 1);
	}

	return false;
}

void Game::CreateParticle(ParticleType particleType, int x, int y)
{
	unsigned int newParticle = particleType.bitmask;
	SetBit(newParticle, turn, 31);
	newParticle += particleType.density << 16;
	map[CoordsToIndex(x, y)] = newParticle;
}
