#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <SDL2_image/SDL_image.h>
#include <unistd.h>
#include <SDL2_ttf/SDL_ttf.h>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int WALKING_ANIMATION_FRAMES = 4;
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

enum LButtonSprite
{
    BUTTON_SPRITE_MOUSE_OUT = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_DOWN = 2,
    BUTTON_SPRITE_MOUSE_UP = 3,
    BUTTON_SPRITE_TOTAL = 4
};

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
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
TTF_Font *gFont;

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

class LButton
{
public:
    LButton();
    void setPosition(int x, int y);
    void handleEvent(SDL_Event *e);
    void render();
private:
    SDL_Point mPosition;
    LButtonSprite mCurrentSprite;
};

LTexture gButtonSpriteSheetTexture;
LButton gButtons[TOTAL_BUTTONS];

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

LButton::LButton()
{
    mPosition.x = 0;
    mPosition.y = 0;
    
    mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y)
{
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent(SDL_Event *e)
{
    if(e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        
        bool inside = true;
        
        if(x < mPosition.x)
        {
            inside = false;
        }
        else if(y < mPosition.y)
        {
            inside = false;
        }
        else if(y > mPosition.y + BUTTON_HEIGHT)
        {
            inside = false;
        }
        if(!inside)
        {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        else
        {
            switch(e->type)
            {
                case SDL_MOUSEMOTION:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
                    break;
            }
        }
    }
}

void LButton::render()
{
    gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
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
    
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                quit = true;
            }
            
            for(int i = 0; i < TOTAL_BUTTONS; ++i)
            {
                gButtons[i].handleEvent(&e);
            }
            
        }
        SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(gRenderer);
        
        for(int i = 0; i < TOTAL_BUTTONS; ++i)
        {
            gButtons[i].render();
        }
        
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
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
            }
        }
    }
    
    return ret;
}

bool loadMedia()
{
    bool ret = true;
    if(!gButtonSpriteSheetTexture.loadFromFile("/button.png"))
    {
        cout << "Error!" << endl;
        ret = false;
    }
    else
    {
        for(int i = 0; i < BUTTON_SPRITE_TOTAL; ++i)
        {
            gSpriteClips[i].x = 0;
            gSpriteClips[i].y = i * 200;
            gSpriteClips[i].w = BUTTON_WIDTH;
            gSpriteClips[i].h = BUTTON_HEIGHT;
        }
        
        gButtons[0].setPosition(0, 0);
        gButtons[1].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, 0);
        gButtons[2].setPosition(0, SCREEN_HEIGHT - BUTTON_HEIGHT);
        gButtons[3].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT);
    }

    
    return ret;
}

void close()
{
    
    TTF_CloseFont(gFont);
    gFont = NULL;
    
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    
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
