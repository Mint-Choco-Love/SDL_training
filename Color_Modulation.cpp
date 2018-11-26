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
SDL_Texture* loadTexture(string path);

SDL_Window *gWindow;
SDL_Surface *gScreenSurface;
SDL_Surface *gHelloWorld;
SDL_Renderer *gRenderer;
SDL_Texture *gTexture;
SDL_Rect gSpriteClips[4];

class LTexture
{
public:
    LTexture();
    
    ~LTexture();
    
    bool loadFromFile(string path);
    void free();
    void setColor(Uint8 red, Uint8 green, Uint8 blue);
    void render(int x, int y, SDL_Rect *clip = NULL);
    
    int getWidth();
    int getHeight();
private:
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
};

LTexture gModulatedTexture;

LTexture::LTexture()
{
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    free();
}

bool LTexture::loadFromFile(string path)
{
    free();
    SDL_Texture *newTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL)
    {
        cout << SDL_GetError() << endl;
    }
    else
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xff, 0xff));
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(newTexture == NULL)
        {
            cout << SDL_GetError() << endl;
        }
        else
        {
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        SDL_FreeSurface(loadedSurface);
    }
    mTexture = newTexture;
    return mTexture != NULL;
}

void LTexture::free()
{
    if(mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y, SDL_Rect* clip)
{
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    if(clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

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
    
    bool quit = false;
    SDL_Event e;
    
    Uint8 r = 255;
    Uint8 g = 255;
    Uint8 b = 255;
    
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                quit = true;
            }
            if(e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_q:
                        r += 32;
                        break;
                    case SDLK_w:
                        g += 32;
                        break;
                    case SDLK_e:
                        b += 32;
                        break;
                    case SDLK_a:
                        r -= 32;
                        break;
                    case SDLK_s:
                        g -= 32;
                        break;
                    case SDLK_d:
                        b -= 32;
                        break;
                        
                    default:
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(gRenderer);
        
        gModulatedTexture.setColor(r, g, b);
        gModulatedTexture.render(0, 0);
        
        SDL_RenderPresent(gRenderer);
        
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
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL)
            {
                cout << SDL_GetError() << endl;
                ret = false;
            }
            else
            {
                SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
                
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags))
                {
                    cout << SDL_GetError() << endl;
                    ret = false;
                }
            }
        }
    }
    
    return ret;
}

bool loadMedia()
{
    bool ret = true;
    
    if(!gModulatedTexture.loadFromFile("arisa.jpg"))
    {
        cout << "Error!" << endl;
        ret = false;
    }
    
    return ret;
}

void close()
{
    
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    
    IMG_Quit();
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

SDL_Texture* loadTexture(string path)
{
    SDL_Texture* ret = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL)
    {
        cout << SDL_GetError() << endl;
    }
    else
    {
        ret = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(ret == NULL)
        {
            cout << SDL_GetError() << endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    
    
    return ret;
}
