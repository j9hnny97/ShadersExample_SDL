#include <iostream>
#include <SDL_image.h>

#include "Display.h"
#include "Definitions.h"
#include "ExampleInOpenGL.h"

#include <vk_engine.h>

int main(int argc, char** argv)
{
    //Display const display(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT);
    //ExampleInOpenGL const exampleInOpenGL(display.renderer);

    VulkanEngine engine;
    engine.init();

    SDL_Renderer* renderer = SDL_CreateRenderer(engine._window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Event event;
    bool isRunning = true;

    SDL_Surface* surface = IMG_Load(makePath("bmt.png").c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textureRect = { (SCREEN_WIDTH - surface->w) / 2, (SCREEN_HEIGHT - surface->h) / 2, surface->w, surface->h };
    SDL_FreeSurface(surface);

    while (isRunning)
    {
        static SDL_Rect const screenRect{ 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };

        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &screenRect);
        //SDL_RenderCopy(renderer, _backgroundTexture, nullptr, &_backgroundTextureRect);

        SDL_RenderCopy(renderer, texture, nullptr, &textureRect);

        SDL_RenderPresent(renderer);
        SDL_Delay(500);
        engine.draw();
        SDL_Delay(500);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                isRunning = false;
            }
        }
    }

    SDL_DestroyTexture(texture);
    //engine.cleanup();
    return 0;
}
