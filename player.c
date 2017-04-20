//
// Created by hai on 4/19/17.
//

#include <SDL2/SDL_system.h>
#include "player.h"
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
        temp->size = 50;
        temp->icon = icon;
        temp->speed = 50;
        temp->minY = SCREEN_HEIGHT - temp->size;
        temp->maxY = temp->minY-100;
        temp->veloX = 0;
        temp->veloY = 0;
        temp->jumping = 0;
        temp->X = 0;
        temp->Y = 0;
        return temp;
    }

}

void setVeloX(Player *player, int veloX){
    player->veloX = veloX;
}

void setVeloY(Player *player){
    if(player->jumping == 1){
        if(player->Y <= player->maxY) player->veloY = 
    } else {

    }
    if(player->Y == player->minY) player->veloY = veloY;
    else player->veloY = 0;
}

void updateXY(Player * player, int gravity){
    int dirY = player->veloY + gravity, Y = player->Y,
            dirX = player->veloX + player->X;

    //prevent the player go out of the Screen width
    if (dirX >= 0 && dirX <= SCREEN_WIDTH-player->size) player->X += player->veloX;

    // prevent players go under ground or jump too high
    if (player->jumping == 1 && (Y+dirY) > player->maxY ) player->Y += dirY;   //if player is jumping and it does not jump too high then let it jump
    else if (player->Y < player->minY) player->Y += dirY;      //if player is not jumping and it still on air, let it fall
    if(player->Y > player->minY) player->Y = player->minY;
}
