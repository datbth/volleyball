//
// Created by hai on 4/19/17.
//

#include <SDL2/SDL_system.h>
#include <time.h>
#include "player.h"
#include "stdbool.h"
#include "main.h"

Player *createPlayer(int pos, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right, char* file_path){
  int status = 0;
  Player *temp = malloc(sizeof(Player));
  SDL_Texture *icon = loadTexture(file_path, renderer);
  if (temp == NULL || icon == NULL) return NULL;
  else {
//        if (&pos == NULL )
//		{
//            free(temp);
//            return NULL;
//        }
	temp->pos = pos;
	temp->up = up;
	temp->left = left;
	temp->right = right;
	temp->icon = icon;
	temp->size = 100;
	temp->speed = 500;
	temp->minY = SCREEN_HEIGHT - temp->size;
	temp->maxY = temp->minY-temp->size;
	temp->veloX = 0;
	temp->veloY = 0;
	temp->accelY = 0;
	temp->X = 0;
	temp->Y = 0;
	temp->onGround = true;
	temp->lastMoveTime = 0;
	return temp;
  }

}

void setVeloX(Player *player, float veloX){
  player->veloX = veloX;
}



//void setVeloY(Player *player, int accelY){
//
//  	accelY += gravity;
//	player->veloY += accelY * ;
//}

void updateXY(Player * player, float newFrameTime){
  if (player->lastMoveTime == 0){
	player->lastMoveTime = newFrameTime;
  }
  float moveTime = (newFrameTime - player->lastMoveTime) / 1000;
  player->X += player->veloX * moveTime;

//  player->veloY = player->accelY;
  player->accelY += gravity;
  player->veloY += player->accelY * moveTime;
  player->Y += player->veloY * moveTime;

//  printf("%f \n", player->veloY);


  // touched the ground border
  if(player->Y > player->minY)
  {
	player->Y = player->minY;
	player->veloY = 0;
//	  player->veloX = 0;
	player->accelY = 0;
	player->onGround = true;
  }

  // touched the left side of the screen
  if(player->X < 0) player->X = 0;
  // touched the right side of the screen
  if(player->X > SCREEN_WIDTH-player->size) player->X = SCREEN_WIDTH-player->size;
  player->lastMoveTime = newFrameTime;
//  	printf("%i \n", player->veloY);
}

void move(Player* player, int left, int up, int right){
  float veloX = 0, veloY = 0, accelY = 0, speed = player->speed;

  if (!player->onGround){
	veloX = player->veloX;
  }
  else if (left == 0 && right ==0) {
	veloX = 0;
  }
  if (left == 1 && right == 0){
	veloX = -speed;
  }
  else if (right == 1 && left == 0){
	veloX = speed;
  }
//    veloX = (right == 1) ? speed : 0;

  if (up == 1 && player->onGround)
  {
//	  veloY -= 50;
	player->accelY = -100;
	player->veloY = -500;
	player->onGround = false;
  }



  setVeloX(player, veloX);
//    setVeloY(player, accelY);

}

