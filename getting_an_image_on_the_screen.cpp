#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstdio>
#include <SDL2_image/SDL_image.h>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();

SDL_Window *gWindow;
SDL_Surface *gScreenSurface;
SDL_Surface *gHelloWorld;

int main(int argc, const char * argv[])
{
    if(!init())
    {
        cout << "Error in init()" << endl; exit(1);
    }
    if(!loadMedia())
    {
        cout << "Error in loadMedia()" << endl; exit(1);
    }
    
    SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
    SDL_UpdateWindowSurface(gWindow);
    SDL_Delay(10000);
    close();
    return 0;
}

bool init()
{
    bool ret = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << SDL_GetError() << endl;
        ret = false;
    }
    else
    {
        gWindow = SDL_CreateWindow("Hello, SDL!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL)
        {
            cout << SDL_GetError() << endl;
            ret = false;
        }
        else
        {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }
    
    return ret;
}

bool loadMedia()
{
    bool ret = true;
    
    gHelloWorld = IMG_Load("arisa.jpg");
    if(gHelloWorld == NULL)
    {
        cout << SDL_GetError() << endl;
        ret = false;
    }
    return ret;
}

void close()
{
    SDL_FreeSurface(gHelloWorld);
    gHelloWorld = NULL;
    
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    
    SDL_Quit();
}
