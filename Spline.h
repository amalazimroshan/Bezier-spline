#pragma once
#include "CubicBezier.h"
#include <vector>

class Spline
{
  public:
    void addCurve(const CubicBezier &curve);
    void render(SDL_Renderer *renderer, int numOfPoints = 30);
    SDL_Point *getSelectedPoint(const SDL_Point &mousePos);
    void updatePoint(SDL_Point *point, SDL_Point mousePos);
    void newCurve(SDL_Point mousePos);
    void showTangent(SDL_Renderer *renderer, float u);
    void showVelocity(SDL_Renderer *renderer, float u);
    void removeCurve();

  private:
    std::vector<CubicBezier> curves;
    bool isWithinProximity(const SDL_Point &p, const SDL_Point &mousePos) const;
};