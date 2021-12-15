#include"fx.c"
#include"gameover.c"

int play_loop(int type);
void shuffle()
{
    srand(time(NULL));
    int done;
    int x,y, n;
    for(y=0; y<chips_y; y++)
	for(x=0; x<chips_x; x++)
	{
	    mchip[x][y].type=0;
	    mchip[x][y].side=0; // ponemos todas la chips boca abajo
	    mchip[x][y].state=0; // el estado de todas las chips es "sin descubrir"
	}

    int loop=0;
    while(loop<2)
    {
	for(n=1; n<=(chips/2);n++)
	{
	    done=0;
	    while(done==0)
	    {
		x=rand()%chips_x;
		y=rand()%chips_y;
		if(mchip[x][y].type==0)
		{
		    mchip[x][y].type=n;
		    done=1;
		}
	    }
	}
	loop++;
    }
    turn=0;
}

void set_screen()
{
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));
}

void print_panel()
{
    int minutes=int(gametime/60);
    int seconds=int(gametime-minutes*60);
    if(seconds<10)
	T_Print(scorefont, 10, floor+32, "TIME: %d:0%d", minutes, seconds);
    else
	T_Print(scorefont, 10, floor+32, "TIME: %d:%d", minutes, seconds);
    T_Print(scorefont, 300, floor+32, "SCORE: %d", score);
}

void display_combo()
{
    showtime=0;
    char * bonus;
    int offset=0;
    if(combo==2)   bonus="  NICE   ";
    if(combo==3) { bonus="  GREAT  "; offset=-(bonusfont->w/91)/2;}
    if(combo==4) { bonus="EXCELLENT"; offset=-(bonusfont->w/91)/2;}
    if(combo>=5)   bonus="TERRIFIC ";
    Mix_PlayChannel(4,combo_sound,0);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,255,255,255));
    SDL_Flip(screen);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));
    #define combo_x 208
    #define combo_y 200
    #define combo_w 320
    #define combo_h 80
    init_combo_fx(combo_x, combo_y, combo_w, combo_h);
    while(showtime<3)
    {
        showtime+=dt;
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));
        play_loop(2); // el 2 indica que no chequee eventos, no actualice la pantalla y no incremente el tiempo
	combo_fx();
	T_Print(bonusfont, combo_x+offset,combo_y, "%s", bonus);
	T_Print(bonusfont, combo_x,combo_y+40,"%dX Combo", combo);
	SDL_Flip(screen);
    }
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));
    flush_events();
}

void print_screen(SDL_Surface *surface)
{
    print_panel();
    int x, y;
    SDL_Rect src, dst;
	src.x=0; src.w=chip_w; src.h=chip_h;
    for(y=0; y<chips_y; y++)
	for(x=0; x<chips_x; x++)
	{
	    dst.x=chips_start_x+(x*(chip_w+chips_spacing_x));
	    dst.y=chips_start_y+(y*(chip_h+chips_spacing_y));

	    if(mchip[x][y].state==1) mchip[x][y].side=1;
	    if(mchip[x][y].side==0)
	    src.y=0;
	    else
		src.y=mchip[x][y].type*chip_h;
	    SDL_BlitSurface(Schips, &src, surface, &dst);
	}
}

void anim()
{
    draw_stars();
    draw_bars();
}

int chk_complete()
{
    int complete=1;
    int x,y;
    for(y=0; y<chips_y; y++)
	for(x=0; x<chips_x; x++)
	    if(mchip[x][y].state==0)
		complete=0;
    return(complete);
}

void chk_mouse()
{
    int x, y; //posicion del mouse
    int xx, yy;
    SDL_GetMouseState(&x, &y);
    x-=chips_start_x; y-=chips_start_y;
    for(yy=0; yy<chips_y; yy++)
	for(xx=0; xx<chips_x; xx++)
	{
	    if(x>=(xx*(chip_w+chips_spacing_x)) && x<=(xx*(chip_w+chips_spacing_x))+chip_w)
	    {
		if(y>=(yy*(chip_h+chips_spacing_y)) && y<=(yy*(chip_h+chips_spacing_y))+chip_h)
		{
		    if(mchip[xx][yy].side==0)
		    {
			mchip[xx][yy].side=1;
			Mix_PlayChannel(1,touch,0);
			switch(turn)
			{
			    case 0:
				match_x=xx;
				match_y=yy;
				turn=1;
				break;
			    case 1: // si es la segunda fase del turno nos fijamos si la chip recogida concuerda con la anterior
				mchip[xx][yy].side=1;
				if(mchip[xx][yy].type==mchip[match_x][match_y].type)
				{
// --- MATCH
				    mchip[xx][yy].state=1;
				    mchip[match_x][match_y].state=1;
				    score+=10;
				    Mix_PlayChannel(2,match,0);
				    turn=0;
				    score+=combo*30;
				    combo++;
				    if(combo>1) display_combo();
				}
				else
				{
// --- NO MATCH
				    Mix_PlayChannel(2,nomatch,0);
				    combo=0;
				    turn=2;
				}
				break;
			}
		    }
		}
	    }
	    if(turn==2)
	    {
		showtime=0;
		while(showtime<1)
		{
		    showtime+=dt;
		    play_loop(1); // el 1 indica que no chequee eventos
		    SDL_Flip(screen);
		}
		flush_events();
		turn=0;
		mchip[xx][yy].side=0;
		mchip[match_x][match_y].side=0;
	    }
	}
}

int game_events(void)
{
    int action;
    action=key_events();
    return(action);
}

int play_loop(int type)
{
    int action;
    int wingame=0;
    while(action!=Kexit && wingame==0 && action!=Kescape)
    {
	timing();
	if(type!=2) gametime+=dt;
	if(type==0) action=game_events();
	play_music();
        anim();
        print_screen(screen);
	if(type==0) SDL_Flip(screen);
	SDL_Delay(5);
	wingame=chk_complete(); // nos fijamos si se completo el puzzle
	if(type!=0) break;
    }
    return(action);
}

void init_fx()
{
    init_stars();
    init_bars();
}

int play()
{
    int action;
    srand(time(NULL));
    init_game();
    flush_events();
    shuffle();
    init_fx();

    set_screen();
    action=play_loop(0); // el 0 es la forma habitual de llamar a esta rutina, si fuera 1 no chequearie eventos
    if(action!=Kexit) game_over();
    close_game();
    return(action);
}

void screen_mode()
{
    if(fullscreen)
    {
        screen = SDL_SetVideoMode(screen_w,screen_h,screen_bpp,0);
        fullscreen = 0;
    }
    else
    {
        screen = SDL_SetVideoMode(screen_w,screen_h,screen_bpp,SDL_FULLSCREEN);
        fullscreen = 1;
    }
    SDL_BlitSurface(menuback, NULL, screen, NULL);
}
