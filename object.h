//
// Created by hai on 4/19/17.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <SDL2/SDL_scancode.h>
#include <stdbool.h>

enum ObjectType {
    OBJECT_PLAYER,
    OBJECT_BALL,
    OBJECT_ITEM,
    OBJECT_WALL,
};

enum ShapeType {
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE
};

typedef struct Object Object;

struct Object {
    int id, speed, accelY, W, H;
    float veloX, veloY, X, Y, lastMoveTime;
    void * wrapper;
    SDL_Texture *image;
    enum ObjectType type;
    enum ShapeType shapeType;
    void (*applyCollision)(Object *A, Object *B);
};

typedef struct {
    Object *object;
    SDL_Keycode up, left, right;
    float oldX, oldY;
    bool onGround, isCollided;
} Player;

typedef struct {
    Object * object;
} Ball;

Object *createObject(int id, float X, float Y,int W, int H, char* imagePath);
Player *createPlayer(Object* object, int speed, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right);
Ball *createBall(Object * object);
void freeObject(Object * object);
void freePlayer(Player * player);
void freeBall(Ball * ball);

void setVeloX(Object *player, float veloX);

void setVeloY(Object *player, float veloY);

void updateXY(Object *object, float newFrameTime);

void move(Player *player, int left, int up, int right);
bool setPlayerOnGround(Player *player);
bool isCircleCollided(Object *A, Object *B);
void getCollideXY(float *X, float *Y, Object* A, Object *B);
void checkCollision(Object * source, Object * target);
bool isMovingCloser(Object * source, Object * target);
void applyBallCollision(Object *source, Object *target);
void applyPlayerCollision(Object *source, Object *target);
int reflectVectorAboutVector(float *vectorX, float *vectorY, float normalX, float normalY);
float distSquare(Object * source, Object * target);

#endif //GAME_PLAYER_H
