#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
class CubicBezier
{
  public:
    SDL_Point p0, p1, p2, p3;
    CubicBezier(const SDL_Point &p0, const SDL_Point &p1, const SDL_Point &p2, const SDL_Point &p3)
        : p0(p0), p1(p1), p2(p2), p3(p3) {};

    SDL_FPoint evaluate(float t) const
    {
        float u = 1 - t;
        float tt = t * t;
        float uu = u * u;
        float ttt = tt * t;
        float uuu = uu * u;
        SDL_FPoint p = {uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x,
                        uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y};
        return p;
    }
    SDL_FPoint velocity(float t) const
    {
        float tt = t * t;
        SDL_FPoint velocity = {
            p0.x * (-3 * tt + 6 * t - 3) + p1.x * (9 * tt - 12 * t + 3) + p2.x * (-9 * tt + 6 * t) + p3.x * (3 * tt),
            p0.y * (-3 * tt + 6 * t - 3) + p1.y * (9 * tt - 12 * t + 3) + p2.y * (-9 * tt + 6 * t) + p3.y * (3 * tt)};
        return velocity;
    }
    void render(SDL_Renderer *renderer, int numOfPoints) const
    {
        for (int i = 0; i < numOfPoints; ++i)
        {
            float t = static_cast<float>(i) / numOfPoints;
            SDL_FPoint p = evaluate(t);
            SDL_RenderDrawPoint(renderer, p.x, p.y);
        }
    }
    void drawControlPoints(SDL_Renderer *renderer) const
    {
        SDL_Rect rect0 = {p0.x - 5, p0.y - 5, 10, 10};
        SDL_Rect rect1 = {p1.x - 5, p1.y - 5, 10, 10};
        SDL_Rect rect2 = {p2.x - 5, p2.y - 5, 10, 10};
        SDL_Rect rect3 = {p3.x - 5, p3.y - 5, 10, 10};
        SDL_RenderFillRect(renderer, &rect0);
        SDL_RenderFillRect(renderer, &rect1);
        SDL_RenderFillRect(renderer, &rect2);
        SDL_RenderFillRect(renderer, &rect3);
        SDL_RenderDrawLine(renderer, p0.x, p0.y, p1.x, p1.y);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
        SDL_RenderDrawLine(renderer, p2.x, p2.y, p3.x, p3.y);
    }
};
bool isWithinProximity(const SDL_Point &p, const SDL_Point &mousePos)
{
    int dx = p.x - mousePos.x;
    int dy = p.y - mousePos.y;
    return sqrt(dx * dx + dy * dy) <= 10;
}
class Spline
{
  private:
    std::vector<CubicBezier> curves;

  public:
    void addCurve(const CubicBezier &curve)
    {
        curves.push_back(curve);
    }
    void render(SDL_Renderer *renderer, int numOfPoints = 30)
    {
        for (auto curve : curves)
        {
            curve.render(renderer, numOfPoints);
            curve.drawControlPoints(renderer);
        }
    }
    SDL_Point *getSelectedPoint(const SDL_Point &mousePos)
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
    void updatePoint(SDL_Point *point, SDL_Point mousePos)
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
    void newCurve(SDL_Point mousePos)
    {
        SDL_FPoint velocity = curves.back().velocity(1);
        SDL_Point lastPoint = curves.back().p3;
        CubicBezier curve(lastPoint, {static_cast<int>(velocity.x), static_cast<int>(velocity.y)}, {100, 100},
                          mousePos);
        curves.push_back(curve);
        std::cout << velocity.x << "," << velocity.y << std::endl;
    }
    void showVelocity(SDL_Renderer *renderer, float u)
    {
        int index = static_cast<int>(u);
        float t = u - index;
        CubicBezier curve = curves[index];
        SDL_FPoint m = curve.evaluate(t);
        SDL_FPoint n = curve.velocity(t);
        SDL_RenderDrawLine(renderer, static_cast<int>(m.x), static_cast<int>(m.y), static_cast<int>(n.x),
                           static_cast<int>(n.y));
    }
    void getInfo() const
    {
        int curveIndex = 0;
        for (const auto &curve : curves)
        {
            std::cout << "Curve " << curveIndex << ":" << std::endl;
            std::cout << "  p0: (" << curve.p0.x << ", " << curve.p0.y << ")" << std::endl;
            std::cout << "  p1: (" << curve.p1.x << ", " << curve.p1.y << ")" << std::endl;
            std::cout << "  p2: (" << curve.p2.x << ", " << curve.p2.y << ")" << std::endl;
            std::cout << "  p3: (" << curve.p3.x << ", " << curve.p3.y << ")" << std::endl;
            curveIndex++;
        }
    }
};
int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Bezier Curve", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    CubicBezier curve1({100, 100}, {150, 300}, {300, 300}, {400, 100});
    CubicBezier curve2({400, 100}, {450, 10}, {600, 10}, {650, 200});
    Spline spline;
    spline.addCurve(curve1);
    spline.addCurve(curve2);

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

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White rectangles
        spline.render(renderer);
        spline.showVelocity(renderer, 1.3);
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
            if (event.key.keysym.sym == SDLK_END)
            {
                spline.getInfo();
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