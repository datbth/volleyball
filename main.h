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
 * check which key combination is pressed and pass the key state to every Player's move() function
 * @return 0 - none
 *          1 - Ctrl Q
 *          2 - Ctrl R
 *          3 - Ctrl P
 *          4 - Ctrl U
 *          5 - Return
 */
int handleKeys();

/**
 * check if an integer exists in an array
 * @param arr : pointer to integer array
 * @param elem : integer element
 * @param length : length of array
 * @return true if element is in array/ false if not
 */
bool isElemInArray(int* arr, int elem, int length);

/**
 * Find the index of the first integer in a char array
 * @param array pointer to array of character
 * @return -1 if no integer found / otherwise return the index of the first integer
 */
int getIntFromArray(char * array);

/**
 * reset all players and the wall to the initial positions and remove velocity. reset the ball to top of the screen
 * (the wall is also resetted because the team has planned to allow users to move the wall, as a hidden feature, in the future)
 * if teamHasBall == 0, ball will be dropped on a random side
 * @param teamHasBall integer to determine which team has ball
 */
void resetPositions(int teamHasBall);

/**
 * generate a char array that displays the score of both teams
 * @param string pointer to string
 * @param teamPoint1st team 1's point
 * @param teamPoint2nd team 2's point
 */
void getScoreString(char *string, int teamPoint1st, int teamPoint2nd);

/**
 * check the position of the ball to determine whether a team has scored a point
 * @param ballId id of the ball object
 * @param teamPoint1st team 1's point
 * @param teamPoint2nd team 2's point
 * @param winningTeam the number of the team who has won the game
 * @return 0 if neither team has scored, otherwise return the number of the team that loses the point and is going to receive the ball in the next round
 */
int updateScore(int ballId, int *teamPoint1st, int *teamPoint2nd, int *winningTeam);

/**
 * show the game instructions
 * @param color color of the instruction text
 * @param fontX X cordinate of the instruction
 * @param fontY Y cordinate of the instruction
 * @param background pointer to background
 */
void showInstruction(SDL_Color color, int fontX, int fontY, SDL_Rect *background);

/**
 * Pause/unpause the game
 */
void togglePause();

/**
 * remove items
 * @param checkNeedRemove if true, the function will check and only remove the collided items; otherwise remove all items
 */
void cleanItems(bool checkNeedRemove);

#endif //GAME_GLOBALVARIABLES_H
