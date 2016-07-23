int main_menu()
{
    int menu_x=166, menu_y=200;
    int optionsc=4;
    char * optionsv[]={
    "    PLAY   ",
    "SCREEN MODE",
    "   SCORES  ",
    "    EXIT   "};
    int menuspacing=35;
    int option=1;
    option=menu(menu_x,menu_y, menu_font_a, menu_font_i , menuspacing, optionsc, optionsv, option);
    return(option);
}

int do_menu()
{
    int option=0;
    while(option!=Kexit)
    {
	init_menu();
	SDL_BlitSurface(menuback, NULL, screen, NULL);
	SDL_Flip(screen);
	option=main_menu();
	flush_events();
	if(option!=Kexit)
	{
	    if(option==1) option=play();
	    if(option==2) screen_mode();
	    if(option==3) do_hiscores();
	    if(option==4) option=Kexit;
	}
	close_menu();
    }
    return(option);
}
