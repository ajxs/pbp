#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "pbp.h"

/**
 * cleanup_program
 */
static void cleanup_program(void);

/**
 * @brief Initialises SDL.
 * Initialises all of the required SDL2 subsystems.
 */
static int initialise_sdl(void);

/**
 * @brief Prints an error to STDERR.
 * Used to log any error encountered to the console.
 */
static void print_error(const char *fmt, ...);

/**
 * @brief Render function pointer.
 * Function pointer for the main render function so that the program only uses
 * the slower BlitScaled if the Surfaces are actually scaled.
 */
static void (*render)(SDL_Rect *rect);

/**
 * @brief
 *
 */
static void render_scaled(SDL_Rect *rect);

/**
 * @brief
 *
 */
static void render_normal(SDL_Rect *rect);

/**
 * cleanup_program
 */
static void cleanup_program(void)
{
	// Free surfaces.
	SDL_FreeSurface(source_surface);
	SDL_FreeSurface(destination_surface);
	SDL_FreeSurface(main_screen);

	SDL_DestroyWindow(main_window);
	SDL_Quit();
};


/**
 * print_error
 */
static void print_error(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
};


/**
 * initialise_sdl
 */
static int initialise_sdl(void)
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		print_error("SDL_Init error: %s\n",SDL_GetError());
		return 0;
	}

	main_window = SDL_CreateWindow("pbp", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	main_screen = SDL_GetWindowSurface(main_window);
	if(!main_screen || !main_window) {
		print_error("initialise_sdl: error: %s\n", SDL_GetError());
		return 0;
	}

	return 1;
}


/*
 * render_scaled
 */
static void render_scaled(SDL_Rect *rect)
{
	SDL_BlitScaled(destination_surface, NULL, main_screen, rect);
	SDL_UpdateWindowSurface(main_window);
}


/**
 * render_normal
 */
static void render_normal(SDL_Rect *rect)
{
	SDL_BlitSurface(destination_surface, NULL, main_screen, rect);
	SDL_UpdateWindowSurface(main_window);
}


/**
 * main
 */
int main(int argc,
	char *argv[])
{
	/**
	 * @brief Tracks whether the destination image has updated.
	 * Tracks whether the plotting process has updated. This boolean value is used
	 * to check whether the screen should be updated. This is used since there
	 * may be dozens of rectangles plotted per iteration. The destination surface
	 * does not need to be re-rendered each time.
	 */
	bool destination_img_updated = false;
	/**
	 * @brief Sentinel value for program exit.
	 * Used to store whether the signal has been given to quit the program.
	 */
	bool exit_flag = false;
	/**
	 * @brief Main SDL event element.
	 * Used to contain any events that occur.
	 */
	SDL_Event main_event;
	/**
	 * @brief Pixel summation values
	 * Used to calculate the heuristic distance between the current progress and
	 * the next tested iteration.
	 */
	uint16_t current_rect_sum = 0;
	uint16_t test_rect_sum = 0;
	/**
	 * @brief Output filename buffer.
	 * Used to store the name of the output file when a screenshot is taken.
	 */
	char output_filename_buf[20];
	/**
	 * @brief The upper bound of the rectangle of pixels being checked.
	 * These values store the upper bound of the rectangle being checked.
	 * Essentially the x,y positions plus width, height.
	 */
	uint32_t upper_bound_x = 0;
	uint32_t upper_bound_y = 0;
	/**
	 * @brief The randomly plotted colour.
	 * This variable stores the colour for the randomly plotted rectangle.
	 */
	SDL_Colour random_colour;
	/**
	 * @brief The source pixel colour.
	 * This variable stores the colour of the current pixel being checked
	 * from the source image.
	 */
	SDL_Colour source_colour;
	/**
	 * @brief The destination pixel colour.
	 * This variable stores the colour of the current pixel being checked
	 * in the destination image surface.
	 */
	SDL_Colour dest_colour;
	/**
	 * @brief The randomly plotted rectangle.
	 * This variable stores the dimensions and positions of the randomly plotted
	 * rectangle on the destination canvas.
	 */
	SDL_Rect randomRect;
	/**
	 * @brief Source and destination image rectangles.
	 * Used to track the image boundaries of the source and destination images.
	 */
	SDL_Rect source_surface_rect;
	SDL_Rect destination_surface_rect;

	if(argc < 2) {
		print_error("No filename argument supplied!\n"
			"Please use the input filename as the first argument.\n");
		exit(EXIT_FAILURE);
	}

	if(!initialise_sdl()) {
		cleanup_program();
		exit(EXIT_FAILURE);
	}

	if(!(source_surface = IMG_Load(argv[1]), source_surface)) {
		print_error("File could not be opened!\n");
		cleanup_program();
		exit(EXIT_FAILURE);
	}

	destination_surface = SDL_CreateRGBSurface(0, source_surface->w, source_surface->h, 32, 0, 0, 0, 0);
	source_surface = SDL_ConvertSurface(source_surface, destination_surface->format, 0);

	source_surface_rect.x = 10;
	source_surface_rect.y = 10;

	// Scale image to fit on screen.
	if(source_surface->w > 500) {
		source_surface_rect.w = 500;
		source_surface_rect.h = (source_surface->h * (500.0f / source_surface->w));
		render = render_scaled;
	} else {
		source_surface_rect.w = source_surface->w;
		source_surface_rect.h = source_surface->h;
		render = render_normal;
	}

	destination_surface_rect.x = 20 + source_surface_rect.w;
	destination_surface_rect.y = 10;

	destination_surface_rect.w = source_surface_rect.w;
	destination_surface_rect.h = source_surface_rect.h;

	// blit source_surface once here since it never actually changes.
	SDL_BlitSurface(source_surface, NULL, main_screen, &source_surface_rect);

	while(!exit_flag) {
		// poll input.
		while(SDL_PollEvent(&main_event)) {
			switch(main_event.type) {
				case SDL_QUIT:
					exit_flag = true;
					break;
				case SDL_KEYDOWN:
					switch(main_event.key.keysym.sym) {
						case SDLK_ESCAPE:
							exit_flag = true;
							break;
						case SDLK_F12:
							// prints screenshot with name format <date>_<getTicks>.bmp
							sprintf(output_filename_buf, "%i_%i.bmp", (int)time(NULL), SDL_GetTicks());
							SDL_SaveBMP(destination_surface, output_filename_buf);
							break;
						}
					break;
				default:
					break;
			}
		}

		for(uint8_t i = 0; i < SQUARES_PER_CYCLE; i++) {
			// Reset 'updated' flag variable.
			destination_img_updated = false;

			// Get random rectangle dimensions.
			randomRect.w = SQUARE_MIN_SIZE + (rand() % SQUARE_MAX_SIZE);
			randomRect.h = SQUARE_MIN_SIZE + (rand() % SQUARE_MAX_SIZE);

			// Get random rectangle position.
			randomRect.x = rand() % (source_surface->w - randomRect.w);
			randomRect.y = rand() % (source_surface->h - randomRect.h);

			// Get random rectangle colour.
			random_colour.r = rand() % 255;
			random_colour.g = rand() % 255;
			random_colour.b = rand() % 255;

			// Reset pixel sum values.
			test_rect_sum = 0;
			current_rect_sum = 0;

			// Set range values.
			upper_bound_y = (randomRect.y + randomRect.h);
			upper_bound_x = (randomRect.x + randomRect.w);

			for(uint32_t ih = randomRect.y; ih < upper_bound_y; ih++) {
				for(uint32_t iw = randomRect.x; iw < upper_bound_x; iw++) {
					// Get the colour values from the source pixel.
					SDL_GetRGB(((Uint32*)source_surface->pixels)[ih * source_surface->w + iw],
						source_surface->format, &source_colour.r, &source_colour.g, &source_colour.b);

					// Get the colour values from the destination pixel.
					SDL_GetRGB(((Uint32*)destination_surface->pixels)[ih * source_surface->w + iw],
						destination_surface->format, &dest_colour.r, &dest_colour.g, &dest_colour.b);

					// Add to the test rectangle sum.
					test_rect_sum += abs(source_colour.r - random_colour.r) +
						abs(source_colour.g - random_colour.g) + abs(source_colour.b - random_colour.b);

					// Add to the current rectangle sum.
					current_rect_sum += abs(source_colour.r - dest_colour.r) +
						abs(source_colour.g - dest_colour.g) + abs(source_colour.b - dest_colour.b);
				}
			}

			// Check distance heuristic.
			// Check if the sum of the differences in pixels is lower in the random
			// rectangle being tested than in the current best effort.
			if(test_rect_sum < current_rect_sum) {
				SDL_FillRect(destination_surface, &randomRect, SDL_MapRGB(destination_surface->format,
					random_colour.r, random_colour.g, random_colour.b));

				// Since we've changed the destination surface, instruct the program
				// to re-render the destination image.
				destination_img_updated = true;
			}
		}

		// only re-render on update.
		if(destination_img_updated) {
			render(&destination_surface_rect);
		}
	}

	cleanup_program();
	return 0;

};
