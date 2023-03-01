#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL.h>

class Display
{
public:
    Display(int x, int y, int w, int h);
    ~Display();

    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
};

#endif
