#pragma once
#include <SDL2/SDL.h>

class CubicBezier
{
  public:
    SDL_Point p0, p1, p2, p3;

    CubicBezier(const SDL_Point &p0, const SDL_Point &p1, const SDL_Point &p2, const SDL_Point &p3);

    SDL_FPoint evaluate(float t) const;
    SDL_FPoint velocity(float t) const;
    void render(SDL_Renderer *renderer, int numOfPoints) const;
    void drawControlPoints(SDL_Renderer *renderer) const;
};