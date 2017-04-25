cmake_minimum_required(VERSION 3.7)
project(game)

set(CMAKE_C_STANDARD 99)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${game_SOURCE_DIR}/findsdl2")

set(SOURCE_FILES main.c)
add_executable(game ${SOURCE_FILES} player.c player.h main.h)

find_package(SDL2 REQUIRED sdl2)
find_package(SDL2_image REQUIRED sdl2_image)
find_package(SDL2_mixer REQUIRED sdl2_mixer )

include_directories(${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_MIXER_INCLUDE_DIR})
target_link_libraries(game ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES})
