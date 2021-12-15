#include<SDL/SDL.h>
#include<SDL/SDL_mixer.h>
SDL_Surface *screen;

#define screen_w 640
#define screen_h 480
#define screen_bpp 16

void initSDL()
{
    if((SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0))
    {
        fprintf(stderr,"Could not Initialize SDL.\nError: %s\n", SDL_GetError());
        exit (-1);
    }
    SDL_Surface *icon;
    icon=SDL_LoadBMP(DATADIR "/gfx/icon.bmp");
    SDL_WM_SetIcon(icon, NULL);
    screen = SDL_SetVideoMode(screen_w,screen_h,screen_bpp, SDL_DOUBLEBUF|SDL_HWSURFACE);
    if (screen == NULL)
    {
        fprintf(stderr, "Can't set the video mode. Quitting.\nError; %s\n", SDL_GetError());
        exit (1);
    }
    if(Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0)
    {
        fprintf(stderr,"Warning: Couldn't set 44100 Hz 16-bit audio\n: %s\n", SDL_GetError());
    }



    SDL_ShowCursor(1);
    SDL_WM_SetCaption("Martian memory", "Martian Memory");
    fprintf(stderr,"System init Ok.\n");
    fprintf(stderr,"   Entering game...\n");
}
