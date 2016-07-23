Uint32 lastframe, thisframe;
double  dt;

void timing()
{
    thisframe=SDL_GetTicks();
    dt=(thisframe-lastframe)/1000.0;
    lastframe=thisframe;
    if(dt>.2) dt=0.01;
}
