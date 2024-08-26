#include "CubicBezier.h"
#include <cmath>

CubicBezier::CubicBezier(const SDL_Point &p0, const SDL_Point &p1, const SDL_Point &p2, const SDL_Point &p3)
    : p0(p0), p1(p1), p2(p2), p3(p3)
{
}

SDL_FPoint CubicBezier::evaluate(float t) const
{
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float ttt = tt * t;
    float uuu = uu * u;

    return {uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x,
            uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y};
}

SDL_FPoint CubicBezier::velocity(float t) const
{
    float tt = t * t;
    return {p0.x * (-3 * tt + 6 * t - 3) + p1.x * (9 * tt - 12 * t + 3) + p2.x * (-9 * tt + 6 * t) + p3.x * (3 * tt),
            p0.y * (-3 * tt + 6 * t - 3) + p1.y * (9 * tt - 12 * t + 3) + p2.y * (-9 * tt + 6 * t) + p3.y * (3 * tt)};
}
void CubicBezier::render(SDL_Renderer *renderer, int numOfPoints) const
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_FPoint previousPoint = evaluate(0);
    for (int i = 1; i < numOfPoints; ++i)
    {
        float t = static_cast<float>(i) / numOfPoints;
        SDL_FPoint currentPoint = evaluate(t);
        SDL_RenderDrawLine(renderer, static_cast<int>(previousPoint.x), static_cast<int>(previousPoint.y),
                           static_cast<int>(currentPoint.x), static_cast<int>(currentPoint.y));
        previousPoint = currentPoint;
    }
}
void CubicBezier::drawControlPoints(SDL_Renderer *renderer) const
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // Cyan color
    SDL_Rect rects[4] = {{p0.x - 5, p0.y - 5, 10, 10},
                         {p1.x - 5, p1.y - 5, 10, 10},
                         {p2.x - 5, p2.y - 5, 10, 10},
                         {p3.x - 5, p3.y - 5, 10, 10}};
    for (const auto &rect : rects)
    {
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255); // Light blue color
    SDL_RenderDrawLine(renderer, p0.x, p0.y, p1.x, p1.y);
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    SDL_RenderDrawLine(renderer, p2.x, p2.y, p3.x, p3.y);
}