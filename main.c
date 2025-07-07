#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <stdlib.h>

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


// Other library imports
#include <math.h>
#include <stdio.h>

// my library imports
#include "vector.h"

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
    {0, 1}, {1, 3}, {3, 2}, {2, 0}, // Bottom face
    {4, 5}, {5, 7}, {7, 6}, {6, 4}, // Top face
    {0, 4}, {1, 5}, {2, 6}, {3, 7}   // Side lines
};

// Pyramid vertices

fv3 pyramidV[5] = {

    {-1, -1, -1},   // 0: back left of base
    { 1, -1, -1},   // 1: back right of base
    { 1, -1,  1},   // 2: front right of base
    {-1, -1,  1},   // 3: front left of base
    { 0,  1,  0}    // 4: apex (top point)
};

// Pyramid edges
int pyramidE[8][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // base square
    {0, 4}, {1, 4}, {2, 4}, {3, 4}  // sides to apex
};

float fov = 1.308997; // 75 degrees
float focalLength = 0.63; // This gives us a screen length of about 1
static float moveAway = 5;

// Camera
Camera camera = { {0.0, 0.0, -5.0}, 0 };

// Obj loading stuff

typedef struct {
    int vCount;
    fv3 *v; // Pointer to the beginning of the array of vertices
    int eCount; // No. of edges * 2
    int *e; // Like above for the array of edges
    fv3 pos;
    double yaw;
    double pitch;
} Obj3D;

Obj3D* createTestCube(fv3 position) {
    Obj3D* newObj = malloc(sizeof(Obj3D));

    newObj->pos = position;

    newObj->vCount = 8;
    newObj->v = malloc(sizeof(fv3)*8);
    for (int i = 0; i < 8; i++) {
        newObj->v[i] = cubeV[i];
    }

    newObj->eCount = 24;
    newObj->e = malloc(sizeof(int)*24);
    for (int i = 0; i < 12; i++){
        newObj->e[i*2] = cubeE[i][0]; 
        newObj->e[i*2+1] = cubeE[i][1]; 
    }
    return newObj;
}

Obj3D* createTestPyramid(fv3 position) {
    Obj3D* newObj = malloc(sizeof(Obj3D));

    newObj->pos = position;

    newObj->vCount = 5;
    newObj->v = malloc(sizeof(fv3)*5);
    for (int i = 0; i < 8; i++) {
        newObj->v[i] = pyramidV[i];
    }

    newObj->eCount = 16;
    newObj->e = malloc(sizeof(int)*16);
    for (int i = 0; i < 8; i++){
        newObj->e[i*2] = pyramidE[i][0]; 
        newObj->e[i*2+1] = pyramidE[i][1]; 
    }
    return newObj;
}

void freeObj3D(Obj3D * obj) {
    free(obj->v);
    free(obj->e);
    free(obj);
}

// My functions --------------------------------------------------------- //

fv3 transformVector(fv3 ihat, fv3 jhat, fv3 khat, fv3 v) {
    fv3 i, j, k;
    i = scalefv3(ihat, v.x);
    j = scalefv3(jhat, v.y);
    k = scalefv3(khat, v.z);

    // Add the vectors together
    i = addfv3(i, j);
    i = addfv3(i, k);
    return i;
}

BasisVectors getBasisVectors (double yaw, double pitch) {
    BasisVectors out,rot1,rot2;
    // --- Yaw ---
    rot1.ihat = (fv3) {(float) cos(yaw), 0, (float) sin(yaw)};
    rot1.jhat = (fv3) {0,1,0};
    rot1.khat = (fv3) {(float) sin(yaw) * -1, 0, (float) cos(yaw)};
    // --- Pitch ---
    rot2.ihat = (fv3) {1, 0, 0};
    rot2.jhat = (fv3) {0, cos(pitch), sin(pitch) * -1};
    rot2.khat = (fv3) {0, sin(pitch), cos(pitch)};

    // --- Yaw and Pitch Combined ---
    out.ihat = transformVector(rot1.ihat, rot1.jhat, rot1.khat, rot2.ihat);
    out.jhat = transformVector(rot1.ihat, rot1.jhat, rot1.khat, rot2.jhat);
    out.khat = transformVector(rot1.ihat, rot1.jhat, rot1.khat, rot2.khat);
    
    return out;
}

// Convert the 3D points of mesh to 2d Points and render them
fv3 vertexToWorld(fv3 vertex, fv3 worldPos, double yaw, double pitch) {
    // worldPos is the position of the mesh in world space
    BasisVectors basis = getBasisVectors(yaw, pitch);
    vertex = transformVector(basis.ihat, basis.jhat, basis.khat, vertex);
    vertex.x += worldPos.x;
    vertex.y += worldPos.y;
    vertex.z += worldPos.z;
    return vertex;
}
fv2 vertexToScreen(fv3 vertex, fv3 worldPos, double yaw, double pitch) {

    fv3 vertexWorld = vertexToWorld(vertex, worldPos, yaw, pitch);

    float screenHeightWorld = tan(fov / 2) * 2;
    float pixelsPerWorldUnit = (float) SCREENHEIGHT / screenHeightWorld / vertexWorld.z;
    
    fv2 pixelOffset = {vertexWorld.x * pixelsPerWorldUnit, vertexWorld.y * pixelsPerWorldUnit};
    pixelOffset.x += (float) SCREENWIDTH / 2;
    pixelOffset.y += (float) SCREENHEIGHT / 2;
    return pixelOffset;
}

void renderMesh(SDL_Renderer *renderer, Obj3D * obj){
    fv2 mesh2D[obj->vCount];
    for (int i = 0; i < obj->vCount; i++) {
        // Vertext to Screen
        mesh2D[i] = vertexToScreen(obj->v[i], obj->pos, obj->yaw, obj->pitch);
    }
    for (int i = 0; i < obj->eCount; i+= 2) {
        fv2 lineStart = {mesh2D[obj->e[i]].x , mesh2D[obj->e[i]].y};
        fv2 lineEnd = {mesh2D[obj->e[i+1]].x , mesh2D[obj->e[i+1]].y};
        SDL_RenderLine(renderer, lineStart.x, lineStart.y, lineEnd.x, lineEnd.y);
    }

}

// Get rid of these later 
static Obj3D *testCube;

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
    testCube = createTestPyramid((fv3) {0, 0, 5});
    fv3 testV = scalefv3( (fv3){4, 1, 3}, 3);
    printf("test : x %f y %f z%f\n", testV.x, testV.y, testV.z);

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

    /* SDL_RenderPoint(renderer, 10, 10); */
    renderMesh(renderer, testCube);

    moveAway += 0.005;
    testCube->yaw += 0.01;
    testCube->pitch += 0.01;



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
    freeObj3D(testCube);
}
