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
const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;
const int gravity = 100;
const float TIME_PER_FRAME = 1000 / SCREEN_FPS;
Player *players[numPlayer];
const Uint8 *currentKeyStates;
int tickIndex = 0;
int tickSum = 0;
int tickList[MAXSAMPLE];

double calculateAverageTick(int newTick) {
    tickSum -= tickList[tickIndex];
    tickSum += newTick;
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

SDL_Window *init() {   //create window
    SDL_Window *window;
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("can't initialize SDL%s\n", SDL_GetError());
        return false;
    } else {
        //Create window
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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
    return (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_Q]) ? 1 : 0;
}


int main(int argc, char *args[]) {
    //The window we'll be rendering to
    renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_Event event;

    //Start up SDL and create window
    SDL_Window *window = init();
    if (window == NULL) return 1;

    // setup controller
    currentKeyStates = SDL_GetKeyboardState(NULL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) return 2;
    SDL_Rect fillRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    // create player
    Object* object_p1 = createObject(1, SCREEN_WIDTH/2, 0, 100, 100, "../pics/ball1.png");
    Object* object_p2 = createObject(2, SCREEN_WIDTH/2, 0, 100, 100, "../pics/ball2.png");
    Object* object_p3 = createObject(3, SCREEN_WIDTH/2, 0, 100, 100, "../pics/ball3.png");
    Object* object_p4 = createObject(4, SCREEN_WIDTH/2, 0, 100, 100, "../pics/ball4.png");
    players[0] = createPlayer(object_p1, 500, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D);
    players[1] = createPlayer(object_p2, 500, SDL_SCANCODE_N, SDL_SCANCODE_B, SDL_SCANCODE_M);
    players[2] = createPlayer(object_p3, 500, SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_9);
    players[3] = createPlayer(object_p4, 500, SDL_SCANCODE_UP, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT);

    Object* collisionIndicator = createObject(4, SCREEN_WIDTH/2, 0, 20, 20, "../pics/hacker_icon.png");
//    Player* players[] = {p1, p2, p3, p4};

    // validate players
    int quit = 0;
    for (int j = 0; j < numPlayer; ++j){
        if (players[j] == NULL || players[j]->object == NULL) {
            printf("%i is NULL\n", players[j]->object->id);
            quit = 1;
        } else {
            players[j]->object->X = j*players[j]->object->W*2;
        }
    }

    // get current time
    // if curtime - lasttime > interval (frametime)
    // -> run loop

    float lastTime = 0, currentTime;

    int counter = 0;
    // game loop
    while (quit == 0) {

        // wait until get key input
//        do if (handleKeys() == 1) quit = 1;
        while (SDL_PollEvent(&event) != 0)if (handleKeys() == 1) quit = 1;

        currentTime = SDL_GetTicks();
        int actualTimePerFrame = (int)currentTime - (int)lastTime;

        if (actualTimePerFrame < TIME_PER_FRAME) {
//            printf("%f %f %d %i \n", lastTime, currentTime, (int)TIME_PER_FRAME,
//                   (Uint32) ((int)TIME_PER_FRAME - actualTimePerFrame) );
            SDL_Delay((Uint32) ((int)TIME_PER_FRAME - actualTimePerFrame));
//            continue;
        }

        currentTime = SDL_GetTicks();

        SDL_RenderClear(renderer);
        // update position
        for (int i = 0; i < numPlayer; ++i) {
            Player *currentPlayer = players[i];
            updateXY(players[i]->object, currentTime);
            checkCollision(players[i]->object, players, collisionIndicator);
            SDL_Rect position = {(int) ceil(currentPlayer->object->X), (int) ceil(currentPlayer->object->Y), currentPlayer->object->W, currentPlayer->object->H};
            SDL_RenderCopyEx(renderer, currentPlayer->object->image, &fillRect, &position, 0, 0, SDL_FLIP_NONE);

//            printf("|%i| X%i Y%i  ", currentPlayer->pos, currentPlayer->X, players[i]->Y);
        }
        // collision effect
        SDL_Rect position = {(int) ceil(collisionIndicator->X)-collisionIndicator->W/2,
                             (int) ceil(collisionIndicator->Y)-collisionIndicator->H/2,
                             collisionIndicator->W,
                             collisionIndicator->H};
        SDL_RenderCopyEx(renderer, collisionIndicator->image, &fillRect, &position, 0, 0, SDL_FLIP_NONE);

        SDL_RenderPresent(renderer);
        lastTime = currentTime;

    }


    //Free resources and close SDL ***
    for (int i = 0; i < numPlayer; i++) {
        printf("free player %i\n", i);
        freePlayer(players[i]);
    }
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();

    return 0;
}
