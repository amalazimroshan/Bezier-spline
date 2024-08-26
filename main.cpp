#include "Spline.h"
#include <SDL2/SDL.h>
#include <iostream>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Bezier Curve", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    Spline spline;
    spline.addCurve({{100, 100}, {150, 300}, {300, 300}, {400, 100}});

    bool running = true;
    SDL_Event event;
    SDL_Point mousePos;
    bool leftMouseButtonDown = false;
    SDL_Point *selectedPoint = NULL;
    while (running)
    {
        SDL_Delay(10);
        SDL_PollEvent(&event);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        spline.render(renderer);
        // spline.showVelocity(renderer, 1.3);
        // spline.showTangent(renderer, .6);
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
            if (event.key.keysym.sym == SDLK_DELETE)
            {
                spline.removeCurve();
            }
            break;
        case SDL_MOUSEMOTION:
            mousePos = {event.motion.x, event.motion.y};
            if (leftMouseButtonDown && selectedPoint)
            {
                spline.updatePoint(selectedPoint, mousePos);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftMouseButtonDown = false;
                selectedPoint = NULL;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftMouseButtonDown = true;
                selectedPoint = spline.getSelectedPoint(mousePos);
                if (selectedPoint == nullptr)
                    spline.newCurve(mousePos);
            }
        default:
            break;
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}