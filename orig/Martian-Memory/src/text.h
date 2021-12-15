void T_Print(SDL_Surface *font, int x, int y, char *str, ...)  //Rutina para imprimir texto estatico en la pantalla
{
    int font_h=font->h; int font_w=font->w/91; // 91 es la cantidad de caracteres en el archivo de font
    SDL_Rect srctxt; // la posicion donde se encuentra el caracter en el bitmap
    SDL_Rect dsttxt; // la posicion donde se imprimira el texto

    char texto [100];
    va_list ap;
    va_start(ap, str);
	vsprintf(texto, str, ap);
    va_end(ap);
    
    srctxt.w = font_w;
    srctxt.h = font_h;
    srctxt.y = 0;
    int linecounter = 0 ; // este contador se utiliza para saber en que linea imprimimos el texto
    int charpos = 0;
    for(int charcounter = 0; charcounter <= (strlen(texto));charcounter++)
    {
	int curchar=texto[charcounter];
	if(curchar == 94)
	{
	    linecounter++;
	    charpos = -1;
	}
	srctxt.x = (curchar - 32) * font_w;
	dsttxt.x = (x + (charpos * font_w));
	dsttxt.y = (y + (linecounter * font_h));
	charpos++;
	SDL_BlitSurface(font, &srctxt, screen, &dsttxt);
    }
}

