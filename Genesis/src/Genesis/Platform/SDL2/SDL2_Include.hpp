#pragma once

//Get rid of the SDL_main that messes up the build
#include <SDL2/SDL_main.h>
#undef main

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

#include <SDL2\SDL_GameController.h>
#include <SDL2\SDL_joystick.h>
