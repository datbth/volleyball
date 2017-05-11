//
// Created by hai on 4/19/17.
//

#ifndef GAME_GLOBALVARIABLES_H
#define GAME_GLOBALVARIABLES_H
#define MAXSAMPLE 100
#define SCREEN_FPS 60



extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern Mix_Chunk *sounds[10];

SDL_Texture *loadTexture(char *path);

/**
 * create window
 * @return window
 */
SDL_Window *init();

/**
 * handle keys when they are pressed
 * move players
 * quit when Ctrl + Q are pressed
 * @return
 */
int handleKeys();

/**
 * find if an element in an array of integer
 * @param arr : pointer to integer array
 * @param elem : integer element
 * @param length : length of array in integer
 * @return bool
 */
bool isElemInArray(int* arr, int elem, int length);



#endif //GAME_GLOBALVARIABLES_H
