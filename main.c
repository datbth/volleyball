/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "player.h"
#include "main.h"

//Screen dimension constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
SDL_Renderer *renderer;
int gravity = 5;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000/60;
const Uint8* currentKeyStates;


SDL_Texture *loadTexture(char *path, SDL_Renderer* renderer) {
    SDL_Surface *loadedSurface = IMG_Load(path);    //  load PNG image
    SDL_Texture * texture = NULL;

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


int handleKeys(Player *players[], int playerNum){
    for (int i = 0; i < 4; ++i) {
        Player* player = players[i];
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
    currentKeyStates = SDL_GetKeyboardState( NULL );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) return 2;
    SDL_Rect fillRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    // create player
    Player * player1 = createPlayer(1, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, "pics/hacker_icon.png");
    Player * player2 = createPlayer(2, SDL_SCANCODE_N, SDL_SCANCODE_B, SDL_SCANCODE_M, "pics/hacker_icon.png");
    Player * player3 = createPlayer(3, SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_9, "pics/hacker_icon.png");
    Player * player4 = createPlayer(4, SDL_SCANCODE_UP, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, "pics/hacker_icon.png");
    Player *players[4] = {player1, player2, player3, player4};

    // validate players
    int quit = 0;
    for (int j = 0; j < 4; ++j) if (players[j] == NULL) {
            printf("%i is NULL", players[j]->pos);
            quit = 1;
    }

    while (quit == 0) {
        while (SDL_PollEvent(&event) != 0) {
            if (handleKeys(players, 4) == 1) quit = 1;
        }
        // update position
        SDL_RenderClear(renderer);
        for (int i = 0; i < 4; ++i) {
            Player * currentPlayer = players[i];
            updateXY(players[i]);
            SDL_Rect position = {currentPlayer->X, currentPlayer->Y, currentPlayer->size, currentPlayer->size};

            SDL_RenderCopyEx(renderer, currentPlayer->icon, &fillRect, &position, 0, 0, 0);
            SDL_RenderPresent(renderer);

//            printf("|%i| X%i Y%i  ", currentPlayer->pos, currentPlayer->X, players[i]->Y);
        }
//        printf("\n");
        SDL_Delay(25);
    }


    //Free resources and close SDL ***
    for (int i = 0; i < 4; i++ ) free(players[i]);
    SDL_DestroyRenderer(renderer); renderer = NULL;
    SDL_DestroyWindow(window);window = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();

    return 0;
}
