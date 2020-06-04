#ifndef PBP_H
#define PBP_H

/** The height and width of the main window. */
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
/** Number of random squares to plot per iteration. */
#define SQUARES_PER_CYCLE 50
#define SQUARE_MAX_SIZE 5
#define SQUARE_MIN_SIZE 1

/**
 * @brief Main window.
 * The main SDL window.
 */
SDL_Window *main_window = NULL;
/**
 * @brief Main screen.
 * The main SDL screen.
 */
SDL_Surface *main_screen;
/**
 * @brief Source and destination render surfaces.
 * Used for rendering the source and destination images.
 */
SDL_Surface *source_surface;
SDL_Surface *destination_surface;

#endif
