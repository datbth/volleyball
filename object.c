//
// Created by hai on 4/19/17.
//

#include <SDL2/SDL_system.h>
#include <SDL_mixer.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "object.h"
#include "stdbool.h"
#include "math.h"
#include "main.h"
#include "objectList.h"
/**
 * player-player: block each other
 * player-wall: block each other
 * player-ball: ball bounces back
 * player-item: item destroyed
 * ball-ball: both bounces back
 * ball-item: item destroyed, effect applies
 * ball-wall: ball bounces back
 * item-wall: should not happen
 */

Object * obj_create(int id, int W, int H, char *imagePath){
    SDL_Texture *icon = loadTexture(imagePath);
    if (icon == NULL) return NULL;
//    printf("sizeof Object %lu int %lu float %lu void %lu \n", sizeof(Object), sizeof(int), sizeof(float), sizeof(void));
    Object *object = malloc(sizeof(Object));
    if (object == NULL) {
        SDL_DestroyTexture(icon);
        return NULL;
    }
    object->id = id;
    object->H = H;
    object->W = W;
    object->X = 0;
    object->Y = 0;
    object->veloX = 0;
    object->veloY = 0;
    object->accelY = 0;
    object->image = icon;
    SDL_Rect rect = {0, 0, W, H};
    object->positionRect = rect;
    object->positionRect.w = W;
    object->positionRect.h = H;
    object->isCollided = false;
    return object;
}


void obj_free(Object *object){
    if(object != NULL){
        if(object->wrapper != NULL) {
            switch (object->type){
                case OBJECT_PLAYER:
                    free((Player*)(object->wrapper));
                    break;
                case OBJECT_BALL:
                    free((Ball*)(object->wrapper));
                    break;
                case OBJECT_WALL:
                    free((Wall*)(object->wrapper));
                    break;
                case OBJECT_ITEM:
                    free((Item*)(object->wrapper));
                    break;
            }
        }
        SDL_DestroyTexture(object->image);
//        printf("free obj id%i\n", object->id);
        free(object);
    }
}

Player *player_create(Object *object, int speedX, int jumpHeight, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right){
    if (object == NULL) return NULL;
//    printf("sizeof Player %lu \n", sizeof(Player));
    Player *player = malloc(sizeof(Player));
    if (player == NULL){
        printf("player NULL");
        obj_free(object);
        return NULL;
    }
    player->object = object;
    player->object->wrapper = player;
    player->object->type = OBJECT_PLAYER;
    player->object->shapeType = SHAPE_CIRCLE;
    player->object->applyCollision = &player_applyCollision;

//    player->oldX = player->object->X;
//    player->oldY = player->object->Y;
//    printf("  sizeof Player keycode %lu bool %lu \n", sizeof(SDL_Keycode), sizeof(bool));
    player->up = up;
    player->left = left;
    player->right = right;
    player->speedX = speedX;
    player->jumpHeight = jumpHeight;
    player->isOnGround = false;
    return player;
}

Wall *wall_create(Object *object) {
    if (object == NULL) return NULL;

    Wall *wall = malloc(sizeof(Wall));
    if (wall == NULL){
        obj_free(object);
        return NULL;
    }

    wall->object = object;
    wall->object->type = OBJECT_WALL;
    wall->object->wrapper = wall;
    wall->object->shapeType = SHAPE_RECTANGLE;
    wall->object->applyCollision = &wall_applyCollision;
    return wall;
}

Ball *ball_create(Object *object){
    if (object == NULL) return NULL;
    Ball * ball = malloc(sizeof(Ball));
    if (ball == NULL) {
        obj_free(object);
        return NULL;
    }
    ball->object = object;
    ball->object->type = OBJECT_BALL;
    ball->object->wrapper = ball;
    ball->object->shapeType = SHAPE_CIRCLE;
    ball->object->applyCollision = &ball_applyCollision;
    return ball;
}

Item *item_create(Object *object, float ratio) {
    if (object == NULL) return NULL;
    Item *item = malloc(sizeof(Item));
    if (item == NULL){
        obj_free(object);
        return NULL;
    }

    item->object = object;
    item->object->type = OBJECT_ITEM;
    item->object->wrapper = item;
    item->object->shapeType = SHAPE_CIRCLE;
    item->object->applyCollision = &item_applyCollision;
    item->ratio = ratio;
    item->needRemove = false;
    return item;
}

Item *item_createRandomly(char *imagePath, int targetIndex, int itemNum){
    Item * newItem = NULL;
    int randomPos = rand() % itemNum;
    imagePath[targetIndex] += randomPos+1;
    Object * itemObj = obj_create(objects->size, 50, 50, imagePath);
    imagePath[targetIndex] -= randomPos+1;

    if(itemObj == NULL) return NULL;
    // initialize position
    itemObj->X = SCREEN_WIDTH/2 - itemObj->W/2;
    itemObj->Y =  SCREEN_HEIGHT/6 + (itemObj->H+20)  * (rand() % 4);
    itemObj->positionRect.x = (int) ceil(itemObj->X);
    itemObj->positionRect.y = (int) ceil(itemObj->Y);
    // random effect
    switch (randomPos){
        case 0:
            newItem = item_create(itemObj, 2);
            newItem->effectType = EFFECT_FASTER;
            break;
        case 1:
            newItem = item_create(itemObj, 2);
            newItem->effectType = EFFECT_SLOWER;
            break;
        case 2:
            newItem = item_create(itemObj, 3);
            newItem->effectType = EFFECT_REFLECT;
            break;
        default:break;
    }
    return newItem;
}

void item_removeFromList(Item *item, struct objectList *objects){
    if(item == NULL) return;
    al_remove(objects, item->object->id);
    obj_free(item->object);
}

void obj_setVeloX(Object *player, float veloX) {
    player->veloX = veloX;
}

void obj_update(Object *object, float moveTime, int gravity) {
    if(moveTime <= 0) return;
    moveTime /= 1000;

    // save old position
    if (!object->isCollided) {
        object->oldX = object->X;
        object->oldY = object->Y;
    }

    object->X += object->veloX * moveTime;  //update X

    float oldVeloY = object->veloY;         //update Y
    object->veloY += gravity * moveTime;
    object->Y += (object->veloY+oldVeloY)/2 * moveTime ;

    // touched the ground border
    if (object->Y >= SCREEN_HEIGHT - object->H) {
        object->Y = SCREEN_HEIGHT - object->H;
        if(object->type == OBJECT_PLAYER) {
            ((Player*) object->wrapper)->isOnGround = true ;
            object->veloY = 0;
        } else if(object->type == OBJECT_BALL) {
            reflectVectorAboutVector(&(object->veloX), &(object->veloY), 0, -object->H);
        }

    } else {
        if(object->type == OBJECT_PLAYER) {
            ((Player*) object->wrapper)->isOnGround = false ;
        }
    }
    // touched the left side of the screen
    if (object->X < 0) {
        object->X = 0;
        object->veloX = -object->veloX;
    }
    // touched the right side of the screen
    if (object->X > SCREEN_WIDTH - object->W) {
        object->X = SCREEN_WIDTH - object->W;
        object->veloX = -object->veloX;
    }

    if (object->type == OBJECT_PLAYER){
        object->isCollided = false;
    }
//    printf("|%i| X%f Y%f velX%f velY%f \n  ", object->id, object->X, object->Y, object->veloX, object->veloY);
}

void player_move(Player *player, int left, int up, int right) {
//    if(player->isCollided) return;
    float veloX = 0;

    if (!player->isOnGround) veloX = player->object->veloX;
    else if (left == 0 && right == 0) veloX = 0;

    if (player->isOnGround || player->object->veloX == 0){
        if (left == 1 && right == 0) veloX = -player->speedX;
        else if (right == 1 && left == 0) veloX = player->speedX;
    }
//    veloX = (right == 1) ? speedX : 0;

    if (up == 1 && player->isOnGround) {
//	  veloY -= 50;
        player->object->accelY = -100;
        player->object->veloY = -player->jumpHeight;
        //player->isOnGround = false;

        // Play sound when jump
        Mix_PlayChannel( -1, sounds[ 3+ rand()%3], 0 );
        // printf("%i\n", 3+ rand()%3);
    }

    obj_setVeloX(player->object, veloX);
//    setVeloY(player, accelY);

}

bool solveQuadEquation(float a, float b, float c, float *x1, float *x2){
//    printf("a: %f, b: %f, c: %f", a,b,c);
    float delta = b*b - 4*a*c;
    if (delta < 0) return false;
    else {
        float sqrtDelta = sqrtf(delta);
        *x1 = (-b - sqrtDelta)/(2*a);
        *x2 = (-b + sqrtDelta)/(2*a);
        return true;
    }
}

void obj_pushOut(Object *pushedObj, float stableX, float stableY, float targetDistance){
    if (pushedObj->veloY == 0 && pushedObj->veloX == 0) return;
    printf("AVelX %f velY%f\n", pushedObj->veloX, pushedObj->veloY);
    float currentToStableX = stableX - (pushedObj->X + pushedObj->W/2);
    float currentToStableY = stableY - (pushedObj->Y + pushedObj->H/2);
    float dotProduct = currentToStableX*(pushedObj->veloX) + currentToStableY*(pushedObj->veloY);
    float currentToStableDistSqr = currentToStableX*currentToStableX + currentToStableY*currentToStableY;
    float veloMagnitude = sqrtf(pushedObj->veloY*pushedObj->veloY + pushedObj->veloX*pushedObj->veloX);
//     pushedDistance^2 - 2 *(dotProduct/veloMagnitude)* pushedDistance
//          - targetDistance^2 + currentToStableDist^2 = 0;
    float x1, x2;
    if (veloMagnitude == 0) return;
    if (solveQuadEquation(1, -2*dotProduct/veloMagnitude, -targetDistance * targetDistance+currentToStableDistSqr, &x1, &x2)){
        float pushedDistance = 0;
        if (x1 >= 0){
            if (x1 <= x2) pushedDistance = x1;
        } else if (x2 >= 0) pushedDistance = x2;

        pushedDistance += 1;
        pushedObj->X += pushedObj->veloX * pushedDistance / veloMagnitude;
        pushedObj->Y += pushedObj->veloY * pushedDistance / veloMagnitude;
        printf("pushedDistance: %f", pushedDistance);
    }
}

void obj_backToUncollidedPosition(Object *object){
    if (object->type != OBJECT_WALL) {
        object->X = object->oldX;
        object->Y = object->oldY;
    }
}

void player_applyCollision(Object *playerObj, Object *target, float *collisionX, float *collisionY){
    playerObj->isCollided = true;
    switch (target->type) {
        case OBJECT_WALL:
            obj_backToUncollidedPosition(playerObj);
            //        printf("ColX %f colY %f\n", *collisionX, *collisionY);
            float normalX = playerObj->X + playerObj->W / 2 - *collisionX,
                    normalY = playerObj->Y + playerObj->H / 2 - *collisionY;
            reflectVectorAboutVector(&(playerObj->veloX), &(playerObj->veloY), -normalX, -normalY);

            //when the ball is on top of the wall
            //        if (*collisionY > target->Y) playerObj->veloY = 0;
            //        else {
            playerObj->veloX /= 3;
            ////            playerObj->veloY /= 10;
            //        }
            //        playerObj->veloX = 0;
            break;
        case OBJECT_PLAYER:
            obj_backToUncollidedPosition(playerObj);
            if (obj_isMovingCloser(playerObj, target)) {
                reflectVectorAboutVector(&(playerObj->veloX), &(playerObj->veloY), target->X - playerObj->X,
                                         target->Y - playerObj->Y);
                //        playerObj->veloX = 0; playerObj->veloY = 0;
                playerObj->veloX /= 10;
                playerObj->veloY /= 10;

                float targetCenterToCollisionY = *collisionY - (target->Y + target->H / 2);
                if (targetCenterToCollisionY < 0) {
                    ((Player *) (playerObj->wrapper))->isOnGround = true;
                    playerObj->isCollided = false;
                }
            }
            break;
        case OBJECT_BALL:
            obj_backToUncollidedPosition(playerObj);
            break;
        case OBJECT_ITEM:
            break;
    }
}

void wall_applyCollision(Object *wallObject, Object *target, float *collisionX, float *collisionY) {

}

void ball_applyCollision(Object *ballObj, Object *target, float *collisionX, float *collisionY){
    // Play sound when ball collide
    if(target->type != OBJECT_PLAYER) Mix_PlayChannel( -1, sounds[ rand()%3], 0 );

//    ((Ball*)ballObj->wrapper)->isCollided = true;
    switch (target->type){
        case OBJECT_WALL:
        {
            //        printf("ColX %f colY %f\n", *collisionX, *collisionY);
            float normalX = ballObj->X + ballObj->W/2 - *collisionX,
                    normalY = ballObj->Y + ballObj->H/2 - *collisionY;
            reflectVectorAboutVector(&(ballObj->veloX), &(ballObj->veloY), -normalX, -normalY);
            obj_backToUncollidedPosition(ballObj);
            break;
        }
        case OBJECT_PLAYER:
            // Play sound when ball collide
            if (ballObj->veloY >= 200) Mix_PlayChannel( -1, sounds[ rand()%3], 0 );
//            obj_backToUncollidedPosition(ballObj);
//            printf("velo ball: %f\n", ballObj->veloY);
            if (obj_isMovingCloser(ballObj, target)) {
                reflectVectorAboutVector(&(ballObj->veloX), &(ballObj->veloY),
                                         ballObj->X + ballObj->W/2 - *collisionX ,
                                         ballObj->Y + ballObj->H/2 - *collisionY);

                ballObj->veloX *= 0.7;
                ballObj->veloY *= 0.7;
                obj_backToUncollidedPosition(ballObj);
            }
            if(ballObj->veloY <= 200) {
                ballObj->veloY += target->veloY;
                ballObj->veloX += target->veloX;
//                printf("*velo ball: %f\n", ballObj->veloY);
            }
            break;
        case OBJECT_BALL:break;
        case OBJECT_ITEM:break;
    }
}

void item_applyCollision(Object *itemObj, Object *target, float *collisionX, float *collisionY){
    Item *currentItem = (Item*)(itemObj->wrapper);
    switch(target->type){
        case OBJECT_PLAYER:
            currentItem->needRemove = true;
            break;
        case OBJECT_BALL:
            Mix_PlayChannel( -1, sounds[7], 0 );
            switch (currentItem->effectType){
                case EFFECT_FASTER:
                    target->veloY *= currentItem->ratio;
                    target->veloX *= currentItem->ratio;
//                    printf("faster\n");
                    break;
                case EFFECT_SLOWER:
                    target->veloY /= currentItem->ratio;
                    target->veloX /= currentItem->ratio;
//                    printf("slower\n");
                    break;
                case EFFECT_REFLECT:
                    target->veloY *= -1;
                    target->veloX *= -1;
//                    printf("reflect\n");
                    break;
            }
            currentItem->needRemove = true;
            break;
        case OBJECT_ITEM:
            if(itemObj->id < target->id) currentItem->needRemove = true;
            break;
        case OBJECT_WALL:break;
    }
}


float distSquared(float x1, float x2, float y1, float y2) {
    float deltaX = x2 - x1;
    float deltaY = y2 - y1;
    return deltaY * deltaY + deltaX * deltaX;
}

bool obj_areCirclesCollided(Object *A, Object *B, float *collisionX, float *collisionY) {
    if(A->type == OBJECT_WALL) return false;

    int rA = A->W / 2, rB = B->W / 2;
    float ACenterX = A->X + A->W/2, ACenterY = A->Y + A->H/2;
    float BCenterX = B->X + B->W/2, BCenterY = B->Y + B->H/2;
    float distX = BCenterX - ACenterX;
    float distY = BCenterY - ACenterY;
    float centerDist = sqrtf(distX * distX + distY * distY);
    if (centerDist < rA + rB) {
        *collisionX = ACenterX + distX / centerDist * rA;
        *collisionY = ACenterY + distY / centerDist * rA;
        return true;
    } else return false;
}

bool obj_areCircleRectCollided(Object *circle, Object *rectangle, float *collisionX, float *collisionY){
    if (rectangle->type != OBJECT_WALL) return false;

    float closestBoxX, closestBoxY = 0;
    /**
     * Find the closest point's X from A's center to B
     * If A is to left of B then the closest point is always on the left edge of B and vice versa
     */

    float radiusA = circle->W / 2;
    float center_A_X = circle->X + radiusA;
    float center_A_Y = circle->Y + radiusA;

    closestBoxX = fmaxf(rectangle->X, fminf(center_A_X, rectangle->X + rectangle->W));
    closestBoxY = fmaxf(rectangle->Y, fminf(center_A_Y, rectangle->Y + rectangle->H));

    if (distSquared(center_A_X, closestBoxX, center_A_Y, closestBoxY) < radiusA * radiusA){
        *collisionX = closestBoxX;
        *collisionY = closestBoxY;
//        printf("collided \n");
        return true;
    }
    return false;
}

int reflectVectorAboutVector(float *vectorX, float *vectorY, float normalX, float normalY){
    //https://en.wikipedia.org/wiki/Reflection_%28mathematics%29#Reflection_across_a_line_in_the_plane
    //https://en.wikipedia.org/wiki/Dot_product
    //https://www.varsitytutors.com/hotmath/hotmath_help/topics/adding-and-subtracting-vectors
    //https://www.evl.uic.edu/dmitr/classes/cs527/proj2/
//    coefficient = 2 * dotProductVL / dotProductLL
    float coefficient = 2 *(*vectorX * normalX + *vectorY * normalY)
                        /(normalX * normalX + normalY * normalY);
    *vectorX = -(coefficient * normalX - *vectorX);
    *vectorY = -(coefficient * normalY - *vectorY);
    return 0;
}

void obj_checkCollision(Object *source, Object *target) {
    bool collided = false;

    float collideX = 0, collideY = 0;

    if (target->shapeType == SHAPE_CIRCLE) {
        collided = obj_areCirclesCollided(source, target, &collideX, &collideY);
    } else if (target->shapeType == SHAPE_RECTANGLE) {
        collided = obj_areCircleRectCollided(source, target, &collideX, &collideY);
    }

    if (collided){
//        if(source->type == OBJECT_PLAYER) printf("---   X %f    Y %f \n---oldX %f oldY %f\n", source->X, source->Y, source->oldX, source->oldY);
        source->applyCollision(source, target, &collideX, &collideY);
        target->applyCollision(target, source, &collideX, &collideY);
    }
}

bool obj_isMovingCloser(Object *source, Object *target){
    /**
     * https://math.stackexchange.com/questions/1438002/determine-if-objects-are-moving-towards-each-other
     * The velocity of object 2 relative to object 1 is given by v = v2???v1.
     * The displacement of object 2 from object 1 is given by d =p2???p1.
     * We may simply take the dot product v???d
     * If the result is positive, then the objects are moving away from each other.
     * If the result is negative, then the objects are moving towards each other.
     * If the result is 00, then the distance is (at that instance) not changing.
     */
    float vX = target->veloX - source->veloX;
    float vY = target->veloY - source->veloY;
    float dX = target->X - source->X;
    float dY = target->Y - source->Y;
    float dotProduct = vX*dX + vY*dY;
    return dotProduct < 0;
}
