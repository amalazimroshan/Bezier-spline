// Spline.cpp
#include "Spline.h"
#include <cmath>
#include <iostream>

namespace
{
constexpr double PI = 3.14159265358979323846;
constexpr double DEG_TO_RAD = PI / 180.0;
constexpr int PROXIMITY_THRESHOLD = 10;

SDL_Point calculateMidPoint(const SDL_Point &p1, const SDL_Point &p2)
{
    return {(p1.x + p2.x) / 2, (p1.y + p2.y) / 2};
}

SDL_Point rotatePoint(const SDL_Point &center, const SDL_Point &point, double angleRad)
{
    int dx = point.x - center.x;
    int dy = point.y - center.y;
    double cos_angle = cos(angleRad);
    double sin_angle = sin(angleRad);
    return {center.x + static_cast<int>(dx * cos_angle - dy * sin_angle),
            center.y + static_cast<int>(dx * sin_angle + dy * cos_angle)};
}

bool isClockwise(const SDL_Point &p1, const SDL_Point &p2, const SDL_Point &p3)
{
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x) < 0;
}
} // namespace

void Spline::addCurve(const CubicBezier &curve)
{
    curves.push_back(curve);
}

void Spline::render(SDL_Renderer *renderer, int numOfPoints)
{
    for (auto &curve : curves)
    {
        curve.render(renderer, numOfPoints);
        curve.drawControlPoints(renderer);
    }
}

SDL_Point *Spline::getSelectedPoint(const SDL_Point &mousePos)
{
    for (size_t i = 0; i < curves.size(); ++i)
    {
        auto &curve = curves[i];
        if (isWithinProximity(curve.p0, mousePos))
            return (i > 0) ? &curves[i - 1].p3 : &curve.p0;
        if (isWithinProximity(curve.p1, mousePos))
            return &curve.p1;
        if (isWithinProximity(curve.p2, mousePos))
            return &curve.p2;
        if (isWithinProximity(curve.p3, mousePos))
            return (i < curves.size() - 1) ? &curves[i + 1].p0 : &curve.p3;
    }
    return nullptr;
}

void Spline::updatePoint(SDL_Point *point, SDL_Point mousePos)
{
    *point = mousePos;
    for (size_t i = 0; i < curves.size(); ++i)
    {
        if (point == &curves[i].p3 && i < curves.size() - 1)
        {
            curves[i + 1].p0 = *point;
            break;
        }
        if (point == &curves[i].p0 && i > 0)
        {
            curves[i - 1].p3 = *point;
            break;
        }
    }
}

void Spline::newCurve(SDL_Point mousePos)
{
    const SDL_Point &startPoint = curves.back().p3;
    const SDL_Point &endPoint = mousePos;
    SDL_Point midPoint = calculateMidPoint(startPoint, endPoint);

    const SDL_Point &previousControlPoint = curves.back().p2;
    bool clockwise = isClockwise(previousControlPoint, startPoint, mousePos);

    double angle1 = clockwise ? 312 * DEG_TO_RAD : 45 * DEG_TO_RAD;
    double angle2 = clockwise ? 225 * DEG_TO_RAD : 135 * DEG_TO_RAD;

    SDL_Point rotatedEnd1 = rotatePoint(midPoint, startPoint, angle1);
    SDL_Point rotatedEnd2 = rotatePoint(midPoint, startPoint, angle2);

    curves.emplace_back(startPoint, rotatedEnd1, rotatedEnd2, endPoint);
}

void Spline::showTangent(SDL_Renderer *renderer, float u)
{
    int index = static_cast<int>(u);
    float t = u - index;
    if (index >= 0 && index < static_cast<int>(curves.size()))
    {
        CubicBezier &curve = curves[index];
        SDL_FPoint point = curve.evaluate(t);
        SDL_FPoint tangent = curve.velocity(t);

        float length = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
        if (length > 0)
        {
            tangent.x /= length;
            tangent.y /= length;
        }
        float scale = 50.0f;
        SDL_FPoint endPoint = {point.x + tangent.x * scale, point.y + tangent.y * scale};
        SDL_RenderDrawLineF(renderer, point.x, point.y, endPoint.x, endPoint.y);
    }
}

void Spline::showVelocity(SDL_Renderer *renderer, float u)
{
    int index = static_cast<int>(u);
    float t = u - index;
    if (index >= 0 && index <= static_cast<int>(curves.size()))
    {
        CubicBezier &curve = curves[index];
        SDL_FPoint point = curve.evaluate(t);
        SDL_FPoint velocity = curve.velocity(t);
        SDL_FPoint endPoint = {point.x + velocity.x, point.y + velocity.y};
        SDL_RenderDrawLineF(renderer, point.x, point.y, endPoint.x, endPoint.y);
    }
}

void Spline::removeCurve()
{
    if (curves.size() > 1)
    {
        curves.pop_back();
    }
}

bool Spline::isWithinProximity(const SDL_Point &p, const SDL_Point &mousePos) const
{
    int dx = p.x - mousePos.x;
    int dy = p.y - mousePos.y;
    return (dx * dx + dy * dy) <= PROXIMITY_THRESHOLD * PROXIMITY_THRESHOLD;
}