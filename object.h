//
// Created by hai on 4/19/17.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <SDL2/SDL_scancode.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL.h>
#include <stdbool.h>
#include "objectList.h"

struct objectList* objects;

enum ObjectType {
    OBJECT_PLAYER,
    OBJECT_BALL,
    OBJECT_WALL,
    OBJECT_ITEM,
};

enum ShapeType {
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE
};

enum EffectType {
    EFFECT_FASTER,
    EFFECT_SLOWER,
    EFFECT_REFLECT
};

typedef struct Object Object;

struct Object {
    int id, accelY, W, H;
    float veloX, veloY, X, Y;
    float oldX, oldY;
    void * wrapper;
    SDL_Texture *image;
    SDL_Rect positionRect;
    enum ObjectType type;
    enum ShapeType shapeType;
    void (*applyCollision)(Object *A, Object *B, float *collisionX, float *collisionY);
    bool isCollided;
};

typedef struct {
    Object *object;
    SDL_Keycode up, left, right;
    int speedX, jumpHeight;
    bool isOnGround, isTeamLeft;
} Player;

typedef struct {
    Object * object;
} Ball;

typedef struct {
    Object * object;
} Wall;

typedef struct {
    Object * object;
    enum EffectType effectType;
    float ratio;
    bool needRemove;
} Item;

/**
 * create Object
 * @param id : integer id of the new object
 * @param W : integer Width of the new object
 * @param H : integer of Height of the new object
 * @param imagePath : string path to image of player
 * @return object
 */
Object *obj_create(int id, int W, int H, char *imagePath);

/**
 * create a struct Player 
 * @param object : pointer to Object
 * @param speedX : integer of speed
 * @param jumpHeight: height player can jump
 * @param up : key input for moving UP
 * @param left : key input for moving LEFT
 * @param right : key input for moving RIGHT
 * @return player
 */
Player *player_create(Object *object, int speedX, int jumpHeight, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right);
/**
 * create a struct Ball
 * @param object pointer to already created object
 * @return ball if create successful / NULL if not
 */
Ball *ball_create(Object *object);

/**
 * create a struct Wall
 * @param object pointer to Object
 * @return wall  if create successful / NULL if not
 */
Wall *wall_create(Object *object);

/**
 * create a struct Item
 * @param object pointer to Object
 * @param ratio float ratio of object
 * @return item if create successful / NULL if not
 */
Item *item_create(Object *object, float ratio);

/**
 * Create random a random item
 * @param imagePath pointer to imagePath
 * @param targetIndex index of the character that need to be replaced
 * @param itemNum number of items
 * @return newItem  if create successful / NULL if not
 */
Item *item_createRandomly(char *imagePath, int targetIndex, int itemNum);

/**
 * free the object along with its wrapper and image
 * @param object pointer to object need to be freed
 */
void obj_free(Object *object);

/**
 *  Remove item from the object list
 * @param item Choose item to remove
 * @param objects pointer to object need to be removed from object list
 */
void item_removeFromList(Item *item, struct objectList *objects);

/**
 * set velocity X for player
 * @param player : pointer to Player
 * @param veloX : float velocity X
 */
void obj_setVeloX(Object *player, float veloX);\

/**
 * Update X and Y of an object
 * @param object : pointer to Object
 * @param newFrameTime : float of new frame time
 */
void obj_update(Object *object, float newFrameTime, int gravity);

/**
 * move the player according to when the corresponding key if pressed
 * @param player : pointer to Player
 * @param left : integer of Left key
 * @param up : integer of Up key
 * @param right : integer of Right key
 */
void player_move(Player *player, int left, int up, int right);

/**
 * check if the two circle objects are collided
 * @param A : pointer to object A
 * @param B : pointer to object B
 * @param X : pointer to collision pointX
 * @param Y : pointer to collision pointY
 */
bool obj_areCirclesCollided(Object *A, Object *B, float *collisionX, float *collisionY);

/**
 * check if a circle obj and a rectangle obj are collided or not
 * @param circle Pointer to object circle
 * @param rectangle Pointer to object rectangle
 * @param collisionX Pointer to float collision X
 * @param collisionY Pointer to float collision Y
 * @return true if collided / false if not
 */
bool obj_areCircleRectCollided(Object *circle, Object *rectangle, float *collisionX, float *collisionY);

/**
 * check for collision between 2 objects and apply effect
 * @param source : pointer to object Source
 * @param target : pointer to object Target
 */
void obj_checkCollision(Object *source, Object *target);

/**
 * check if the source object is moving toward the target or not
 * @param source : pointer to source object
 * @param target : pointer to target object
 * @return true if 2 objects move closer
 */
bool obj_isMovingCloser(Object *source, Object *target);


/**
 * find the distance square between the two objects using Pythagorean
 * @param source : pointer to Object source
 * @param target : pointer to Object target
 * @return float hypotenuse^2
 */
float distSquare(Object * source, Object * target);

/**
 * Apply effect on Wall depends on colliding with different object
 * @param source pointer to source object
 * @param target the object that collides with ball
 * @param collisionX pointer to float collision X
 * @param collisionY pointer to float collision Y
 */
void wall_applyCollision(Object *source, Object *target, float *collisionX, float *collisionY);

/**
 * Apply effect on Player depends on colliding with different object
 * @param source pointer to source object
 * @param target the object that collides with ball
 * @param collisionX pointer to float collision X
 * @param collisionY pointer to float collision Y
 */
void player_applyCollision(Object *source, Object *target, float *collisionX, float *collisionY);

/**
 * Apply effect on Item depends on colliding with different object
 * @param itemObj pointer to ball object
 * @param target  the object that collides with ball
 * @param collisionX pointer to float collision X
 * @param collisionY pointer to float collision Y
 */
void item_applyCollision(Object *itemObj, Object *target, float *collisionX, float *collisionY);

/**
 * Apply effect on Ball depends on colliding with different object
 * @param ballObj pointer to ball object
 * @param target  the object that collides with ball
 * @param collisionX pointer to float collision X
 * @param collisionY pointer to float collision Y
 */
void ball_applyCollision(Object *ballObj, Object *target, float *collisionX, float *collisionY);

/**
 * push the current object backward the collision point 
 * @param playerObj
 * @param collisionX
 * @param collisionY
 * @param targetDistance
 */
void obj_pushOut(Object *playerObj, float collisionX, float collisionY, float targetDistance);

/**
 * reflect ball vector base on the normal vector when collide
 * @param vectorX pointer to float vector X
 * @param vectorY pointer to float vector Y
 * @param normalX float X
 * @param normalY float Y
 * @return 0
 */
int reflectVectorAboutVector(float *vectorX, float *vectorY, float normalX, float normalY);

//float centerDistSquared(Object *source, Object *target);

#endif //GAME_PLAYER_H
