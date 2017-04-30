//
// Created by hai on 4/19/17.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <SDL2/SDL_scancode.h>
#include <stdbool.h>

enum objectType {
    OBJECT_PLAYER,
    OBJECT_BALL,
    OBJECT_ITEM,
    OBJECT_WALL,
};
//typedef struct Player Player;

typedef struct {
    int id, speed, accelY, W, H;
    float veloX, veloY, X, Y, lastMoveTime;
    void * wrapper;
    SDL_Texture *image;
    enum objectType type;
} Object;

typedef struct {
    Object *object;
    SDL_Keycode up, left, right;
    bool onGround;
} Player;

typedef struct {
    Object * object;

} Item;
Object *createObject(int id, float X, float Y,int W, int H, char* imagePath);
Player *createPlayer(Object* object, int speed, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right);

void setVeloX(Object *player, float veloX);

void setVeloY(Object *player, float veloY);

void updateXY(Object *object, float newFrameTime);

void move(Player *player, int left, int up, int right);
bool setPlayerOnGround(Player *player);
bool isCollided(Object *A, Object *B);
void getCollideXY(float *X, float *Y, Object* A, Object *B);
void checkCollision(Object * object, Player* players[], Object * collision);

void freeObject(Object * object);
void freePlayer(Player * player);

#endif //GAME_PLAYER_H
