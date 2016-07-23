#include<stdio.h>
#include<stdarg.h>
#include<string.h>
#include<stdlib.h>
#include<SDL/SDL.h>
#include<math.h>
#include<time.h>

#include"initSDL.h"
#include"define.h"
#include"timing.c"
#include"gfx.h"
#include"text.h"
#include"file.c"
#include"music.c"
#include"input.c"
#include"game.c"

#include"menu.h"
#include"menu.c"


int main (int argc, char * argv[])
{
    initSDL();

    if(argc > 1)
	if(strcmp(argv[1], "--fullscreen")==0 || strcmp(argv[1], "-f")==0)
	    screen_mode();

    int exit_game=0; // cuando se pone a 1 sale del juego
    while(exit_game!=Kexit)
	exit_game=do_menu();

    SDL_Quit();
    fprintf(stderr,"\nThanks for Playing\n");
    return 0;
}
