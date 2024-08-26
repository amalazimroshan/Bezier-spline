#include "Spline.h"
#include <cmath>
#include <iostream>

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
    point->x = mousePos.x;
    point->y = mousePos.y;
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
    SDL_Point startPoint = curves.back().p3;
    SDL_Point endPoint = mousePos;
    SDL_Point midPoint = {(startPoint.x + endPoint.x) / 2, (startPoint.y + endPoint.y) / 2};

    int dx = startPoint.x - midPoint.x;
    int dy = startPoint.y - midPoint.y;
    SDL_Point origin = curves.back().p2;
    int cross = (origin.x - startPoint.x) * (mousePos.y - startPoint.y) -
                (origin.y - startPoint.y) * (mousePos.x - startPoint.x);
    double angleRad1;
    double angleRad2;
    if (cross < 0)
    {
        angleRad1 = 45 * M_PI / 180.0;
        angleRad2 = 135 * M_PI / 180.0;
    }
    else
    {
        angleRad1 = 312 * M_PI / 180.0;
        angleRad2 = 225 * M_PI / 180.0;
    }
    int rotatedDx = static_cast<int>(dx * cos(angleRad1) - dy * sin(angleRad1));
    int rotatedDy = static_cast<int>(dx * sin(angleRad1) + dy * cos(angleRad1));
    SDL_Point rotatedEnd1 = {midPoint.x + rotatedDx, midPoint.y + rotatedDy};
    rotatedDx = static_cast<int>(dx * cos(angleRad2) - dy * sin(angleRad2));
    rotatedDy = static_cast<int>(dx * sin(angleRad2) + dy * cos(angleRad2));
    SDL_Point rotatedEnd2 = {midPoint.x + rotatedDx, midPoint.y + rotatedDy};
    CubicBezier newCurve(startPoint, rotatedEnd1, rotatedEnd2, endPoint);
    curves.push_back(newCurve);
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
        curves.pop_back();
}
bool Spline::isWithinProximity(const SDL_Point &p, const SDL_Point &mousePos) const
{
    int dx = p.x - mousePos.x;
    int dy = p.y - mousePos.y;
    return sqrt(dx * dx + dy * dy) <= 10;
}