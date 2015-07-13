#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const Uint16 _windowWidth = 1024, _windowHeight = 768;
Uint8 _quit = 0;

SDL_Window *_window = NULL;
SDL_Event _event;

SDL_Surface *_screen;

SDL_Surface *srcSurface;
SDL_Surface *mainSurface;

SDL_Rect srcSurface_rect, mainSurface_rect, randomRect;

Uint16 srcWidth, srcHeight;

Uint16 mainSum, testSum;

Uint32 _color;
Uint8 _r, _g, _b, sr, sg, sb, mr, mg, mb;

Uint16 iw, ih;

Uint32 *_src_pixelData_ptr;
Uint32 *_main_pixelData_ptr;

char filenameBuffer[20];

const Uint8 _squaresPerCycle = 50;	// number of square plotting cycles per frame.
const Uint8 squareMaxSize = 6;
const Uint8 squareMinSize = 1;

void SDLFreeSurfaces() {
	SDL_FreeSurface(srcSurface);
	SDL_FreeSurface(mainSurface);
};


void SDLQuit() {
	SDLFreeSurfaces();

	SDL_FreeSurface(_screen);
	SDL_DestroyWindow(_window);
	SDL_Quit();
};


void SDLerror(const char *fmt, ...) {
	va_list args;
  va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
};


int SDLinit() {
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDLerror("SDL_Init error: %s\n",SDL_GetError());
		return 0;
	}

	_window = SDL_CreateWindow("Simulated Annealing square plotter!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _windowWidth, _windowHeight, SDL_WINDOW_SHOWN);
	if(!_window) {
		SDLerror("SDL_CreateWindow error: %s\n", SDL_GetError());
		return 0;
	}

    _screen = SDL_GetWindowSurface(_window);
	if(!_screen) {
		SDLerror("SDL_GetWindowSurface error: %s\n", SDL_GetError());
		return 0;
	}

	return 1;
};


SDL_Surface *main_loadSurface(char *file) {
	return IMG_Load(file);
};


void input() {
	while(SDL_PollEvent(&_event)) {

        switch(_event.type) {
        	case SDL_QUIT:
        		_quit = 1;
        		break;
            case SDL_KEYDOWN:
            	switch(_event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        _quit = 1;
                        break;
                    case SDLK_F12:
                    	sprintf(filenameBuffer, "%i_%i.bmp", (int)time(NULL),SDL_GetTicks());	// prints screenshot with name format <date>_<getTicks>.bmp
                    	SDL_SaveBMP(mainSurface,filenameBuffer);
                    	break;
            	}
            	break;
        }

	}
};

Uint8 _cycleIt;	// iterator - cycles per frame.
void update() {
	for(_cycleIt = 0; _cycleIt < _squaresPerCycle; _cycleIt++) {
		randomRect.x = rand()%srcWidth;
		randomRect.y = rand()%srcHeight;

		randomRect.w = squareMinSize+rand()%squareMaxSize;
		randomRect.h = squareMinSize+rand()%squareMaxSize;

		_r = rand()%255;
		_g = rand()%255;
		_b = rand()%255;

		testSum = 0;
		mainSum = 0;

		for(ih = 0; ih < randomRect.h; ih++) {
			for(iw = 0; iw < randomRect.w; iw++) {

				SDL_GetRGB( _src_pixelData_ptr[ (randomRect.y + ih) * srcSurface->w + (randomRect.x + iw) ], srcSurface->format,&sr,&sg,&sb);
				SDL_GetRGB( _main_pixelData_ptr[ (randomRect.y + ih) * mainSurface->w + (randomRect.x + iw) ], mainSurface->format,&mr,&mg,&mb);

				testSum += abs(sr - _r) + abs(sg - _g) + abs(sg - _b);
				mainSum += abs(sr - mr) + abs(sg - mg) + abs(sb - mb);

			}
		}

		if(testSum < mainSum) SDL_FillRect(mainSurface, &randomRect, SDL_MapRGB(mainSurface->format, _r, _g, _b));
	}
};


void (*render)();	//function pointer for render so that the program only uses the slower BlitScaled if the Surfaces are actually scaled.

void render_scaled() {
	SDL_BlitScaled(srcSurface,NULL,_screen,&srcSurface_rect);
	SDL_BlitScaled(mainSurface,NULL,_screen,&mainSurface_rect);
	SDL_UpdateWindowSurface(_window);
}

void render_normal() {
	SDL_BlitSurface(srcSurface,NULL,_screen,&srcSurface_rect);
	SDL_BlitSurface(mainSurface,NULL,_screen,&mainSurface_rect);
	SDL_UpdateWindowSurface(_window);
}


int main(int argc, char *argv[]) {
	if(!SDLinit()) {
		SDLQuit();
		exit(1);
	}

	if(argc < 2) {
		SDLerror("No filename argument supplied!\nPlease use the input filename as the first argument.\n");
		SDLQuit();
		exit(1);
	}

	srcSurface = main_loadSurface(argv[1]);
	if(!srcSurface) {
		SDLerror("File could not be opened!\n");
		SDLQuit();
		exit(1);
	}

	Uint32 start = SDL_GetTicks();

	mainSurface = SDL_CreateRGBSurface(0, srcSurface->w, srcSurface->h, 32, 0, 0, 0, 0);
	srcSurface = SDL_ConvertSurface(srcSurface, mainSurface->format, 0);

	_src_pixelData_ptr = (Uint32 *)srcSurface->pixels;	// pointers to the pixel data for both surfaces.
	_main_pixelData_ptr = (Uint32 *)mainSurface->pixels;

	srcWidth = srcSurface->w;
	srcHeight = srcSurface->h;

	srcSurface_rect.x = 10;
	srcSurface_rect.y = 10;

	if(srcWidth > 500) {	// image scaling to fit on screen.
		srcSurface_rect.w = 500;
		srcSurface_rect.h = (srcHeight * (500.0f / srcWidth));
		render = render_scaled;
	} else {
		srcSurface_rect.w = srcWidth;
		srcSurface_rect.h = srcHeight;
		render = render_normal;
	}

	mainSurface_rect.x = 20 + srcSurface_rect.w;
	mainSurface_rect.y = 10;

	mainSurface_rect.w = srcSurface_rect.w;
	mainSurface_rect.h = srcSurface_rect.h;

	while(!_quit) {
		input();
		update();
		render();
	}

	SDLQuit();
	return 0;
	
}
