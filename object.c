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
    player->object->isCollided = &isCircleCollided;

    player->oldX = player->object->X;
    player->oldY = player->object->Y;
//    printf("  sizeof Player keycode %lu bool %lu \n", sizeof(SDL_Keycode), sizeof(bool));
    player->up = up;
    player->left = left;
    player->right = right;
    object->speed = speed;
    player->onGround = false;
    player->isCollided = false;
    return player;
}
void freePlayer(Player * player){
    freeObject(player->object);
    free(player);
}

Ball *createBall(Object * object){
    Ball * ball = malloc(sizeof(Ball));
    ball->object = object;
    ball->object->type = OBJECT_BALL;
    ball->object->wrapper = ball;
    ball->object->isCollided = &isCircleCollided;

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
    if (object->X < 0) object->X = 0;
    // touched the right side of the screen
    if (object->X > SCREEN_WIDTH - object->W) object->X = SCREEN_WIDTH - object->W;

    object->lastMoveTime = newFrameTime;

    if (object->type == OBJECT_PLAYER){
        ((Player*)(object->wrapper))->isCollided = false;
    }
}

void move(Player *player, int left, int up, int right) {
    if(player->isCollided) return;
    float veloX = 0;

    if (!player->onGround) veloX = player->object->veloX;
    else if (left == 0 && right == 0) veloX = 0;

    if (left == 1 && right == 0) veloX = -player->object->speed;
    else if (right == 1 && left == 0) veloX = player->object->speed;
//    veloX = (right == 1) ? speed : 0;

    if (up == 1 && player->onGround) {
//	  veloY -= 50;
        player->object->accelY = -100;
        player->object->veloY = -player->object->speed;
        player->onGround = false;
    }

    setVeloX(player->object, veloX);
//    setVeloY(player, accelY);

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

float distSquare(Object * source, Object * target){
    float sourceCenterX = source->X + source->W/2, sourceCenterY = source->Y + source->H/2;
    float targetCenterX = target->X + target->W/2, targetCenterY = target->Y + target->H/2;
    float distX = (float) fabs(sourceCenterX - targetCenterX);
    float distY = (float) fabs(sourceCenterY - targetCenterY);
    return distX * distX + distY * distY; // Pythagorean
}

//printf("%i collide %i\n", player->pos, players[j]->pos);
bool isCircleCollided(Object *A, Object *B) {
    if(A->type == OBJECT_WALL) return false;

    int rA = A->W / 2, rB = B->W / 2;
    if (distSquare(A, B) < (rA + rB)*(rA + rB)) return true;
    else return false;
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


void checkCollision(Object * source, Player* players[]){
//    printf("source %i", source->id);
    for (int i = 0; i < numPlayer; ++i) {
        Object * target = players[i]->object;
        if(target!=source && source->isCollided(source, target)){
            if(source->type == OBJECT_PLAYER)((Player*)(source->wrapper))->isCollided = true;
            if(target->type == OBJECT_PLAYER)((Player*)(target->wrapper))->isCollided = true;

            if (isMovingCloser(source, target)) {
//                if(source->veloY>=0) {
//                    printf(" p%i veloX%f \n",source->id, source->veloX);
//                    printf("%i move to %i with velo X%f, Y%f. \n", source->id, target->id, source->veloX, source->veloY);
//                }
                reflectVectorAboutVector(&(source->veloX), &(source->veloY),
                                         target->X - source->X, target->Y - source->Y);
//                if(source->veloY != 0)printf("reflected velo X%f, Y%f. \n\n", source->veloX, source->veloY);
            }else {
//                if(source->veloY>=0) {
//                    printf(" p%i veloX%f \n",source->id, source->veloX);
//                    printf("%i move away %i with velo X%f, Y%f. \n", source->id, target->id, source->veloX, source->veloY);
//                }
            }

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
    return dotProduct <= 0;
}
