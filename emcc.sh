#!/bin/sh

set -e

emcc *.c -o out/main.html -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s TOTAL_MEMORY=32MB --preload-file assets/ --use-preload-plugins
sudo rm -rf /var/www/html/volleyball
sudo cp -R out/ /var/www/html/volleyball
