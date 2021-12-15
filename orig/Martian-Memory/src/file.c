void stop_music();
void init_menu()
{
    menuback=T_LoadT8(DATADIR "/gfx/menuback.T8");
    menu_font_a=T_LoadT8(DATADIR "/fonts/menu_a.T8");
    menu_font_i=T_LoadT8(DATADIR "/fonts/menu_i.T8");
    SDL_SetColorKey(menu_font_a, SDL_SRCCOLORKEY, SDL_MapRGB(menu_font_a->format,0,255,0));
    SDL_SetColorKey(menu_font_i, SDL_SRCCOLORKEY, SDL_MapRGB(menu_font_i->format,0,255,0));
    option_sound=Mix_LoadWAV(DATADIR "/sounds/option.wav");
}

void close_menu()
{
    SDL_FreeSurface(menu_font_a);
    SDL_FreeSurface(menu_font_i);
    SDL_FreeSurface(menuback);
    Mix_FreeChunk(option_sound);
}

void init_game()
{
    touch=Mix_LoadWAV(DATADIR "/sounds/touch.wav");
    match=Mix_LoadWAV(DATADIR "/sounds/match.wav");
    nomatch=Mix_LoadWAV(DATADIR "/sounds/nomatch.wav");
    combo_sound=Mix_LoadWAV(DATADIR "/sounds/combo.wav");
    music=Mix_LoadMUS(DATADIR "/music/ingame.s3m");
    Schips=T_LoadT8(DATADIR "/gfx/chips.T8");
    SDL_SetColorKey(Schips, SDL_SRCCOLORKEY, SDL_MapRGB(Schips->format,0,255,0));
    scorefont=T_LoadT8(DATADIR "/fonts/score.T8");
    bonusfont=T_LoadT8(DATADIR "/fonts/bonus.T8");
    SDL_SetColorKey(bonusfont, SDL_SRCCOLORKEY, SDL_MapRGB(bonusfont->format,0,255,0));

    Mix_Volume(1,48);
    Mix_Volume(2,48);
    Mix_Volume(3,48);
    Mix_Volume(4,128);
    gametime=0;
    score=0;
    combo=0;


}

void close_game()
{
    SDL_FreeSurface(Schips);
    SDL_FreeSurface(scorefont);
    SDL_FreeSurface(bonusfont);
    Mix_FreeChunk(touch);
    Mix_FreeChunk(match);
    Mix_FreeChunk(nomatch);
    Mix_FreeChunk(combo_sound);
    stop_music();

    Mix_Volume(-1,128);
}

void load_hiscoredata()
{
//    LoadT8(&background,DATADIR "/Tgfx/gameover.T8");
    scorefont2=T_LoadT8(DATADIR "/fonts/score2.T8");
    SDL_SetColorKey(scorefont2, SDL_SRCCOLORKEY,SDL_MapRGB(scorefont2->format,0,255,0));
    FILE *file = fopen(SCOREDIR "/data/scores.dat","rb");
    for(int a=0; a < 10; a++)
    {
	for(int b=0; b < 10; b++)
	{
	    scorename[a][b]=getc(file);
	}
	    int lobyte,hibyte,vhibyte;
	    hibyte=getc(file);
	    lobyte=getc(file);
	    scoretime[a]=(hibyte*256)+lobyte;
	    vhibyte=getc(file);
	    hibyte=getc(file);
	    lobyte=getc(file);
	    scorescore[a]=(vhibyte*65536)+(hibyte*256)+lobyte;
    }
    fclose(file);
}

void unload_hiscoredata()
{
    SDL_FreeSurface(scorefont2);
}

void save_hiscoredata()
{
    FILE *file = fopen(SCOREDIR "/data/scores.dat","rb+");

    int a, b;
    for(a=0;a<10;a++)
    {    
        for(b=0;b<10;b++)
	{
	    putc(scorename[a][b],file);
	}

        int lobyte,hibyte,vhibyte;

        hibyte=int(scoretime[a]/256);
        lobyte=int(scoretime[a]-(hibyte+256));
        putc(hibyte,file);
        putc(lobyte,file);

        vhibyte=int(scorescore[a]/65536);
        hibyte=int((scorescore[a] - (scorescore[a]/65536))/256);
        lobyte=int(scorescore[a] - (vhibyte*65536 + hibyte*256));

	putc(vhibyte,file);
	putc(hibyte,file);
	putc(lobyte,file);
    }
    fclose(file);
}
