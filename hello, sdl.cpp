//
//  main.cpp
//  SDL
//
//  Created by 김만기 on 25/11/2018.
//  Copyright © 2018 김만기. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, const char * argv[]) {
    SDL_Window *window = NULL;
    SDL_Surface *screenSurface = NULL;
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0) { exit(1); }
    
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL) { exit(1); }
    
    screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xff, 0xff, 0xff));
    
    SDL_UpdateWindowSurface(window);
    SDL_Delay(10000);
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    
    return 0;
}
