void play_music()
{
	if(!Mix_PlayingMusic())
	    Mix_PlayMusic(music,0);
}

void stop_music()
{
	if(Mix_PlayingMusic())
	{
	    while(Mix_FadeOutMusic(1500) && Mix_PlayingMusic())
		SDL_Delay(100);
	    Mix_FreeMusic(music);
	}
}
