/*
    Funcion para realizar menues para una aplicacion
	Parametros:
    ---------------------------------------------------------------------
    |  x            | posicion x del comienzo del menu en la pantalla   |
    |  y            | posicion y del comienzo del menu en la pantalla   |
    |  inactivefont | font a utilizar para las opciones no selecionadas |
    |  activefont   | font a utilizar para la opcion selecionada        |
    |  spacing      | separacion entre una opcion y la siguiente        |
    |  optionsc     | cantidad de opciones en el menu                   |
    |  options      | el texto de cada opcion                           |
    |  pos          | opcion activa en el menu al inicio                |
    ---------------------------------------------------------------------
*/

SDL_Surface *back;

int menu(int x, int y, SDL_Surface * inactivefont, SDL_Surface * activefont, int spacing, int optionc, char *option[], int pos)
{
    SDL_Surface * font;
    SDL_Event event;

    while(true)
    {
    // --- chequeamos el mouse ATENCION!!! esto es especifico de cada menu NO ES ESTANDAR PARA TODOS LOS PROYECTOS
	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	int oldpos=pos;
	if(mouse_x>=280 && mouse_x<=389 && mouse_y>=200 && mouse_y<=227)
	    pos=1;
	if(mouse_x>=168 && mouse_x<=473 && mouse_y>=235 && mouse_y<=262)
	    pos=2;
	if(mouse_x>=252 && mouse_x<=417 && mouse_y>=270 && mouse_y<=297)
	    pos=3;
	if(mouse_x>=280 && mouse_x<=389 && mouse_y>=305 && mouse_y<=332)
	    pos=4;

	if(pos!=oldpos) Mix_PlayChannel(1, option_sound,0);

	int n;
	for(n=0;n<optionc;n++)
	{
	    if(n+1==pos)
		font=inactivefont;
	    else
		font=activefont;

	    int fw=font->w/91;
	    int fh=font->h;

	    T_Print(font, x, (y+(n*spacing)), "%s",option[n]);
	    SDL_UpdateRect(screen,x, (y+(n*spacing)), (fw*(strlen(option[n]))), fh);
	    SDL_Delay(5);
	    while(SDL_PollEvent(&event))
		if(event.type==SDL_QUIT) return(4);
		if(event.type==SDL_MOUSEBUTTONDOWN && event.button.button==SDL_BUTTON_LEFT)
		{
		    Mix_PlayChannel(1,option_sound,0);
		    return(pos);
		}
	}
    }
}

