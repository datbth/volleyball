//
// Created by Dzũng Lê on 4/24/17.
//

#ifndef GAME_GAMETIMER_H
#define GAME_GAMETIMER_H

#include <SDL_quit.h>
#include <stdbool.h>

typedef struct
{
	Uint32 mStartTicks;
	Uint32 mPausedTicks;

	bool started;
	bool paused;
} GameTimer;

GameTimer * createTimer();
void startTimer();
void stopTimer();
void pauseTimer();
void unpauseTimer();
Uint32 getTicks();

#endif //GAME_GAMETIMER_H
