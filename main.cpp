#include <SDL2/SDL.h>
#include <iostream>
#include <list>

SDL_Point BezierPoint(float t, SDL_Point p0, SDL_Point p1, SDL_Point p2, SDL_Point p3)
{
    SDL_Point result;
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    result.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    result.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;

    return result;
}

int main(int argc, char **argv)
{
    bool quit = false;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("rect", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Rect rect1 = {290, 210, 10, 10};
    SDL_Rect rect2 = {100, 100, 10, 10};
    SDL_Rect rect3 = {500, 100, 10, 10};
    SDL_Rect rect4 = {100, 400, 10, 10};

    std::list<SDL_Rect *> rectangles;
    rectangles.push_back(&rect1);
    rectangles.push_back(&rect2);
    rectangles.push_back(&rect3);
    rectangles.push_back(&rect4);

    SDL_Rect *selectedRect = NULL;
    bool leftMouseButtonDown = false;
    SDL_Point clickOffset;
    SDL_Point mousePos;
    while (!quit)
    {
        SDL_Delay(10);
        SDL_PollEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                quit = true;
            break;
        case SDL_MOUSEMOTION:
            mousePos = {event.motion.x, event.motion.y};
            if (leftMouseButtonDown && selectedRect != NULL)
            {
                selectedRect->x = mousePos.x - clickOffset.x;
                selectedRect->y = mousePos.y - clickOffset.y;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftMouseButtonDown = false;
                selectedRect = NULL;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftMouseButtonDown = true;
                for (auto rect : rectangles)
                {
                    if (SDL_PointInRect(&mousePos, rect))
                    {
                        selectedRect = rect;
                        clickOffset.x = mousePos.x - rect->x;
                        clickOffset.y = mousePos.y - rect->y;
                        break;
                    }
                }
            }
        default:
            break;
        }

        // rendering points
        SDL_SetRenderDrawColor(renderer, 12, 45, 72, 200);
        SDL_RenderClear(renderer);
        for (auto const &rect : rectangles)
        {
            if (rect == selectedRect)
                SDL_SetRenderDrawColor(renderer, 100, 140, 155, 200);
            else
                SDL_SetRenderDrawColor(renderer, 200, 240, 255, 255);
            SDL_RenderFillRect(renderer, rect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (auto it = rectangles.begin(); it != rectangles.end(); ++it)
        {
            auto next_it = std::next(it);
            if (next_it != rectangles.end())
            {
                SDL_Rect *rect1 = *it;
                SDL_Rect *rect2 = *next_it;
                SDL_RenderDrawLine(renderer, rect1->x + rect1->w / 2, rect1->y + rect1->h / 2, rect2->x + rect2->w / 2,
                                   rect2->y + rect2->h / 2);
            }
        }

        // draw cubic bezier curve
        SDL_Point p0 = {rect1.x + rect1.w / 2, rect1.y + rect1.h / 2};
        SDL_Point p1 = {rect2.x + rect2.w / 2, rect2.y + rect2.h / 2};
        SDL_Point p2 = {rect3.x + rect3.w / 2, rect3.y + rect3.h / 2};
        SDL_Point p3 = {rect4.x + rect4.w / 2, rect4.y + rect4.h / 2};

        // Draw the cubic Bezier curve
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Red color for the curve
        SDL_Point prevPoint = p0;
        for (float t = 0.0; t <= 1.0; t += 0.01)
        {
            SDL_Point currentPoint = BezierPoint(t, p0, p1, p2, p3);
            SDL_RenderDrawLine(renderer, prevPoint.x, prevPoint.y, currentPoint.x, currentPoint.y);
            prevPoint = currentPoint;
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}