#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <SDL2_image/SDL_image.h>
#include <unistd.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <sstream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
//const int WALKING_ANIMATION_FRAMES = 4;
//const int BUTTON_WIDTH = 300;
//const int BUTTON_HEIGHT = 200;
//const int TOTAL_BUTTONS = 4;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

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
//SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
TTF_Font *gFont;
Mix_Music *gMusic;
Mix_Chunk *gScratch;
Mix_Chunk *gHigh;
Mix_Chunk *gMedium;
Mix_Chunk *gLow;

class LTexture
{
public:
    LTexture();
    
    ~LTexture();
    
    bool loadFromFile(string path);
#ifdef _SDL_TTF_H
    bool loadFromRenderedText(string textureText, SDL_Color textColor);
#endif
    void free();
    void setColor(Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode(SDL_BlendMode blending);
    void setAlpha(Uint8 alpha);
    void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip = SDL_FLIP_NONE);
    
    int getWidth();
    int getHeight();
private:
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
};

class LTimer
{
public:
    LTimer();
    
    void start();
    void stop();
    void pause();
    void unpause();
    
    Uint32 getTicks();
    
    bool isStarted();
    bool isPause();
private:
    Uint32 mStartTicks;
    Uint32 mPausedTicks;
    bool mPaused;
    bool mStarted;
};

LTexture gFPSTextTexture;

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

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(string textureText, SDL_Color textColor)
{
    free();
    
    SDL_Surface *textSurface = TTF_RenderUTF8_Solid(gFont, textureText.c_str(), textColor);
    if(textSurface == NULL)
    {
        cout << TTF_GetError() << endl;
    }
    else
    {
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(mTexture == NULL)
        {
            cout << SDL_GetError() << endl;
        }
        else
        {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        SDL_FreeSurface(textSurface);
    }
    
    
    return mTexture != NULL;
}
#endif

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

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    if(clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
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

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

LTimer::LTimer()
{
    mStartTicks = 0;
    mPausedTicks = 0;
    
    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    mStarted = true;
    mPaused = false;
    
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void LTimer::stop()
{
    mStarted = false;
    mPaused = false;
    mStarted = 0;
    mPausedTicks = 0;
}

void LTimer::pause()
{
    if(mStarted && !mPaused)
    {
        mPaused = true;
        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    if(mStarted && mPaused)
    {
        mPaused = false;
        mStartTicks = SDL_GetTicks() - mPausedTicks;
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
    Uint32 time = 0;
    if(mStarted)
    {
        if(mPaused)
        {
            time = mPausedTicks;
        }
        else
        {
            time = SDL_GetTicks() - mStartTicks;
        }
    }
    return time;
}

bool LTimer::isStarted()
{
    return mStarted;
}

bool LTimer::isPause()
{
    return mPaused && mStarted;
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
    SDL_Color textColor = {0, 0, 0, 255};
  //  Uint32 startTime = 0;
    stringstream timeText;
    LTimer fpsTimer;
    LTimer capTimer;
    int countedFrames = 0;
    fpsTimer.start();
    
    while(!quit)
    {
        capTimer.start();
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        
        float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);

        if(avgFPS > 2000000)
        {
            avgFPS = 0;
        }
        timeText.str("");
        timeText << "Average Frames Per Second " << avgFPS;
        
        if(!gFPSTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor))
        {
            cout << "Unable to render FPS texture!" << endl;
        }

        
        SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(gRenderer);
        
        gFPSTextTexture.render((SCREEN_WIDTH - gFPSTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gFPSTextTexture.getHeight()) / 2);

        SDL_RenderPresent(gRenderer);
        ++countedFrames;
        
        int frameTicks = capTimer.getTicks();
        if(frameTicks < SCREEN_TICKS_PER_FRAME)
        {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }
    }
    close();
    return 0;
}

bool init()
{
    bool ret = true;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
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
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
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
                
                if(TTF_Init() == -1)
                {
                    cout << TTF_GetError() << endl;
                    ret = false;
                }
                
                if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    cout << Mix_GetError() << endl;
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
    
    gFont = TTF_OpenFont("unifont.ttf", 28);
    if(gFont == NULL)
    {
        cout << TTF_GetError() << endl;
        ret = false;
    }
    return ret;
}

void close()
{
    
    gFPSTextTexture.free();
    
    Mix_FreeChunk(gScratch); gScratch = NULL;
    Mix_FreeChunk(gHigh); gHigh = NULL;
    Mix_FreeChunk(gMedium); gMedium = NULL;
    Mix_FreeChunk(gLow); gLow = NULL;
    
    Mix_FreeMusic(gMusic); gMusic = NULL;
    
    TTF_CloseFont(gFont);
    gFont = NULL;
    
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    
    Mix_Quit();
    TTF_Quit();
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



