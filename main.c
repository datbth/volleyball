/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "object.h"
#include "main.h"
#include <unistd.h>


//Screen dimension constants
SDL_Renderer *renderer;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int gravity = 1500;
const float TIME_PER_FRAME = 1000 / SCREEN_FPS;
int numObjects = 0;
int numPlayer = 0;
Player *players[4];
Object *objects[100];
const Uint8 *currentKeyStates;
int tickIndex = 0;
int tickSum = 0;
int tickList[MAXSAMPLE];

double calculateAverageTick(int newTick) {
    tickSum -= tickList[tickIndex];
    tickSum += SDL_GetTicks();
    tickList[tickIndex] = newTick;
    if (++tickSum == MAXSAMPLE) {
        tickIndex = 0;
    }

    return (double) tickSum / MAXSAMPLE;
}

SDL_Texture *loadTexture(char *path, SDL_Renderer *renderer) {
    SDL_Surface *loadedSurface = IMG_Load(path);    //  load PNG image
    SDL_Texture *texture = NULL;

    if (loadedSurface == NULL) {
        printf("can't load %s %s\n", path, IMG_GetError());
        return NULL;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (texture == NULL) {
            printf("can't create Texture %s %s\n", path, SDL_GetError());
            return NULL;
        } else {
            SDL_FreeSurface(loadedSurface);
            return texture;
        }
    }
}

SDL_Window *init() {
    SDL_Window *window;
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("can't initialize SDL%s\n", SDL_GetError());
        return false;
    } else {
        //Create window
        window = SDL_CreateWindow("Volleyball", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("can't initialize Window%s\n", SDL_GetError());
            return NULL;
        }
        return window;
    }
}

int handleKeys() {
    for (int i = 0; i < numPlayer; ++i) {
        Player *player = players[i];
        move(player,
             currentKeyStates[player->left],
             currentKeyStates[player->up],
             currentKeyStates[player->right]);
    }
    if (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_Q]) return 1;
    else if (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_R]) return 2;
    else return 0;
}

bool isElemInArray(int* arr, int elem, int length){
    for (int i = 0; i < length; i++){
        if (elem == arr[i]) return true;
    }
    return false;
}

void resetPosition(){
    for (int i = 0; i < numPlayer; ++i) {
        Player *currentPlayer = players[i];
        currentPlayer->object->X = currentPlayer->object->W*2*i;
        currentPlayer->object->Y = 0;
    }
}

int main(int argc, char *args[]) {
    //Start up SDL and create window
    SDL_Window *window = init();
    if (window == NULL) return 1;

    SDL_Event event;

    // setup controller
    currentKeyStates = SDL_GetKeyboardState(NULL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) return 2;
    SDL_Rect background = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    // create player
    SDL_Keycode keycode[] = {SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D,
                             SDL_SCANCODE_N, SDL_SCANCODE_B, SDL_SCANCODE_M,
                             SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_9,
                             SDL_SCANCODE_UP, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT};
    char imagePath[] = "../pics/ball0.png";
    for (int l = 0; l < 2; l++) {
        imagePath[12] += 1;
        Object* object = createObject(1, SCREEN_WIDTH/2, 0, 100, 100, imagePath);
        players[numPlayer] = createPlayer(object, 250, 675, keycode[l*3+0], keycode[l*3+1], keycode[l*3+2]);
        objects[numObjects++] = players[numPlayer++]->object;
    }

    // create wall
  	Object *object_w1 = createObject(3, SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 50, objects[0]->H * 2, "../pics/wall.png");
  	Wall *wall = createWall(object_w1);
    objects[numObjects++] = wall->object;


    // validate players and set their position
    int quit = 0;
    for (int j = 0; j < numPlayer; ++j){
        if (players[j] == NULL || players[j]->object == NULL) {
            quit = 1;
        } else {
            if (j == 3) {
                players[j]->object->X = (j-1)*players[j]->object->W*2;
                continue;
            }
            players[j]->object->X = j*players[j]->object->W*2;
        }
    }

    // get current time
    // if curtime - lasttime > interval (frametime)
    // -> run loop

    float lastTime = 0, currentTime;
    // game loop
    while (quit == 0){
        // get key input
        do {
            int signal = handleKeys();
            if (signal == 1) quit = 1;
            else if(signal == 2) resetPosition();
        }
        while (SDL_PollEvent(&event) != 0);

        // lock FPS
        currentTime = SDL_GetTicks();
        int actualTimePerFrame = (int) currentTime - (int) lastTime;
        if (actualTimePerFrame < TIME_PER_FRAME) {
            SDL_Delay((Uint32) ((int) TIME_PER_FRAME - actualTimePerFrame));
        }
        currentTime = SDL_GetTicks();

        //update logic position
        for (int i = 0; i < numObjects; ++i) updateXY(objects[i], currentTime);

        // Check collision and correct their positions
        int checkedObjIndexes[numObjects];
        int numCheckedObj = 0;
        for (int i = 0; i < numObjects; ++i) {
            Object *currentObj = objects[i];
            for (int j = 0; j < numObjects; j++) {
                if (i == j || isElemInArray(checkedObjIndexes, j + 1, numCheckedObj)) {
                    continue;
                }
                checkCollision(objects[i], objects[j]);
            }
        }
        //printf("|%i| X%i Y%i  ", currentPlayer->pos, currentPlayer->X, players[i]->Y);

        //update SDL position
        SDL_RenderClear(renderer);
        for (int k = 0; k < numObjects; ++k) {
            Object *currentObj = objects[k];
            SDL_Rect position = {(int) ceil(currentObj->X), (int) ceil(currentObj->Y), currentObj->W, currentObj->H};
            SDL_RenderCopyEx(renderer, currentObj->image, &background, &position, 0, 0, SDL_FLIP_NONE);
            checkedObjIndexes[numCheckedObj++] = k + 1;
        }
        SDL_RenderPresent(renderer);
        lastTime = currentTime;
    }


    //Free resources and close SDL ***
    printf("free player ");
    for (int i = 0; i < numPlayer; i++) {
        if(players[i])
        printf("%i ", i);
        freePlayer(players[i]);
    }
    printf("\n");

    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();

    return 0;
}
