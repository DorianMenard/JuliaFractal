#include <iostream>
#include <SDL2/SDL.h>
#include <Windows.h>
#include <string>
#include <omp.h>

int WIDTH = 800;
int HEIGHT = 800;

long double min = -1.5;
long double max = 1.5;
int MAX_ITERATIONS = 255;
int nb_threads = 5;

long double map(long double value, long double in_min, long double in_max, long double out_min, long double out_max) {
	return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void saveBMP(SDL_Window* window, SDL_Renderer* renderer, int count) {
	SDL_Surface* sshot = SDL_GetWindowSurface(window);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
	std::string file = std::to_string(count) + ".bmp";
	SDL_SaveBMP(sshot, file.c_str());
	SDL_FreeSurface(sshot);
}

int  main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;

	SDL_CreateWindowAndRenderer(1440, 900, 0, &window, &renderer);
	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

	int count = 0;

	while (1) {
		SDL_RenderPresent(renderer);

		#pragma omp parallel for num_threads(nb_threads)
		for (int x = 0; x < WIDTH; x++) {
			
			if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
				return 0;
			if (GetKeyState('Q') & 0x8000)
				return 0;
			for (int y = 0; y < HEIGHT; y++) {
			
				long double a = map(x, 0, WIDTH, min, max );
				long double b = map(y, 0, HEIGHT, min, max);

				long double ai = a;
				long double bi = b;

				int n = 0;

				for (int i = 0; i < MAX_ITERATIONS; i++) {

					long double a1 = a * a - b * b;
					long double b1 = 2 * a * b;

					//a = a1 + ai;
					//b = b1 + bi;

					a = a1 - 0.0849;
					b = b1 - 0.8721;

					if ((a + b) > 2) {
						break;
					}

					n++;
				}

				int bright = map(n, 0, MAX_ITERATIONS, 0 , 255);

				if ((n == MAX_ITERATIONS) || (bright < 20)) {
					bright = 0;
				}

				//Variable pour changement de couleur
				int red = map(bright * bright, 0, 6502, 0, 255);
				int green = bright;
				int blue = map(sqrt(bright), 0, sqrt(255), 0, 255);

				SDL_SetRenderDrawColor(renderer, bright, bright, bright, 255);
				SDL_RenderDrawPoint(renderer, x, y);

				
				
			}	
		}
		
		if (count == 0) {
			saveBMP(window, renderer, count);
		}
		
		count++;

	}
	return 0;
}