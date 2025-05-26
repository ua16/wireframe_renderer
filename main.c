#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


// Other library imports
#include <math.h>
#include <stdio.h>

/* We will use this renderer to draw into this window every frame. */
#define SCREENWIDTH 1024 
#define SCREENHEIGHT 512
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

// Constants 
#define SPEED 10
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533

// Floating Vector2
typedef struct {
    float x; 
    float y; 
} fv2; 

// Floating Vector 3
typedef struct {
    float x;
    float y;
    float z;
} fv3;

// Camera
typedef struct {
    fv3 pos;
    float rotation;
} Camera;

// Store the global state of the program in this thing 
typedef struct {
} GlobalState;

// Store the global state between functions 
static GlobalState globalState;

// Z axis is perpendicular to the camera

// Cube vertices
fv3 cubeV[8] = {
    {-1, -1, -1},   // Left bottom back 
    { 1, -1, -1},   // right bottom back
    {-1, -1,  1},   // left bottom front
    { 1, -1,  1},   // right bottom front
    {-1,  1, -1},   // Left top back 
    { 1,  1, -1},   // right top back
    {-1,  1,  1},   // left top front
    { 1,  1,  1},   // right top front
};

// Cube edges
int cubeE[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
    {4, 5}, {5, 6}, {6, 7}, {4, 7}, // Top face
    {0, 4}, {1, 5}, {2, 6}, {3,7}   // Side lines
};

float FOV = 1.308997;

// Camera
Camera camera = { {0.0, 0.0, -5.0}, 0.9162979  };
// This rotation centers the camera for an FOV of 75 deg


// My functions --------------------------------------------------------- //

fv2 offsetToCenter(fv2 in) {
    return (fv2) {in.x + SCREENWIDTH/2 , in.y + SCREENHEIGHT/2};
}

// SDL Functions -------------------------------------------------------- //

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("SDL3 Test", "1.0", "com.test.of-SDL3");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("SDL3 Test", SCREENWIDTH, SCREENHEIGHT, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize MY stuff

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    if (event->type == SDL_EVENT_KEY_DOWN) {
        // Handle Key presses 
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */

static Uint64 previous = 0;
static Uint64 delta = 0;
const Uint64 sleepTime = 1000000000 / 60;
static Uint64 actSleepTime = 0;

SDL_AppResult SDL_AppIterate(void *appstate)
{
    // Time elapsed since start
    Uint64 now = SDL_GetTicksNS();  /* convert from milliseconds to seconds. */
    delta = now - previous;


    // Logic stuff

    // This basically changes the pen color
    SDL_SetRenderDrawColor(renderer, 11, 39, 120, SDL_ALPHA_OPAQUE);

    /* clear the window to the draw color. */
    SDL_RenderClear(renderer);

    // Keep white as the default draw color
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    // Render calls 



    /* put the newly-cleared rendering on the screen. */
    // This puts the buffer that we just rendered on to the screen.
    SDL_RenderPresent(renderer);

    previous = SDL_GetTicksNS();

    actSleepTime = sleepTime - previous + now;
    // we see if it's negative like this instead since unsigned
    if (actSleepTime < sleepTime) {
        SDL_DelayNS(actSleepTime);
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}
