//
// Created by hai on 4/19/17.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <SDL2/SDL_scancode.h>

typedef struct {
    int pos, speed, minY, maxY, veloX, veloY, X, Y, size, jumping;
    SDL_Keycode up, left, right;
    SDL_Texture * icon;

} Player;

Player *createPlayer(int pos, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right, char* file_path);


void setVeloX(Player *player, int veloX);
void setVeloY(Player *player);
void updateXY(Player * player, int gravity);


#endif //GAME_PLAYER_H
