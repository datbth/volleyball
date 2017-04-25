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
        if ( pos == NULL ) {
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
	  	temp->onGround = true;
        return temp;
    }

}

void setVeloX(Player *player, int veloX){
    player->veloX = veloX;
}



void setVeloY(Player *player, int veloY){
  	veloY += gravity;
	player->veloY = veloY;
}

void updateXY(Player * player){
    player->X += player->veloX;
    player->Y += player->veloY;

  	// touched the ground border
    if(player->Y > player->minY)
	{
	  player->Y = player->minY;
	  player->onGround = true;
	}

  // touched the left side of the screen
    if(player->X < 0) player->X = 0;
  // touched the right side of the screen
    if(player->X > SCREEN_WIDTH-player->size) player->X = SCREEN_WIDTH-player->size;

  	printf("%i \n", player->veloY);
}

void move(Player* player, int left, int up, int right){
    int veloX = 0, veloY = 0, speed = player->speed;

    veloX += (left == 1) ? -speed : 0;
    veloX += (right == 1) ? speed : 0;

  	if (up == 1 && player->onGround)
	{
	  veloY -= 20;
	  player->onGround = false;
	}
  	else if (up == 0)
	{
	  if (veloY < -10)
	  {
		veloY = -10;
	  }
	}
//  	printf("%i \n", veloY);

    setVeloX(player, veloX);
    setVeloY(player, veloY);

}

