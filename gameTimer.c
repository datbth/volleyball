//
// Created by Dzũng Lê on 4/24/17.
//

#include "gameTimer.h"

GameTimer * createTimer()
{
  GameTimer *temp = malloc(sizeof(GameTimer));
  if (temp == NULL) return NULL;
  else
  {
	temp->mStartTicks = 0;
	temp->mPausedTicks = 0;
	temp->started = false;
	temp->paused = false;

	return temp;
  }

}
