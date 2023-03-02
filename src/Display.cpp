
#include "Display.h"

#include <iostream>

Display::Display(int x, int y, int w, int h)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    }
    else
    {
        window = SDL_CreateWindow("Hello there!", x, y, w, h, SDL_WINDOW_VULKAN);
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
}

Display::~Display()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::draw() const
{
    SDL_RenderPresent(renderer);
}
