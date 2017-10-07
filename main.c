#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
//#include <SDL_ttf.h>
#include <time.h>
#include "object.h"
#include "main.h"

SDL_Renderer *renderer;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const float TIME_PER_FRAME = 1000 / SCREEN_FPS;
const Uint8 *currentKeyStates;
const int gravity = 1500;

TTF_Font *font;
char *fontPath = "../DejaVuSans-Bold.ttf";
//int numObjects = 0;
float timeScale = 1.0;
Player *players[4];
int desiredPlayers = 2;
int winningScore = 10;
Mix_Chunk *sounds[10];
bool hasIntro = false;
SDL_Texture * introTexture[20];


void resetPositions(int teamHasBall){
	/**
	 * reset all players to the initial positions and remove velocity
	 * if teamHasBall == 0, ball will be put in randomly
	 */

	if(teamHasBall == 0) teamHasBall = (rand() % 2)+1;
//    printf("team has ball %i\n", teamHasBall);
	for (int i = 0; i < objects->size; ++i) {
		Object * currentObj = objects->data[i];
		currentObj->X = 0;
		currentObj->Y = 0;
		currentObj->veloX = 0;
		currentObj->veloY = 0;
		switch (currentObj->type) {
			case OBJECT_PLAYER:
                if(objects->numPlayer == 2){
                    currentObj->X = SCREEN_WIDTH/2 * i + SCREEN_WIDTH/4 - currentObj->W/2 ;
                }else{
                    currentObj->X = SCREEN_WIDTH/4 * ((objects->numPlayer>2)?i:i*2) +SCREEN_WIDTH/8 - currentObj->W/2;
                }
				currentObj->Y = SCREEN_HEIGHT - currentObj->H;
				break;
			case OBJECT_BALL:
				if (teamHasBall == 1) {
					currentObj->X = players[0]->object->X + players[0]->object->W/2 - currentObj->W / 2;
				} else if (teamHasBall == 2) {
					currentObj->X = players[objects->numPlayer/2+(objects->numPlayer>2?1:0)]->object->X + players[0]->object->W/2 - currentObj->W / 2;
				}
				break;
			case OBJECT_WALL:
				currentObj->X = SCREEN_WIDTH / 2 - currentObj->W/2;
				currentObj->Y = SCREEN_HEIGHT - currentObj->H;
				break;
			case OBJECT_ITEM:break;
		}
	}
}

void getScoreString(char *string, int teamPoint1st, int teamPoint2nd){
	int strMaxLen = 6;
	char scoreString1[strMaxLen], scoreString2[strMaxLen];
	snprintf(scoreString1, (size_t) strMaxLen, "%d", teamPoint1st);
	snprintf(scoreString2, (size_t) strMaxLen, "%d", teamPoint2nd);
	strcat(scoreString1, "-");
	strcat(scoreString1, scoreString2);
	for (int i = 0; i < 6; ++i) {
		string[i] = scoreString1[i];
	}
}

int updateScore(int ballID, int *teamPoint1st, int *teamPoint2nd, int *winningTeam){
    Object * ballObj = objects->data[ballID];
    if (teamPoint1st == 0 && teamPoint2nd == 0){
        *winningTeam = 0;
    }
	if(ballObj->Y >= SCREEN_HEIGHT-ballObj->H){
//            printf("touch ground\n");
		int teamHasBall = 0;

		if(ballObj->X <SCREEN_WIDTH/2) {
			(*teamPoint2nd)++;
            teamHasBall = 1;
        } else {
            (*teamPoint1st)++;
            teamHasBall = 2;
		}
		if (*teamPoint1st == winningScore)
		{
			*winningTeam = 1;
			*teamPoint1st = 0;
			*teamPoint2nd = 0;
		}
		else if (*teamPoint2nd == winningScore)
		{
			*winningTeam = 2;
			*teamPoint1st = 0;
			*teamPoint2nd = 0;
		}
		return teamHasBall;
	}
	return -1;
}


SDL_Texture * renderText(char * text, SDL_Color color){
	SDL_Texture * scoreTexture;
	SDL_Surface * textSurface = TTF_RenderText_Solid(font, text, color);
	if (textSurface == NULL) return NULL;
	else{
		scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if(scoreTexture == NULL) return NULL;
	}
	SDL_FreeSurface(textSurface);
	return scoreTexture ;
}

void showInstruction(SDL_Color color, int fontX, int fontY, SDL_Rect *background){
    for (int l = 0; l < 20; ++l) {
        if(introTexture[l]) SDL_DestroyTexture(introTexture[l]);
    }
    int counter = 0;
    char * shortcuts[] = {"Ctrl + R to reset position",
                          "Ctrl + P to pause             ",
                          "Ctrl + Q to quit                ",
                          "Ctrl + U to change mode ",
                          "ENTER to Start the Match  "};
//    SDL_Texture * shortcutsTexture = NULL;
    for (int k = 0; k < 5; ++k) {
        SDL_Texture * shortcutsTexture = renderText(shortcuts[k], color);
        introTexture[counter++] = shortcutsTexture;
        SDL_Rect shortcutPos = {SCREEN_WIDTH/2-fontY*10, fontY*2*(k+5), fontY*20, fontY*2};
        SDL_RenderCopyEx(renderer, shortcutsTexture, background, &shortcutPos, 0, 0, SDL_FLIP_NONE);
    }

    char * buttonList[] = {"  A  ", "  W  ", "  D  ",
                           "  J  ", "  I  ", "  L  ",
                           "LEFT ", "  UP ", "RIGHT",
                           "NUM_4", "NUM_8", "NUM_6",};
    for (int i = 0; i < objects->numPlayer; ++i) {
        Object * currentObj = objects->data[i];
        if (desiredPlayers == 2 && i == 1) i = 2;
        SDL_Rect wordPos = {(int) (currentObj->X - currentObj->W/2 - fontX*2.5),
                            (int) (currentObj->Y + currentObj->H/2 - fontY),
                            fontX, fontY};
        int tempPosY = wordPos.y;
        for (int j = 0; j < 3; ++j) {
            SDL_Texture *buttonTexture = renderText(buttonList[i*3+j], color);
            introTexture[counter++] = buttonTexture;
            wordPos.x +=  fontX*2;
            wordPos.y = (j==1)? tempPosY - currentObj->H/2 - fontY : tempPosY;
            SDL_RenderCopyEx(renderer, buttonTexture, background, &wordPos, 0, 0, SDL_FLIP_NONE);
        }
    }
}

SDL_Texture *loadTexture(char *path) 	{
	SDL_Surface *loadedSurface = IMG_Load(path);    //  load PNG image
	SDL_Texture *texture = NULL;

	if (loadedSurface == NULL) {
		printf("can't load %s %s\n", path, IMG_GetError());
		return NULL;
	} else {
		texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (texture == NULL) {
			printf("can't create Texture %s %s\n", path, SDL_GetError());
		}
		SDL_FreeSurface(loadedSurface);
		return texture;
	}
}

SDL_Window *init() {
	SDL_Window *window;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("can't initialize SDL%s\n", SDL_GetError());
		return NULL;
	} else {
		//Create window
		window = SDL_CreateWindow("Volleyball", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			printf("can't initialize Window%s\n", SDL_GetError());
			return NULL;
		}

		if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ){
			printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
			return NULL;
		}
		return window;
	}
}

void togglePause() {
	timeScale = timeScale? 0:1;
}

int handleKeys() {
	if      (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_Q]) return 1;
	else if (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_R]) return 2;
	else if (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_P]) return 3;
	else if (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_U]) return 4;
    else if (currentKeyStates[SDL_SCANCODE_RETURN]) return 5;

	for (int i = 0; i < objects->numPlayer; ++i) {
		Player *player = players[i];
        player_move(player,
                    currentKeyStates[player->left],
                    currentKeyStates[player->up],
                    currentKeyStates[player->right]);
	}
	return 0;
}

void cleanItems(bool checkNeedRemove){
    for (int i = objects->size-1; i > objects->numPlayer; i--) {
        if (objects->data[i]->type == OBJECT_ITEM){
            Item * currentItem = (Item *)(objects->data[i]->wrapper);
            if(checkNeedRemove) {
                if (currentItem->needRemove) {
                    item_removeFromList(currentItem, objects);
                }
            } else {
                item_removeFromList(currentItem, objects);
            }
        }
    }
}

int findObjIDByType(struct objectList *objects, enum ObjectType targetType, int index){
    for (int i = 0; i < objects->size; ++i) {
        if (objects->data[i]->type == targetType){
            if (index == 0) return i;
            else index--;
        }
    }
    return -1;
}

bool isElemInArray(int* arr, int elem, int length){
    for (int i = 0; i < length; i++){
        if (elem == arr[i]) return true;
    }
    return false;
}

int getIntFromArray(char * array){
    int counter = 0;
    while (array[counter] != '\0'){
        if (array[counter] == '0'){
            return counter;
        }
        counter++;
    }
    return -1;
}

void cleanObjects(struct objectList *objects){
    printf("free objects \n");
    for (int m = 0; m < objects->size; ++m) {
        Object *currentObj = objects->data[m];
        if(currentObj){
            printf("id: %i\n", currentObj->id);
            obj_free(currentObj);
        }
    }
    objects->size = 0;
    objects->numPlayer = 0;
    printf("\n");
}

void createGameObj(int desiredPlayers, struct objectList *objects){
    //// CREATE PLAYER
    SDL_Keycode keycode[] = {SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_D,
                             SDL_SCANCODE_I, SDL_SCANCODE_J, SDL_SCANCODE_L,
                             SDL_SCANCODE_UP, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
                             SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_4, SDL_SCANCODE_KP_6};
    char playerImagePath[] = "../pics/player0.png";
    int targetIndex = getIntFromArray(playerImagePath);
    for (int l = 0; l < desiredPlayers; l++) {
        playerImagePath[targetIndex] += 1;
        Object *object = obj_create(objects->size, 100, 100, playerImagePath);
        if(desiredPlayers == 2 ) l *= 2;

        players[objects->numPlayer++] = player_create(object, 1000 / desiredPlayers, 700, keycode[l * 3 + 0],
                                                      keycode[l * 3 + 1], keycode[l * 3 + 2]);
        objAppend(objects, object);
    }
    //// CREATE BALL
    Object *object_b1 = obj_create(objects->size, 50, 50, "../pics/ball2.png");
    ball_create(object_b1);
    objAppend(objects, object_b1);

    //// CREATE WALL
    //the height of the wall is 3 times player's height
    Object *object_w1 = obj_create(objects->size, 25, objects->data[0]->H * 3, "../pics/wall.png");
    wall_create(object_w1);
    objAppend(objects, object_w1);
}

void toggleMode(int * desiredPlayer, struct objectList * objects, int *teamPoint1st, int *teamPoint2nd){
    *teamPoint1st = 0, *teamPoint2nd = 0;
    *desiredPlayer = *desiredPlayer == 2?4:2;
    cleanItems(false);
    cleanObjects(objects);
    createGameObj(*desiredPlayer, objects);
    hasIntro = false;
    resetPositions(0);
}

int main(int argc, char *args[]) {
	int gameQuit = 0;
	//// START UP SDL AND CREATE WINDOW
	SDL_Window *window = init();
	if (window == NULL) return 1;
    SDL_Rect background = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	SDL_Event event;

	// SETUP SOUNDS
	int numSound = 0;
	sounds[numSound++] = Mix_LoadWAV("../sounds/hit1.wav");     // 0
	sounds[numSound++] = Mix_LoadWAV("../sounds/hit2.wav");     // 1
	sounds[numSound++] = Mix_LoadWAV("../sounds/hit3.wav");     // 2
	sounds[numSound++] = Mix_LoadWAV("../sounds/jump1.wav");    // 3
	sounds[numSound++] = Mix_LoadWAV("../sounds/jump2.wav");    // 4
	sounds[numSound++] = Mix_LoadWAV("../sounds/jump3.wav");    // 5
	sounds[numSound++] = Mix_LoadWAV("../sounds/whistle.wav");  // 6
	sounds[numSound++] = Mix_LoadWAV("../sounds/point.wav");    // 7
	Mix_Music *music = Mix_LoadMUS("../sounds/sandstorm.mp3");

	//// SETUP CONTROLLER
	currentKeyStates = SDL_GetKeyboardState(NULL);

	//// SETUP IMAGE RENDERER
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	if (renderer == NULL) return 2;

	//// SETUP FONT RENDERER
	if (TTF_Init() == -1) {
		printf("can't initialize TTF loader%s \n", SDL_GetError());
		return 3;
	}
	font = TTF_OpenFont(fontPath, 20);
	if (font == NULL) {
		printf("can't load font %s\n", SDL_GetError());
		return 3;
	}


	//// SETUP SCORE INDICATOR
	int teamPoint1st = 0, teamPoint2nd = 0;
	char scoreString[10];
	getScoreString(scoreString, teamPoint1st, teamPoint2nd);
	SDL_Color scoreColor = { 0, 255, 0, 255 };
	SDL_Texture * scoreTexture = renderText(scoreString, scoreColor);
	SDL_Texture * winnerTexture = NULL;

	//// SETUP BACKGROUND IMAGE
	SDL_Texture * background_image = loadTexture("../pics/background.png");
	if (background_image == NULL) gameQuit = 1;

	//// CREATE OBJECT ARRAYLIST
	struct objectList *temp_objects = objCreate();
	objects = temp_objects;

	//// CREATE GAME OBJECTS
	createGameObj(desiredPlayers, objects);
	int ballId = findObjIDByType(objects, OBJECT_BALL, 0);

	//// SETUP ITEMS SPAWN
	//    enum ItemType itemList[] = {};
	char itemImagePath[] = "../pics/item0.png";
	int itemImagePathIndex = getIntFromArray(itemImagePath);
	float lastItemSpawnTime = SDL_GetTicks();
	int itemSpawnTime = (rand() % 3) + 5;


	//// VALIDATE PLAYERS AND SET THEIR POSITIONS
	for (int j = 0; j < objects->size; ++j){
		if (objects->data[j] == NULL || (objects->data[j]->wrapper) == NULL) {
			gameQuit = 1;
		}
	}
	if (ballId < 0) gameQuit = 1;
	srand((unsigned int) time(NULL));
	// Play sound when start / startover
    Mix_PlayChannel( -1, sounds[6], 0 );
	resetPositions(0);

	//// PLAY SOUND WHEN START
	Mix_PlayMusic(music, -1);

	float lastTime = 0, currentTime;
	int rotation = 0;

	//// GAME LOOP
	while (gameQuit == 0){
		//// GET KEY INPUT
		do {
			switch (handleKeys()){
				case 1:
					gameQuit = 1;
					break;
				case 2:
					resetPositions(0);
                    cleanItems(false);
					break;
				case 3:
					togglePause();
					break;
                case 4:
                    toggleMode(&desiredPlayers, objects, &teamPoint1st, &teamPoint2nd);
                    ballId = findObjIDByType(objects, OBJECT_BALL, 0);
                    if(scoreTexture != NULL) SDL_DestroyTexture(scoreTexture);
                    getScoreString(scoreString, teamPoint1st, teamPoint2nd);
                    scoreTexture = renderText(scoreString, scoreColor);
                    break;
				default:
					break;
			}
		}
		while (SDL_PollEvent(&event) != 0);
		if (event.type == SDL_QUIT) gameQuit = 1;   // close on top right 'x'

		//// LOCK FPS
		currentTime = SDL_GetTicks();
		int actualTimePerFrame = (int) currentTime - (int) lastTime;
		if (actualTimePerFrame < TIME_PER_FRAME) {
			SDL_Delay((Uint32) ((int) TIME_PER_FRAME - actualTimePerFrame));
		}
		currentTime = SDL_GetTicks();

		//// SPAWN ITEM
		if(currentTime - lastItemSpawnTime > itemSpawnTime*1000){
			Item * newItem = item_createRandomly(itemImagePath, itemImagePathIndex, 3);
			if (newItem != NULL){
                objAppend(objects, newItem->object);
				itemSpawnTime = (rand() % 3) + 5;
				lastItemSpawnTime = currentTime;
			}
		}
//        al_print(objects);

		//// UPDATE LOGIC POSITION
		for (int i = 0; i < objects->size; ++i) {
			if (objects->data[i]->type == OBJECT_ITEM) continue;
            obj_update(objects->data[i], (currentTime - lastTime) * timeScale, gravity);
		}

		//// CHECK COLLISION AND CORRECT THEIR POSITIONS
		int checkedObjIndexes[objects->size];
		int numCheckedObj = 0;
		for (int i = 0; i < objects->size; ++i) {
			for (int j = 0; j < objects->size; j++) {
				if (i == j || isElemInArray(checkedObjIndexes, j + 1, numCheckedObj)) {
					continue;
				}
//                printf("Checking %i and %i\n", i,j);
                obj_checkCollision(objects->data[i], objects->data[j]);
			}
			checkedObjIndexes[numCheckedObj++] = i + 1;
		}
		//// REMOVE ITEMS
        cleanItems(true);


		//// UPDATE SCORE, CHECK WINNER
        int winningTeam;
		if (teamPoint1st == 0 && teamPoint2nd == 0){
			winningTeam = 0;
		}

        int teamHasBall = updateScore(ballId, &teamPoint1st, &teamPoint2nd, &winningTeam);

		if(winnerTexture != NULL) SDL_DestroyTexture(winnerTexture);
		if (winningTeam > 0){
			winnerTexture = renderText("Winner!", scoreColor);
		} else{
			winnerTexture = renderText("", scoreColor);
		}


        //reset position and remove all remaining items
		if (teamHasBall > 0){
            Mix_PlayChannel( -1, sounds[6], 0 );
            cleanItems(false);
            resetPositions(teamHasBall);
            getScoreString(scoreString, teamPoint1st, teamPoint2nd);
			if(scoreTexture != NULL) SDL_DestroyTexture(scoreTexture);
            scoreTexture = renderText(scoreString, scoreColor);
            SDL_Delay(1000);
            currentTime = SDL_GetTicks();
        };


        //// UPDATE UI
        SDL_RenderClear(renderer);  //// clear the last screen
		SDL_RenderCopyEx(renderer, background_image, &background, &background, 0, 0, SDL_FLIP_NONE);

		for (int k = 0; k < objects->size; ++k) {
			Object *currentObj = objects->data[k];
			currentObj->positionRect.x = (int) ceil(currentObj->X);
			currentObj->positionRect.y = (int) ceil(currentObj->Y);
			if (currentObj->type == OBJECT_BALL){
				SDL_RenderCopyEx(renderer, currentObj->image, &background, &currentObj->positionRect, rotation++, 0, SDL_FLIP_NONE);
            } else {
                SDL_RenderCopyEx(renderer, currentObj->image, &background, &currentObj->positionRect, 0, 0, SDL_FLIP_NONE);
            }
        }
        int winPositionX = (SCREEN_WIDTH / 4 * winningTeam) - 100;
        if (winningTeam == 2) winPositionX += (SCREEN_WIDTH / 4);
        SDL_Rect winPosition = {winPositionX, 50, 200, 50};
        SDL_RenderCopyEx(renderer, winnerTexture, &background, &winPosition, 0, 0, SDL_FLIP_NONE);
		SDL_DestroyTexture(winnerTexture);

        SDL_Rect scorePosition = {SCREEN_WIDTH / 2 - 100, 0, 200, 50};
        SDL_RenderCopyEx(renderer, scoreTexture, &background, &scorePosition, 0, 0, SDL_FLIP_NONE);

        if(!hasIntro) showInstruction(scoreColor, 50, 20, &background);

        //// RENDER
		SDL_RenderPresent(renderer);

        if (winningTeam > 0){
            SDL_Delay(2000);
            currentTime = SDL_GetTicks();
        }
        if (!hasIntro){
            while (handleKeys() != 5 ) {
                SDL_PollEvent(&event);
                if (event.type == SDL_QUIT) {
                    gameQuit = 1;
                    break;
                }
                SDL_Delay(10);
            }
            hasIntro = true;
            currentTime = SDL_GetTicks();
        }

		lastTime = currentTime;
	}

	//// FREE RESOURCES AND CLOSE SDL
    cleanObjects(objects);
	al_free(objects);

    for (int l = 0; l < 20; ++l) {
        if(introTexture[l]) SDL_DestroyTexture(introTexture[l]);
    }

	TTF_CloseFont(font); font = NULL;
	SDL_DestroyRenderer(renderer); renderer = NULL;
	SDL_DestroyWindow(window); window = NULL;

	//// FREE SOUND
	for (int i = 0; i < numSound; ++i){
		Mix_FreeChunk(sounds[i]);sounds[i] = NULL;
	}
	Mix_FreeMusic(music); music = NULL;

	//// QUIT SDL SUBSYSTEMS
//	Mix_Quit();
//	TTF_Quit();
//	IMG_Quit();
	SDL_Quit();

	return 0;
}
