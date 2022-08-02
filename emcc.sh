#!/bin/sh

set -e

emcc -c main.c -o main.o -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]'
emcc main.o -o main.html -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' --preload-file pics/ --preload-file sounds/ --use-preload-plugins
sudo cp main.* /var/www/html/