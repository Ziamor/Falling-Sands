#include "Game.h"

Game::Game() {
	running = true;
	renderer = NULL;
	activeColor = sandColor;
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
		printf("%i\n", rand());
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
			activeColor = sandColor;
			break;
		case SDLK_2:
			activeColor = waterColor;
			break;
		case SDLK_3:
			activeColor = rockColor;
			break;
		case SDLK_4:
			activeColor = woodColor;
			break;
		case SDLK_5:
			activeColor = fireColor;
			break;
		case SDLK_6:
			activeColor = steamColor;
			break;
		}
	}
}

void Game::Update() {
	int index;
	float randFloat = RandNextFloat();
	for (int x = width - 1; x >= 0; x--) {
		for (int y = height - 1; y >= 0; y--) {
			index = CoordsToIndex(x, y);
			if (map[index] == sandColor) {
				if (!MovePixel(x, y, x, y + 1)) {
					if (!MovePixel(x, y, x - 1, y + 1)) {
						MovePixel(x, y, x + 1, y + 1);
					}
				}
			}
			else if (map[index] == waterColor) {
				if (!MovePixel(x, y, x, y + 1)) {
					if (!MovePixel(x, y, x - 1, y + 1)) {
						if (!MovePixel(x, y, x + 1, y + 1)) {
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
	}

	for (int x = 0; x < width; x++) {
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
	}

	if (input.leftMouseDown) {
		map[CoordsToIndex(input.x, input.y)] = activeColor;
	}
	else if (input.rightMouseDown) {
		map[CoordsToIndex(input.x, input.y)] = waterColor;
	}
	//map[CoordsToIndex(width / 2, height / 2)] = sandColor;
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
			unsigned int c = map[map_x + map_y * width];
			unsigned int r = (c & 0xff0000) >> 16;
			unsigned int g = (c & 0xff00) >> 8;
			unsigned int b = c & 0xff;

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
		if (map[target_index] == waterColor && map[src_index] != waterColor && src_x == target_x && target_y > src_y) {
			map[target_index] = map[src_index];
			map[src_index] = waterColor;
		}
		else if(map[target_index] == fireColor && map[src_index] == waterColor){
			map[src_index] = steamColor;
		}
		else {
			return false;
		}
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
