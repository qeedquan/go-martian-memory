void print_screen(SDL_Surface *surface);

#define total_bars 4
struct Cbar
{
    float y;
    int x, w, h;
    int R, G, B;
    float speed;
    int accel;
};
Cbar mbar[total_bars];
#define bar_top_speed 340
#define bar_w screen_w
#define bar_h 10
#define floor 420


void init_bars()
{
    mbar[0].x=0; mbar[0].y=100; mbar[0].w=bar_w; mbar[0].h=1;
    mbar[0].accel=150; mbar[0].speed=0;
    mbar[0].R=50; mbar[0].G=0; mbar[0].B=0;
    mbar[1].x=0; mbar[1].y=150; mbar[1].w=bar_w; mbar[1].h=1;
    mbar[1].accel=150; mbar[1].speed=0;
    mbar[1].R=0; mbar[1].G=50; mbar[1].B=0;
    mbar[2].x=0; mbar[2].y=200; mbar[2].w=bar_w; mbar[2].h=1;
    mbar[2].accel=150; mbar[2].speed=0;
    mbar[2].R=0; mbar[2].G=0; mbar[2].B=50;
    mbar[3].x=0; mbar[3].y=250; mbar[3].w=bar_w; mbar[3].h=1;
    mbar[3].accel=150; mbar[3].speed=0;
    mbar[3].R=10; mbar[3].G=10; mbar[3].B=10;
}

void bar_events(int number)
{
    SDL_Rect bar; //limpiamos la barra en la posicion anterior
	bar.x=mbar[number].x; bar.y=int(mbar[number].y); bar.h=bar_h*2; bar.w=bar_w;
    SDL_FillRect(screen, &bar, SDL_MapRGB(screen->format,0,0,0));
    mbar[number].speed+=mbar[number].accel*dt;
    mbar[number].y+=mbar[number].speed*dt;
    if(mbar[number].y>=(floor-bar_h*2))
    {
	mbar[number].speed=-bar_top_speed;
    }
}

void draw_bar(int number)
{
    bar_events(number);
    int x,y;
    int scanpos=int(mbar[number].y);
    SDL_Rect barpos;
	barpos.x=mbar[number].x;
	barpos.y=int(mbar[number].y);
	barpos.w=mbar[number].w;
	barpos.h=mbar[number].h;
    int R=mbar[number].R;
    int G=mbar[number].G;
    int B=mbar[number].B;
    for(y=scanpos; y<scanpos+bar_h; y++)
    {
	barpos.y=y;
	R+=(255/int(bar_h));
	G+=(255/int(bar_h));
	B+=(255/int(bar_h));
	if(R>255) R=255;
	if(G>255) G=255;
	if(B>255) B=255;
	SDL_FillRect(screen, &barpos, SDL_MapRGB(screen->format,R, G, B));
	barpos.y+=((bar_h-1)*2)-(y-scanpos)*2;
	SDL_FillRect(screen, &barpos, SDL_MapRGB(screen->format,R, G, B));
	barpos.y-=((bar_h-1)*2)-(y-scanpos)*2;
    }
}

void draw_bars()
{
    int n;
    for(n=0; n<total_bars; n++)
	draw_bar(n);
}



#define star_speed 50
#define total_stars 256
struct Cstar
{
    int x,y;
    int R, G, B;
    int speed;
};
Cstar mstar[total_stars];

void init_stars()
{
    int n;
    int speed;
    int R, G, B;
    speed=1;
    for(n=0; n<total_stars; n++)
    {
	if(speed==1) {R=49; G=62; B=71;}
	if(speed==2) {R=86; G=109; B=124;}
	if(speed==3) {R=130; G=162; B=183;}
	if(speed==4) {R=213; G=227; B=237;}
	mstar[n].x=int(rand()%screen_w);
	mstar[n].y=int(rand()%floor);
	mstar[n].R=R; mstar[n].G=G; mstar[n].B=B;
	mstar[n].speed=speed;
	speed++;
	if(speed>4)
	    speed=1;
    }
}

void draw_stars()
{
    SDL_Rect star;
	star.w=1; star.h=1;
    int n;

    for(n=0; n<total_stars; n++)
    {
	star.x=mstar[n].x;
	star.y=mstar[n].y;
	SDL_FillRect(screen, &star, SDL_MapRGB(screen->format,0,0,0));

	mstar[n].x+=int((star_speed*mstar[n].speed)*dt);
	if(mstar[n].x>=screen_w)
	    mstar[n].x=0;

	star.y=mstar[n].y;
	star.x=mstar[n].x;
	SDL_FillRect(screen, &star, SDL_MapRGB(screen->format,mstar[n].R,mstar[n].G,mstar[n].B));
    }
}


#define particles 2048
#define particle_accel 70
#define particle_max_speed 200
struct Cparticle
{
    float x, y;
    float speed_x, speed_y;
    int R, G, B;
};
Cparticle mparticle[particles];

void init_combo_fx(int x, int y, int w, int h)
{
    int n;
    for(n=0; n<particles; n++)
    {
	mparticle[n].x=rand()%w+x;
	mparticle[n].y=rand()%h+y;
	mparticle[n].speed_x=rand()%(particle_max_speed*2)-particle_max_speed;
	mparticle[n].speed_y=rand()%particle_max_speed-particle_max_speed;
	mparticle[n].R=255; mparticle[n].G=255; mparticle[n].B=255;
    }
}

void combo_fx()
{
    SDL_Rect partpos;
	partpos.w=2; partpos.h=2;
    int n;
    for(n=0; n<particles; n++)
    {
	if(mparticle[n].speed_x>0) mparticle[n].speed_x-=particle_accel*dt;
	else mparticle[n].speed_x+=particle_accel*dt;
	mparticle[n].speed_y+=particle_accel*dt;
	if(mparticle[n].speed_x>0 && mparticle[n].speed_x>= particle_max_speed) mparticle[n].speed_x= particle_max_speed;
	if(mparticle[n].speed_x<0 && mparticle[n].speed_x<= -particle_max_speed) mparticle[n].speed_x= -particle_max_speed;
	if(mparticle[n].speed_y>=particle_max_speed) mparticle[n].speed_y=particle_max_speed;
	mparticle[n].x+=mparticle[n].speed_x*dt;
	mparticle[n].y+=mparticle[n].speed_y*dt;
	partpos.x=int(mparticle[n].x); partpos.y=int(mparticle[n].y);
	SDL_FillRect(screen, &partpos, SDL_MapRGB(screen->format,mparticle[n].R, mparticle[n].G, mparticle[n].B));
    }
}
