#include "CubicBezier.h"
#include <cmath>

namespace
{
constexpr int CONTROL_POINT_SIZE = 10;
constexpr int CONTROL_POINT_HALF_SIZE = CONTROL_POINT_SIZE / 2;

SDL_FPoint interpolate(const SDL_Point &p0, const SDL_Point &p1, const SDL_Point &p2, const SDL_Point &p3, float t)
{
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    return {uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x,
            uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y};
}

SDL_Rect createControlPointRect(const SDL_Point &p)
{
    return {p.x - CONTROL_POINT_HALF_SIZE, p.y - CONTROL_POINT_HALF_SIZE, CONTROL_POINT_SIZE, CONTROL_POINT_SIZE};
}
} // namespace

CubicBezier::CubicBezier(const SDL_Point &p0, const SDL_Point &p1, const SDL_Point &p2, const SDL_Point &p3)
    : p0(p0), p1(p1), p2(p2), p3(p3)
{
}

SDL_FPoint CubicBezier::evaluate(float t) const
{
    return interpolate(p0, p1, p2, p3, t);
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
    for (int i = 1; i <= numOfPoints; ++i)
    {
        float t = static_cast<float>(i) / numOfPoints;
        SDL_FPoint currentPoint = evaluate(t);
        SDL_RenderDrawLineF(renderer, previousPoint.x, previousPoint.y, currentPoint.x, currentPoint.y);
        previousPoint = currentPoint;
    }
}

void CubicBezier::drawControlPoints(SDL_Renderer *renderer) const
{
    // Draw control points
    SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255); // Light green color
    SDL_Rect controlPoints[] = {createControlPointRect(p0), createControlPointRect(p1), createControlPointRect(p2),
                                createControlPointRect(p3)};
    SDL_RenderFillRects(renderer, controlPoints, 4);

    // Draw control lines
    SDL_RenderDrawLine(renderer, p0.x, p0.y, p1.x, p1.y);
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    SDL_RenderDrawLine(renderer, p2.x, p2.y, p3.x, p3.y);
}