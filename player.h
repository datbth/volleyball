//
// Created by hai on 4/19/17.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <SDL2/SDL_scancode.h>
#include <stdbool.h>

typedef struct {
    int pos, speed, minY, maxY,size, accelY;
	float veloX, veloY, X, Y;
    SDL_Keycode up, left, right;
    SDL_Texture * icon;
	float lastMoveTime;
	bool onGround;

} Player;

Player *createPlayer(int pos, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right, char* file_path);


void setVeloX(Player *player, float veloX);
void setVeloY(Player *player, float veloY);
void updateXY(Player * player, float newFrameTime);
void move(Player * player, int left, int up, int right);

#endif //GAME_PLAYER_H
