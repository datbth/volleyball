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
    Object *temp = malloc(sizeof(Object));
    if (temp == NULL) {
        SDL_DestroyTexture(icon);
        return NULL;
    }
    temp->speed = 0;
    temp->id = id;
    temp->image = icon;
    temp->H = H;
    temp->W = W;
    temp->X = X;
    temp->Y = Y;
    temp->veloX = 0;
    temp->veloY = 0;
    temp->accelY = 0;
    temp->lastMoveTime = 0;
}

void freeObject(Object * object){
    SDL_DestroyTexture(object->image);
    free(object);
}

Player *createPlayer(Object* object,int speed, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right){
    if (object == NULL) return NULL;
//    printf("sizeof Player %lu \n", sizeof(Player));
    Player *temp = malloc(sizeof(Player));
    if (temp == NULL){
        printf("player NULL");
        freeObject(object);
        return NULL;
    }
    temp->object = object;
    temp->object->wrapper = temp;
    temp->object->type = OBJECT_PLAYER;
    temp->object = object;
//    printf("  sizeof Player keycode %lu bool %lu \n", sizeof(SDL_Keycode), sizeof(bool));
    temp->up = up;
    temp->left = left;
    temp->right = right;
    object->speed = speed;
    temp->onGround = true;
    return temp;
}
void freePlayer(Player * player){
    freeObject(player->object);
    free(player);
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
//    printf("last move %f current %f\n", object->lastMoveTime, newFrameTime);
    if (object->lastMoveTime == 0) object->lastMoveTime = newFrameTime;

    float moveTime = (newFrameTime - object->lastMoveTime) / 1000;
    object->X += object->veloX * moveTime;  //update X

    object->accelY += gravity;              //update Y
    object->veloY += object->accelY * moveTime;
    object->Y += object->veloY * moveTime;

    // touched the ground border
    if(object->type == OBJECT_PLAYER) setPlayerOnGround((Player*) object->wrapper);

    // touched the left side of the screen
    if (object->X < 0) object->X = 0;
    // touched the right side of the screen
    if (object->X > SCREEN_WIDTH - object->W) object->X = SCREEN_WIDTH - object->W;

    object->lastMoveTime = newFrameTime;
}

void move(Player *player, int left, int up, int right) {
    float veloX = 0, veloY = 0, accelY = 0, speed = player->object->speed;

    if (!player->onGround) {
        veloX = player->object->veloX;
    } else if (left == 0 && right == 0) {
        veloX = 0;
    }
    if (left == 1 && right == 0) {
        veloX = -speed;
    } else if (right == 1 && left == 0) {
        veloX = speed;
    }
//    veloX = (right == 1) ? speed : 0;

    if (up == 1 && player->onGround) {
//	  veloY -= 50;
        player->object->accelY = -100;
        player->object->veloY = -500;
        player->onGround = false;
    }


    setVeloX(player->object, veloX);
//    setVeloY(player, accelY);

}
bool setPlayerOnGround(Player *player){
    if (player->object->Y > SCREEN_HEIGHT - player->object->H) {
        player->object->Y = SCREEN_HEIGHT - player->object->H;
        player->object->veloY = 0;
        player->object->accelY = 0;
        player->onGround = true;
    }
}

//printf("%i collide %i\n", player->pos, players[j]->pos);
bool isCollided(Object *A, Object *B) {
    int rA = A->W / 2, rB = B->W / 2;
    float centerA_X = A->X + rA, centerA_Y = A->Y + rA;
    float centerB_X = B->X + rB, centerB_Y = B->Y + rB;
    float distX = (float) fabs(centerA_X - centerB_X);
    float distY = (float) fabs(centerA_Y - centerB_Y);
    // Pythagorean
    float centerDist = distX * distX + distY * distY;   // avoid using square root
    if (centerDist < (rA + rB)*(rA + rB)) return true;
    else return false;
}

void getCollideXY(float *X, float *Y, Object* A, Object *B){
    float centerA_X = A->X + A->W/2, centerA_Y = A->Y + A->W/2;
    float centerB_X = B->X + B->W/2, centerB_Y = B->Y + B->W/2;
    float distX = (float) fabs(centerA_X - centerB_X);
    float distY = (float) fabs(centerA_Y - centerB_Y);
    double alpha = atan(distY / distX) * 180 / M_PI;
    if(A->Y > B->Y){
        if (A->X > B->X){
            *X = centerA_X - (float) (A->W/2 * cos(alpha));
            *Y = centerA_Y - (float) (A->W/2 * sin(alpha));
        } else {
            *X = centerA_X + (float) (A->W/2 * cos(alpha));
            *Y = centerA_Y + (float) (A->W/2 * sin(alpha));
        }
    } else {
        if (A->X > B->X){
            *X = centerB_X + (float) (B->W/2 * cos(alpha));
            *Y = centerB_Y + (float) (B->W/2 * sin(alpha));
        } else {
            *X = centerB_X - (float) (B->W/2 * cos(alpha));
            *Y = centerB_Y - (float) (B->W/2 * sin(alpha));
        }
    }
}

void pushback(Object * object, float X, float Y){
//    double inAngle = atan(object->veloX / object->veloY) * 180 / M_PI;
    float centerB_X = object->X + object->W/2, centerB_Y = object->Y + object->W/2;
    float distX =  (X - centerB_X);
    float distY =  (Y - centerB_Y);
    object->veloX -= distX*2;
    object->veloY -= distY*2;
}


int reflectVectorAboutVector(float *vectorX, float *vectorY, float normalX, float normalY){
    //https://en.wikipedia.org/wiki/Reflection_%28mathematics%29#Reflection_across_a_line_in_the_plane
    //https://en.wikipedia.org/wiki/Dot_product
    //https://www.varsitytutors.com/hotmath/hotmath_help/topics/adding-and-subtracting-vectors
    //https://www.evl.uic.edu/dmitr/classes/cs527/proj2/
    if (*vectorX == 0 && *vectorY==0) return 1;

//    coefficient = 2 * dotProductVL * dotProductLL

    float coefficient = 2 *(*vectorX * normalX + *vectorY * normalY)
                        /(normalX * normalX + normalY * normalY);
    *vectorX = (float) (-1.05 * (coefficient * normalX - *vectorX));
    *vectorY = (float) (-1.05 * (coefficient * normalY - *vectorY));
//    printf("velo X %f Y %f\n\n", *vectorX, *vectorY);
    return 0;
}


void checkCollision(Object * object, Player* players[], Object * collision){
    for (int i = 0; i < numPlayer; ++i) {
        Object * other = players[i]->object;
        if(other!=object && isCollided(object, other)){
            float X = 0, Y = 0;
            getCollideXY(&X, &Y, object, other);
//            printf("collision at X %f Y %f \n", X, Y);
            if (1 == reflectVectorAboutVector(&(object->veloX), &(object->veloY),
                                     (float) fabs(object->X - X), (float) fabs(object->Y - Y))
                    ) pushback(object, X, Y);
            collision->X = X;
            collision->Y = Y;
        }
    }
}
