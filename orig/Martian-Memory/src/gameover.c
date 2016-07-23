void hiscore_events()
{
    int n;
    SDL_Event event;
    
    while(SDL_PollEvent(&event))
    {
        if(event.type==SDL_KEYDOWN)
        {
	    if(event.key.keysym.sym>41)
	    {
	        playername[namechar] = event.key.keysym.sym;
	        if(playername[namechar]>=97 && playername[namechar]<=122)
	    	    playername[namechar]=event.key.keysym.sym-32;
		if(namechar < 9)
		    namechar++;
	    }
	    else
	    {
	        if(event.key.keysym.sym==SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE)
	    	    inputloop=0;
		if(event.key.keysym.sym==SDLK_BACKSPACE)
		{
		    namechar=0;
		    for(n=0;n<10;n++)
		        playername[n] = '.';
		}
	    }
	}
    }
}

void show_hiscores()
{
    SDL_SetColorKey(scorefont2,SDL_SRCCOLORKEY,SDL_MapRGB(scorefont2->format,0,255,0));
    char plyername[]="          ";
    T_Print(scorefont2,20,25,"   MARTIAN MEMORY     TOP TEN    ");
    T_Print(scorefont2,20,55,"NAME           TIME        SCORE ");
    T_Print(scorefont2,20,75,"---------------------------------");

    T_Print(scorefont2,20,400,"---------------------------------");
    T_Print(scorefont2,20,425,"    PRESS RIGHT MOUSE BUTTON     ");
    T_Print(scorefont2,20,450,"            TO RETURN            ");

    for(int a=0; a < 10;a++)
    {
	strncpy(playername,scorename[a],10);
	T_Print(scorefont2,5,100+(25*a),"%s",playername);
	int mins=int(scoretime[a]/60);
	int secs=int(scoretime[a]-(mins*60));
	if(secs>9) T_Print(scorefont2,290,100+(25*a),"%d:%d",mins,secs);
	else T_Print(scorefont2,290,100+(25*a),"%d:0%d",mins,secs);
	T_Print(scorefont2,510,100+(25*a),"%d",scorescore[a]);
    }
    SDL_Flip(screen);
}

void do_hiscores()
{
    flush_events();
    load_hiscoredata();
    //--- creamos el fondo de pantalla
    int n;
    float R, G, B;
    R=50; G=130; B=180;
    SDL_Rect back;
	back.h=1; back.w=screen_w; back.x=0;
    for(n=0; n<screen_h;n++)
    {
	back.y=n;
	SDL_FillRect(screen, &back, SDL_MapRGB(screen->format,int(R), int(G), int(B)));
	R-=0.33; G-=0.25; B-=0.50;
	if(R<0) R=0; if(G<0) G=0; if(B<0) B=0;
    }
    
    show_hiscores();	
    wait_mouse();
    unload_hiscoredata();
}

void do_gameover()
{
    load_hiscoredata();
    char nameplayer[]="          ";
    strncpy(playername,nameplayer,10);
    namechar=0;
    int highscore=0;
    for(int a=9; a>=0;a--)
    {
	if(score>scorescore[a])
	{
	    highscore=1;
	    scorepos=a;
	}
	if(score==scorescore[a]) // comprobamos que tiempo fue mejor
	    if(gametime<scoretime[a]) 
	    {
		highscore=1;
		scorepos=a;
	    }
    }
    if(highscore==1) // si se hizo una buena puntuacion
    {
	for(int b=8;b>=scorepos;b--)
	{
	    strncpy(scorename[b+1],scorename[b],10);
	    scorescore[b+1]=scorescore[b];
	    scoretime [b+1]=scoretime [b];
	}
	inputloop=1;
	SDL_Rect box;
	box.x=175;box.y=220;box.w=290;box.h=52;
	SDL_FillRect(screen,&box,SDL_MapRGB(screen->format,255,255,255));
	box.x+=3;box.y+=3;box.w-=6;box.h-=6;
	while(inputloop==1)
	{
	    hiscore_events();
	    SDL_FillRect(screen,&box,SDL_MapRGB(screen->format,0,0,0));
	    T_Print(scorefont2,box.x+10,box.y+5,"ENTER YOUR NAME");
	    T_Print(scorefont2,box.x+51+(namechar*16),box.y+29,".");
	    T_Print(scorefont2,box.x+51,box.y+24,"%s",playername);
	    SDL_Flip(screen);
	}
	strncpy(scorename[scorepos],playername,10);
	scorescore[scorepos]=score;
	scoretime[scorepos]=int(gametime);
	save_hiscoredata();
    }
    unload_hiscoredata();
}

void game_over()
{
    do_gameover();
    do_hiscores();
}
