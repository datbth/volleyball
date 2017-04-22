//
// Created by hai on 4/19/17.
//

#ifndef GAME_GLOBALVARIABLES_H
#define GAME_GLOBALVARIABLES_H

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int gravity;
extern SDL_Renderer *renderer;

SDL_Texture *loadTexture(char *path, SDL_Renderer* renderer);

#endif //GAME_GLOBALVARIABLES_H
