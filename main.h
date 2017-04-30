//
// Created by hai on 4/19/17.
//

#ifndef GAME_GLOBALVARIABLES_H
#define GAME_GLOBALVARIABLES_H
#define MAXSAMPLE 100
#define SCREEN_FPS 60
#define numPlayer 4

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int gravity;
extern SDL_Renderer *renderer;
//extern Player * players[numPlayer];

SDL_Texture *loadTexture(char *path, SDL_Renderer* renderer);

#endif //GAME_GLOBALVARIABLES_H
