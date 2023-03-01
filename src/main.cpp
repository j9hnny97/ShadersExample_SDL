#include <iostream>

#include "Display.h"
#include "Definitions.h"
#include "ExampleInOpenGL.h"

int main(int argc, char** argv)
{
    Display const display(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT);
    ExampleInOpenGL const exampleInOpenGL(display.renderer);

    bool isRunning = true;

    while (isRunning)
    {
        exampleInOpenGL.tick(display.renderer);

        SDL_RenderPresent(display.renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                isRunning = false;
            }
        }
    }

    return 0;
}
