//
// Created by hai on 4/19/17.
//

#ifndef GAME_GLOBALVARIABLES_H
#define GAME_GLOBALVARIABLES_H
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
 * @return 0
 */
int handleKeys();

/**
 * find if an element in an array of integer
 * @param arr : pointer to integer array
 * @param elem : integer element
 * @param length : length of array in integer
 * @return true if element is in array/ false if not
 */
bool isElemInArray(int* arr, int elem, int length);

/**
 *
 * @param array pointer to array of character
 * @return -1 if there is integer in array / 0 if nothing in array
 */
int getIntFromArray(char * array);

/**
 * reset positions
 * @param teamHasBall integer to determine which team has ball
 */
void resetPositions(int teamHasBall);

/**
 *
 * @param string pointer to string
 * @param teamPoint1st integer of point team 1
 * @param teamPoint2nd integer of point team 2
 */
void getScoreString(char *string, int teamPoint1st, int teamPoint2nd);

/**
 *
 * @param ballId integer, id of a ball object
 * @param teamPoint1st pointer to integer point team 1
 * @param teamPoint2nd pointer to integer point team 2
 * @param winningTeam pointer to integer win 
 */
int updateScore(int ballId, int *teamPoint1st, int *teamPoint2nd, int *winningTeam);

/**
 *
 * @param color Color of SDL
 * @param fontX integer of font X
 * @param fontY interger of font Y
 * @param background pointer to background
 */
void showInstruction(SDL_Color color, int fontX, int fontY, SDL_Rect *background);

/**
 * Pause the game
 */
void togglePause();

/**
 * remove items
 * @param checkNeedRemove boolean
 */
void cleanItems(bool checkNeedRemove);

#endif //GAME_GLOBALVARIABLES_H
