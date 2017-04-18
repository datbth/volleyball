/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


SDL_Window *init() {   //create window
    SDL_Window *window;
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("can't initialize SDL%s\n", SDL_GetError());
        return false;
    } else {
        //Create window
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("can't initialize Window%s\n", SDL_GetError());
            return NULL;
        }
        //Initialize SDL_mixer
        if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048 ) < 0 )
        {
            printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
            return NULL;
        }
        return window;
    }
}

SDL_Texture *loadTexture(char *path, SDL_Renderer* renderer) {
    SDL_Surface *loadedSurface = IMG_Load(path);    //  load PNG image
    SDL_Texture * texture = NULL;

    if (loadedSurface == NULL) {
        printf("can't load %s %s\n", path, IMG_GetError());
        return NULL;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (texture == NULL) {
            printf("can't create Texture %s %s\n", path, SDL_GetError());
            return NULL;
        } else {
            SDL_FreeSurface(loadedSurface);
            return texture;
        }
    }
}

//int createJoystick(SDL_Joystick){
//    if(SDL_NumJoysticks() < 1) return 1;
//
//}

int main(int argc, char *args[]) {
    //The window we'll be rendering to
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_Event event;

    //Start up SDL and create window
    SDL_Window *window = init();
    if (window == NULL) return 1;
    
    // setup controller 
    const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );

    // setup audio
    Mix_Music * music = Mix_LoadMUS("audios/beat.wav"); if (music == NULL) printf("%s\n", Mix_GetError());
    Mix_Chunk * left = Mix_LoadWAV("audios/left.wav"); if (left == NULL) printf("%s\n", Mix_GetError());
    Mix_Chunk * right = Mix_LoadWAV("audios/right.wav"); if (right == NULL) printf("%s\n", Mix_GetError());

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) return 2;

    texture = loadTexture("pics/hacker_icon.png", renderer);
    if (texture == NULL) return 3;

    SDL_Rect fillRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    // get texture size
    int textureW, textureH;
    SDL_QueryTexture(texture, 0, 0, &textureW, &textureH);
    SDL_Rect texture_4parts[] = {
            {0, 0, textureW/2, textureH/2},
            {0, 128, textureW/2, textureH/2},
            {128, 128, textureW/2, textureH/2},
            {128, 0, textureW/2, textureH/2}
    };

    //Apply the PNG image and update window
    for (int i = 0; i < 361; i++){
        SDL_RenderClear(renderer);
        SDL_RenderCopyEx(renderer, texture, &texture_4parts[0], &texture_4parts[0], i, 0, 0);
        SDL_RenderCopyEx(renderer, texture, &texture_4parts[1], &texture_4parts[1], i, 0, 0);
        SDL_RenderCopyEx(renderer, texture, &texture_4parts[2], &texture_4parts[2], i, 0, 0);
        SDL_RenderCopyEx(renderer, texture, &texture_4parts[3], &texture_4parts[3], i, 0, 0);
        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    Mix_PlayMusic(music, -1);
    int quit = 0, total_degree = 0;
    while (quit == 0){
        while( SDL_PollEvent( &event ) != 0 )
            if (event.type == SDL_KEYDOWN){
                if(currentKeyStates[SDL_SCANCODE_LEFT]) {
                    total_degree -= 1;
                    Mix_PlayChannel(-1, left, 0);
                }
                else if(currentKeyStates[SDL_SCANCODE_RIGHT]) {
                    total_degree += 1;
                    Mix_PlayChannel(-1, right, 0);
                }
                if (currentKeyStates[SDL_SCANCODE_LCTRL] && currentKeyStates[SDL_SCANCODE_Q]) {
                    quit = 1;
                    Mix_HaltMusic();
                }

                //update
                SDL_RenderClear(renderer);
                SDL_RenderCopyEx(renderer, texture, &fillRect, &fillRect, total_degree, 0, 0);
                SDL_RenderPresent(renderer);
            }
    }

    // free music
    Mix_FreeChunk(left);left = NULL;
    Mix_FreeMusic(music);music = NULL;

    //Free resources and close SDL ***
    SDL_DestroyRenderer(renderer); renderer = NULL;
    SDL_DestroyWindow(window);window = NULL;

    //Quit SDL subsystems
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

return 0;
}
