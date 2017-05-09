//Using SDL, SDL_image, standard IO
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <SDL_ttf.h>
#include "object.h"
#include "main.h"



//Screen dimension constants
SDL_Renderer *renderer;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
TTF_Font *font;
char * fontPath = "../DejaVuSans-Bold.ttf";
const int gravity = 1500;
const float TIME_PER_FRAME = 1000 / SCREEN_FPS;
int numObjects = 0;
int numPlayer = 0;
float timeScale = 1.0;
Player *players[4];
Object *objects[50];
const Uint8 *currentKeyStates;
Mix_Chunk *sounds[10];

void resetPositions(int teamHasBall){
    /**
     * reset all players to the initial positions and remove velocity
     * if teamHasBall == 0, ball will be put in randomly
     */

    // Play sound when start / startover
    Mix_PlayChannel( -1, sounds[6], 0 );
    if(teamHasBall == 0) teamHasBall = (rand() % 2)+1;
//    printf("team has ball %i\n", teamHasBall);
    for (int i = 0; i < numObjects; ++i) {
        Object * currentObj = objects[i];
        currentObj->X = 0;
        currentObj->Y = 0;
        currentObj->veloX = 0;
        currentObj->veloY = 0;
        switch (currentObj->type) {
            case OBJECT_PLAYER:
                currentObj->X = SCREEN_WIDTH/4 * i +SCREEN_WIDTH/8 - currentObj->W/2;
                currentObj->Y = SCREEN_HEIGHT - currentObj->H;
                break;
            case OBJECT_BALL:
                if (teamHasBall == 1) {
                    currentObj->X = players[0]->object->X + players[0]->object->W/2 - currentObj->W / 2;
                } else if (teamHasBall == 2) {
                    currentObj->X = players[3]->object->X + players[0]->object->W/2 - currentObj->W / 2;
                }
                currentObj->Y = -200;
                currentObj->veloY = -100;
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
    snprintf(scoreString1, strMaxLen, "%d", teamPoint1st);
    snprintf(scoreString2, strMaxLen, "%d", teamPoint2nd);
    strcat(scoreString1, "-");
    strcat(scoreString1, scoreString2);
    for (int i = 0; i < 6; ++i) {
        string[i] = scoreString1[i];
    }
}

int updateScore(Ball *ball, int *teamPoint1st, int *teamPoint2nd){
    if(ball->object->Y >= SCREEN_HEIGHT-ball->object->H){
//            printf("touch ground\n");
        int teamHasBall = 0;
        if(ball->object->X <SCREEN_WIDTH/2) {
            (*teamPoint2nd)++;
            teamHasBall = 2;
        } else {
            (*teamPoint1st)++;
            teamHasBall = 1;
        }
        if (*teamPoint1st == 12 || *teamPoint2nd == 12) {
            *teamPoint1st = 0;
            *teamPoint2nd = 0;
        }
        return teamHasBall;
    }
    return -1;
}

SDL_Texture * updateScoreTexture(char * text, SDL_Color color){
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

SDL_Texture *loadTexture(char *path, SDL_Renderer *renderer) {
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

        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
        {
            printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
            return NULL;
        }
        return window;
    }
}

void togglePause(float * lastTime) {
    timeScale = timeScale? 0:1;
}

int handleKeys() {
    for (int i = 0; i < numPlayer; ++i) {
        Player *player = players[i];
        move(player,
             currentKeyStates[player->left],
             currentKeyStates[player->up],
             currentKeyStates[player->right]);
    }
    if (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_Q]) return 1;
    else if (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_R]) return 2;
    else if (currentKeyStates[SDL_SCANCODE_P] && currentKeyStates[SDL_SCANCODE_LCTRL]) return 3;
    else return 0;
}

bool isElemInArray(int* arr, int elem, int length){
    for (int i = 0; i < length; i++){
        if (elem == arr[i]) return true;
    }
    return false;
}

int main(int argc, char *args[]) {

    //// START UP SDL AND CREATE WINDOW
    SDL_Window *window = init();
    if (window == NULL) return 1;

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
    sounds[numSound++] = Mix_LoadWAV("../sounds/item.wav");    // 7

    //// SETUP CONTROLLER
    currentKeyStates = SDL_GetKeyboardState(NULL);

    //// SETUP IMAGE RENDERER
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) return 2;

    //// SETUP FONT RENDERER
    if (TTF_Init() == -1) return 3;
    font = TTF_OpenFont(fontPath, 20);
    if (font == NULL) {
        printf("can't load font %s\n", SDL_GetError());
        return 3;
    }

    //// SETUP SCORE INDICATOR
    int teamPoint1st = 0, teamPoint2nd = 0;
    char scoreString[10];
    SDL_Texture * scoreTexture = NULL;
    SDL_Color scoreColor = { 0, 255, 0, 255 };
    getScoreString(scoreString, teamPoint1st, teamPoint2nd);
    scoreTexture = updateScoreTexture(scoreString, scoreColor);

    //// CREATE PLAYER
    SDL_Keycode keycode[] = {SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D,
                             SDL_SCANCODE_N, SDL_SCANCODE_B, SDL_SCANCODE_M,
                             SDL_SCANCODE_UP, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
                             SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_9};
    char imagePath[] = "../pics/player0.png";
    for (int l = 0; l < 4; l++) {
        imagePath[14] += 1;
        Object* object = createObject(l+1, 100, 100, imagePath);
        players[numPlayer] = createPlayer(object, 250, 675, keycode[l*3+0], keycode[l*3+1], keycode[l*3+2]);
        objects[numObjects++] = players[numPlayer++]->object;
    }

    //// CREATE BALL
    Object * object_b1 = createObject(5, 50, 50, "../pics/ball2.png");
    Ball * ball = createBall(object_b1);
    objects[numObjects++] = object_b1;

    //// CREATE WALL
    Object *object_w1 = createObject(6, 25, objects[0]->H * 3, "../pics/wall.png");
    Wall *wall = createWall(object_w1);
    objects[numObjects++] = wall->object;

    // validate players and set their positions
    int quit = 0;
    for (int j = 0; j < numPlayer; ++j){
        if (players[j] == NULL || players[j]->object == NULL) quit = 1;
    }
    srand((unsigned int) time(NULL));
    resetPositions(0);

    float lastTime = 0, currentTime;
    int rotation = 0;
    SDL_Rect background = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    // Play sound when start
    Mix_PlayChannel( -1, sounds[6], 0 );
     // printf("%i\n", sounds[6]);

    // game loop
    while (quit == 0){
        //// GET KEY INPUT
        do {
            int signal = handleKeys();
            switch (handleKeys()){
                case 1:
                    quit = 1;
                    break;
                case 2:
                    resetPositions(0);
                    break;
                case 3:
                    togglePause(&lastTime);
                    break;
                default:break;
            }
        }
        while (SDL_PollEvent(&event) != 0);
        if (event.type == SDL_QUIT) quit = 1;   // close on top right 'x'

        //// LOCK FPS
        currentTime = SDL_GetTicks();
        int actualTimePerFrame = (int) currentTime - (int) lastTime;
        if (actualTimePerFrame < TIME_PER_FRAME) {
            SDL_Delay((Uint32) ((int) TIME_PER_FRAME - actualTimePerFrame));
        }
        currentTime = SDL_GetTicks();

        //// UPDATE LOGIC POSITION
        for (int i = 0; i < numObjects; ++i) {
            updateXY(objects[i], (currentTime - lastTime) * timeScale);
        }

        //// CHECK COLLISION AND CORRECT THEIR POSITIONS
        int checkedObjIndexes[numObjects];
        int numCheckedObj = 0;
        for (int i = 0; i < numObjects; ++i) {
            for (int j = 0; j < numObjects; j++) {
                if (i == j || isElemInArray(checkedObjIndexes, j + 1, numCheckedObj)) {
                    continue;
                }
//                printf("Checking %i and %i\n", i,j);
                checkCollision(objects[i], objects[j]);
                checkedObjIndexes[numCheckedObj++] = i + 1;
            }
        }
        //printf("|%i| X%i Y%i  ", currentPlayer->pos, currentPlayer->X, players[i]->Y);

        //// UPDATE UI
        SDL_RenderClear(renderer);  //// clear the last screen
        for (int k = 0; k < numObjects; ++k) {
            Object *currentObj = objects[k];
            SDL_Rect position = {(int) ceil(currentObj->X), (int) ceil(currentObj->Y), currentObj->W, currentObj->H};
            if (currentObj->type == OBJECT_BALL){
                SDL_RenderCopyEx(renderer, currentObj->image, &background, &position, rotation++, 0, SDL_FLIP_NONE);
            } else {
                SDL_RenderCopyEx(renderer, currentObj->image, &background, &position, 0, 0, SDL_FLIP_NONE);
            }
        }
        //// UPDATE SCORE
        int teamHasBall = updateScore(ball, &teamPoint1st, &teamPoint2nd);
        if (teamHasBall > 0){
            resetPositions(teamHasBall);
//            timeScale = 0;
            SDL_Delay(1000);
            getScoreString(scoreString, teamPoint1st, teamPoint2nd);
            scoreTexture = updateScoreTexture(scoreString, scoreColor);
        };

        SDL_Rect position = {SCREEN_WIDTH /2 - 100, 0, 200, 50};
        SDL_RenderCopyEx(renderer, scoreTexture, &background, &position, 0, 0, SDL_FLIP_NONE);

        //// RENDER
        SDL_RenderPresent(renderer);

        lastTime = currentTime;
    }


    //// Free resources and close SDL
    printf("free objects ");
    for (int m = 0; m < numObjects; ++m) {
        Object *currentObj = objects[m];
        if(currentObj){
            free(currentObj->wrapper);
            SDL_DestroyTexture(currentObj->image);
            free(currentObj);
            printf("%i ", m);
        }
    }
    printf("\n");
    SDL_DestroyRenderer(renderer); renderer = NULL;
    SDL_DestroyWindow(window); window = NULL;
    TTF_CloseFont(font); font = NULL;

    // free / quite sound
    for (int i = 0; i < numSound; ++i)
    {
    Mix_FreeChunk(sounds[i]);sounds[i] = NULL;
    }

    Mix_FreeMusic(music); music=NULL;
    //Quit SDL subsystems
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
