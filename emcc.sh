#!/bin/sh

set -e

emcc *.c -o out/main.html -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' --preload-file assets/ --use-preload-plugins
sudo cp main.* /var/www/html/
