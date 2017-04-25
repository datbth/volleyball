//
// Created by hai on 4/19/17.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <SDL2/SDL_scancode.h>
#include <stdbool.h>

typedef struct {
    int pos, speed, minY, maxY, veloX, veloY, X, Y, size;
    SDL_Keycode up, left, right;
    SDL_Texture * icon;
	bool onGround;

} Player;

Player *createPlayer(int pos, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right, char* file_path);


void setVeloX(Player *player, int veloX);
void setVeloY(Player *player, int veloY);
void updateXY(Player * player);
void move(Player * player, int left, int up, int right);

#endif //GAME_PLAYER_H
