#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <SDL2_image/SDL_image.h>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();
SDL_Surface* loadSurface(string path);

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
    
    bool quit = false;
    SDL_Event e;
    
    while(!quit)
    {
        SDL_UpdateWindowSurface(gWindow);
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                quit = true;
            }
            if(e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        cout << "UP" << endl;
                        break;
                    case SDLK_DOWN:
                        cout << "DOWN" << endl;
                        break;
                    case SDLK_RIGHT:
                        cout << "RIGHT" << endl;
                        break;
                    case SDLK_LEFT:
                        cout << "LEFT" << endl;
                        break;
                        
                    default:
                        break;
                }
            }
        }
    }
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
    
 //   gHelloWorld = IMG_Load("arisa.jpg");
    gHelloWorld = loadSurface("arisa.jpg");
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

SDL_Surface* loadSurface(string path)
{
    SDL_Surface* ret = NULL;
    
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL)
    {
        cout << SDL_GetError() << endl;
    }
    else
    {
        ret = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, NULL);
        if(ret == NULL)
        {
            cout << SDL_GetError() << endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    
    return ret;
}
