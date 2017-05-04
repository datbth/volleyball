//
// Created by hai on 4/19/17.
//

#include <SDL2/SDL_system.h>
#include <time.h>
#include "object.h"
#include "stdbool.h"
#include "math.h"
#include "main.h"

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

Object *createObject(int id, float X, float Y, int W, int H, char* imagePath){
    SDL_Texture *icon = loadTexture(imagePath, renderer);
    if (icon == NULL) return NULL;
//    printf("sizeof Object %lu int %lu float %lu void %lu \n", sizeof(Object), sizeof(int), sizeof(float), sizeof(void));
    Object *object = malloc(sizeof(Object));
    if (object == NULL) {
        SDL_DestroyTexture(icon);
        return NULL;
    }
    object->speed = 0;
    object->id = id;
    object->image = icon;
    object->H = H;
    object->W = W;
    object->X = X;
    object->Y = Y;
    object->veloX = 0;
    object->veloY = 0;
    object->accelY = 0;
    object->lastMoveTime = 0;

    return object;

}

void freeObject(Object * object){
    SDL_DestroyTexture(object->image);
    free(object);
}

Player *createPlayer(Object* object,int speed, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right){
    if (object == NULL) return NULL;
//    printf("sizeof Player %lu \n", sizeof(Player));
    Player *player = malloc(sizeof(Player));
    if (player == NULL){
        printf("player NULL");
        freeObject(object);
        return NULL;
    }
    player->object = object;
    player->object->wrapper = player;
    player->object->type = OBJECT_PLAYER;
    player->object->shapeType = SHAPE_CIRCLE;

    player->oldX = player->object->X;
    player->oldY = player->object->Y;
//    printf("  sizeof Player keycode %lu bool %lu \n", sizeof(SDL_Keycode), sizeof(bool));
    player->up = up;
    player->left = left;
    player->right = right;
    object->speed = speed;
    player->onGround = false;
    player->object->applyCollision = &applyPlayerCollision;
    return player;
}

Wall *createWall(Object* object)
{
    Wall *wall = malloc(sizeof(Wall));

    if (wall == NULL)
    {
        freeObject(object);
        return NULL;
    }

    wall->object = object;
    wall->object->type = OBJECT_WALL;
    wall->object->wrapper = wall;
    wall->object->shapeType = SHAPE_RECTANGLE;
    wall->object->applyCollision = &applyWallCollision;
    return wall;

}

void freePlayer(Player * player){
    if (player == NULL)
    {
        return;
    }

    freeObject(player->object);
    free(player);
}

Ball *createBall(Object * object){
    Ball * ball = malloc(sizeof(Ball));
    ball->object = object;
    ball->object->type = OBJECT_BALL;
    ball->object->wrapper = ball;
    ball->object->shapeType = SHAPE_CIRCLE;

    return ball;
}

void freeBall(Ball * ball){
    freeObject(ball->object);
    free(ball);
}

void setVeloX(Object *player, float veloX) {
    player->veloX = veloX;
}

//void setVeloY(Player *player, int accelY){
//
//  	accelY += gravity;
//	player->veloY += accelY * ;
//}us

void updateXY(Object *object, float newFrameTime) {
    // first frame of the game
    if (object->lastMoveTime == 0) object->lastMoveTime = newFrameTime;

    float moveTime = (newFrameTime - object->lastMoveTime) / 1000;
    object->X += object->veloX * moveTime;  //update X

    float oldVeloY = object->veloY;         //update Y
    object->veloY += gravity * moveTime;
    object->Y += (object->veloY+oldVeloY)/2 * moveTime;

    // touched the ground border
    if (object->Y >= SCREEN_HEIGHT - object->H) {
        if(object->type == OBJECT_PLAYER) {
            ((Player*) object->wrapper)->onGround = true ;
            object->veloY = 0;
        }
        else if(object->type == OBJECT_BALL) {
//            object->veloY = 0;
            reflectVectorAboutVector(&(object->veloX), &(object->veloY), 0, -object->H);
//        printf("-------------------\n");
        }
        object->Y = SCREEN_HEIGHT - object->H;

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

    object->lastMoveTime = newFrameTime;

    if (object->type == OBJECT_PLAYER){
        ((Player*)(object->wrapper))->isCollided = false;
    }
                printf("|%i| X%f Y%f velX%f velY%f \n  ", object->id, object->X, object->Y, object->veloX, object->veloY);
}

void move(Player *player, int left, int up, int right) {
    if(player->isCollided) return;
    float veloX = 0;

    if (!player->onGround) veloX = player->object->veloX;
    else if (left == 0 && right == 0) veloX = 0;

    if (player->onGround || player->object->veloX == 0){
        if (left == 1 && right == 0) veloX = -player->object->speed;
        else if (right == 1 && left == 0) veloX = player->object->speed;
    }
//    veloX = (right == 1) ? speed : 0;

    if (up == 1 && player->onGround) {
//	  veloY -= 50;
        player->object->accelY = -100;
        player->object->veloY = -675;
        player->onGround = false;
    }

    setVeloX(player->object, veloX);
//    setVeloY(player, accelY);

}

void applyPlayerCollision(Object *playerObj, Object *target, float *collisionX, float *collisionY){
    ((Player*)playerObj->wrapper)->isCollided = true;
    if (target->type == OBJECT_WALL){
        float normalX = playerObj->X + playerObj->W/2 - *collisionX,
                normalY = playerObj->Y + playerObj->H/2 - *collisionY;
        reflectVectorAboutVector(&(playerObj->veloX), &(playerObj->veloY), normalX, normalY);

        pushOut(playerObj, *collisionX, *collisionY, playerObj->W/2);
//        playerObj->veloX = 0; playerObj->veloY = 0;
        playerObj->veloX /= 1000; playerObj->veloY /= 1000;
    }

    if (target->type == OBJECT_PLAYER){
        reflectVectorAboutVector(&(playerObj->veloX), &(playerObj->veloY),
                                 target->X - playerObj->X, target->Y - playerObj->Y);
        pushOut(playerObj, target->X + target->W/2, target->Y + target->H/2, playerObj->W);
//        playerObj->veloX = 0; playerObj->veloY = 0;
        if (*collisionY > target->Y){
            playerObj->veloY = 0;
        }
        else {
            playerObj->veloY /= 1000;
        }
        playerObj->veloX /= 1000;
//                if(source->veloY != 0)printf("reflected velo X%f, Y%f. \n\n", source->veloX, source->veloY);
    }
//    ((Player*)(playerObj->wrapper))->isCollided = false;
    if (isMovingCloser(playerObj, target)) {
//                if(source->veloY>=0) {
//                    printf(" p%i veloX%f \n",source->id, source->veloX);
//                    printf("%i move to %i with velo X%f, Y%f. \n", source->id, target->id, source->veloX, source->veloY);
//                }

    } else {
//                if(source->veloY>=0) {
//                    printf(" p%i veloX%f \n",source->id, source->veloX);
//                    printf("%i move away %i with velo X%f, Y%f. \n", source->id, target->id, source->veloX, source->veloY);
//                }
    }
}

bool solveQuadEquation(float a, float b, float c, float *x1, float *x2){
    printf("a: %f, b: %f, c: %f", a,b,c);
    float delta = b*b - 4*a*c;
    if (delta < 0){
        return false;
    }
    else {
        float sqrtDelta = sqrtf(delta);
        *x1 = (-b - sqrtDelta)/(2*a);
        *x2 = (-b + sqrtDelta)/(2*a);
        return true;
    }
}

void pushOut(Object *pushedObj, float stableX, float stableY, float targetDistance){
    if (pushedObj->veloY == 0 && pushedObj->veloX == 0) return;
    float currentToStableX = stableX - (pushedObj->X + pushedObj->W/2);
    float currentToStableY = stableY - (pushedObj->Y + pushedObj->H/2);
    float dotProduct = currentToStableX*(pushedObj->veloX) + currentToStableY*(pushedObj->veloY);
    float currentToStableDistSqr = currentToStableX*currentToStableX + currentToStableY*currentToStableY;
    float veloMagnitude = sqrtf(pushedObj->veloY*pushedObj->veloY + pushedObj->veloX*pushedObj->veloX);
//     pushedDistance^2 - 2 *(dotProduct/veloMagnitude)* pushedDistance
//          - targetDistance^2 + currentToStableDist^2 = 0;
    float x1, x2;
    if (veloMagnitude == 0) return;
    if (solveQuadEquation(1, -2*dotProduct/veloMagnitude, -targetDistance*targetDistance+currentToStableDistSqr,
        &x1, &x2)){
        float pushedDistance = 0;
        if (x1 >= 0){
            if (x1 <= x2) pushedDistance = x1;
        }
        else if (x2 >= 0){
            pushedDistance = x2;
        }
        pushedDistance += 1;
        pushedObj->X += pushedObj->veloX * pushedDistance / veloMagnitude;
        pushedObj->Y += pushedObj->veloY * pushedDistance / veloMagnitude;
        printf("distance: %f", pushedDistance);
    }
}

void applyWallCollision(Object *wallObject, Object *target, float *collisionX, float *collisionY)
{
//    ((Wall*)wallObject->wrapper)->isCollided = true;

    if (isMovingCloser(target, wallObject))
    {
//        float normalX = target->X + target->W/2 - *collisionX,
//            normalY = target->Y + target->H/2 - *collisionY;
//        reflectVectorAboutVector(&(target->veloX), &(target->veloY), normalX, normalY);
//
//        pushOut(target, *collisionX, *collisionY, target->W/2);
    }

//    printf("wall collided \n");
}
//bool setPlayerOnGround(Player *player){
//    if (player->object->Y >= SCREEN_HEIGHT - player->object->H) {
//        player->object->Y = SCREEN_HEIGHT - player->object->H;
////        if (!player->onGround) printf("%f \n", player->object->veloY);
//        player->object->veloY = 0;
//        player->object->accelY = 0;
//        player->onGround = true;
//    }
//}

//float centerDistSquared(Object *source, Object *target){
//    float sourceCenterX = source->X + source->W/2, sourceCenterY = source->Y + source->H/2;
//    float targetCenterX = target->X + target->W/2, targetCenterY = target->Y + target->H/2;
//    float distX = (float) fabs(sourceCenterX - targetCenterX);
//    float distY = (float) fabs(sourceCenterY - targetCenterY);
//    return distX * distX + distY * distY; // Pythagorean
//}
float distSquared(float x1, float x2, float y1, float y2)
{
    float deltaX = x2 - x1;
    float deltaY = y2 - y1;
    return deltaY * deltaY + deltaX * deltaX;
}

//printf("%i collide %i\n", player->pos, players[j]->pos);
bool isCircleCollided(Object *A, Object *B, float *collisionX, float *collisionY) {
    if(A->type == OBJECT_WALL) return false;

    int rA = A->W / 2, rB = B->W / 2;
    float ACenterX = A->X + A->W/2, ACenterY = A->Y + A->H/2;
    float BCenterX = B->X + B->W/2, BCenterY = B->Y + B->H/2;
    float distX = BCenterX - ACenterX;
    float distY = BCenterY - ACenterY;
    float centerDistSquared = distX * distX + distY * distY;
//    float centerDist = sqrtf(centerDistSquared);
    if (centerDistSquared < (rA + rB)*(rA + rB)) {
//        *collisionX = ACenterX + distX / centerDist * rA;
//        *collisionY = ACenterY + distY / centerDist * rA;
        return true;
    }
    else return false;
}



bool isWallCollided(Object *A, Object *B, float *collisionX, float *collisionY)
{
    if (B->type != OBJECT_WALL) return false;

    float closestBoxX, closestBoxY = 0;
    /*
     * Find the closest point's X from A's center to B
     * If A is to left of B then the closest point is always on the left edge of B and vice versa
     */

    float radiusA = A->W / 2;

    float center_A_X = A->X + radiusA;
    float center_A_Y = A->Y + radiusA;

    closestBoxX = fmaxf(B->X, fminf(center_A_X, B->X + B->W));
    closestBoxY = fmaxf(B->Y, fminf(center_A_Y, B->Y + B->H));

    if (distSquared(center_A_X, closestBoxX, center_A_Y, closestBoxY) < radiusA * radiusA)
    {
        *collisionX = closestBoxX;
        *collisionY = closestBoxY;
        return true;
//        printf("collided \n");
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


void checkCollision(Object * source, Object *target)
{

    for (int i = 0; i < numPlayer; ++i)
    {
        if (target == source) continue;
        bool collided = false;

        float collideX = 0, collideY = 0;

        if (target->shapeType == SHAPE_CIRCLE)
        {
            collided = isCircleCollided(source, target, &collideX, &collideY);
        }

        if (target->shapeType == SHAPE_RECTANGLE)
        {
            collided = isWallCollided(source, target, &collideX, &collideY);
        }

        if (collided)
        {
            source->applyCollision(source, target, &collideX, &collideY);
            target->applyCollision(target, source, &collideX, &collideY);

//            printf("normal X %f Y %f \n", target->X - source->X, target->Y - source->Y);
//            if (oldVeloY == 0) source->accelY = 0;
//            else {
//                printf("accelY1: %d\n", (source->accelY));
//                source->accelY *= source->veloY/oldVeloY;
//                printf("accelY: %d\n\n", (source->accelY));
//            }
//            source->veloY = oldVeloY;
//            collisionPoint->X = X;
//            collisionPoint->Y = Y;
        }
    }
}

bool isMovingCloser(Object * source, Object * target){
    //check if the source object is moving toward the target or not
    /**
     * https://math.stackexchange.com/questions/1438002/determine-if-objects-are-moving-towards-each-other
     * The velocity of object 2 relative to object 1 is given by v = v2−v1.
     * The displacement of object 2 from object 1 is given by d =p2−p1.
     * We may simply take the dot product v⋅d
     * If the result is positive, then the objects are moving away from each other.
     * If the result is negative, then the objects are moving towards each other.
     * If the result is 00, then the distance is (at that instance) not changing.
     */
    float vX = target->veloX - source->veloX;
    float vY = target->veloY - source->veloY;
    float dX = target->X - source->X;
    float dY = target->Y - source->Y;
    float dotProduct = vX*dX + vY*dY;
    printf("away: %i\n", dotProduct<=0);
    return dotProduct <= 0;
}

