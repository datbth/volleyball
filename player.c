//
// Created by hai on 4/19/17.
//

#include <SDL2/SDL_system.h>
#include "player.h"
#include "stdbool.h"
#include "main.h"



Player *createPlayer(int pos, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right, char* file_path){
    int status = 0;
    Player *temp = malloc(sizeof(Player));
    SDL_Texture *icon = loadTexture(file_path, renderer);
    if (temp == NULL || icon == NULL) return NULL;
    else {
        if (pos == NULL ) {
            free(temp);
            return NULL;
        }
        temp->pos = pos;
        temp->up = up;
        temp->left = left;
        temp->right = right;
        temp->icon = icon;
        temp->size = 100;
        temp->speed = 10;
        temp->minY = SCREEN_HEIGHT - temp->size;
        temp->maxY = temp->minY-temp->size;
        temp->veloX = 0;
        temp->veloY = 0;
        temp->X = 0;
        temp->Y = 0;
        return temp;
    }

}

void setVeloX(Player *player, int veloX){
    player->veloX = veloX;
}

void setVeloY(Player *player, int veloY){
    // check if player is on ground
    // if on ground -> can jump
    //      if up is pressed -> increase vel
    //      else -> nothing

    // if not on ground -> can not jump
    //      check if player is falling
    //      if player is falling -> return
    //      if player is not falling
    //          if up is pressed -> increase vel
    //          else -> vel = 0

    bool isOnGround = player->Y == player->minY;
    if(isOnGround){
        if(veloY != 0) {
            player->veloY = -veloY;
        }
    } else {
        if(player->Y < player->maxY) {  //if player jumps too high
//            printf("13123121\n");
            player->veloY = 0;
        }
        bool isFalling = player->veloY >= 0;
        if(!isFalling){
            if (veloY != 0) player->veloY = -veloY;
            else player->veloY = 0;
        }
    }
}

void updateXY(Player * player){
    player->X += player->veloX;
    player->Y += (player->veloY + gravity);

    if(player->Y > player->minY) player->Y = player->minY;
//    if(player->Y < player->maxY) player->Y = player->maxY;
    if(player->X < 0) player->X = 0;
    if(player->X > SCREEN_WIDTH-player->size) player->X = SCREEN_WIDTH-player->size;
}

void move(Player* player, int left, int up, int right){
    int veloX = 0, veloY = 0, speed = player->speed;
    veloX += (left == 1)? -speed : 0;
    veloX += (right == 1)? speed : 0;

    veloY += (up == 1)? speed : 0;

    setVeloX(player, veloX);
    setVeloY(player, veloY);
}