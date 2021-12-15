#define Kleft   1
#define Kright  2
#define Kup     3
#define Kdown   4
#define Kaction 5
#define Ktab    6
#define Kload   7
#define Ksave   8
#define Kprev   9
#define Knext   10

#define Kescape 11
#define Kexit   99

int keyaction=0;

#define left 1
#define right 2

void chk_mouse();

int key_events()
{
    int keyaction;
    SDL_Event event;
    Uint8 *keys;
    while(SDL_PollEvent(&event)==1)
    {
	if(event.type==SDL_MOUSEBUTTONDOWN) chk_mouse();
	if(event.type==SDL_QUIT) keyaction=Kexit;
	if(event.type==SDL_KEYDOWN)
	{
	    switch(event.key.keysym.sym)
	    {
		case SDLK_F10:
		    keyaction=Kexit;
		    break;
		case SDLK_DOWN:
		    keyaction=Kdown;
		    break;
		case SDLK_UP:
		    keyaction=Kup;
		    break;
		case SDLK_LEFT:
		    keyaction=Kleft;
		    break;
		case SDLK_RIGHT:
		    keyaction=Kright;
		    break;
		case SDLK_SPACE:
		    keyaction=Kaction;
		    break;
		case SDLK_ESCAPE:
		    keyaction=Kescape;
		    break;
	    }
	}
    }
    return(keyaction);
}

int menu_events()
{
    SDL_Event event;
    Uint8 *keys;
    keyaction=0;
    while(SDL_PollEvent(&event))
    {
	if(event.type==SDL_KEYDOWN)
	{
	    keys=SDL_GetKeyState(NULL);
	    if(event.type==SDL_QUIT) keyaction=Kexit;
	    if(keys[SDLK_F10]) keyaction=Kexit;
	    if(keys[SDLK_DOWN]) keyaction=Kdown;
	    if(keys[SDLK_UP]) keyaction=Kup;
	    if(keys[SDLK_LEFT]) keyaction=Kleft;
	    if(keys[SDLK_RIGHT]) keyaction=Kright;
	    if(keys[SDLK_SPACE]) keyaction=Kaction;
	}
    }
    return(keyaction);
}

void flush_events()
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) {}
}

void wait_action()
{
    int action=0;
    while(action!=Kescape && action!=Kaction)
    {
	action=key_events();
	SDL_Delay(5);
	play_music();
    }
}

void wait_mouse()
{
    SDL_Event event;
    while(true)
    {
	SDL_PollEvent(&event);
	if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_RIGHT) break;
	SDL_Delay(5);
	play_music();
    }
}
