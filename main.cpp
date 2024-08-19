#include <SDL2/SDL.h>
#include <iostream>
#include <list>

class BezierCurve
{
  public:
    SDL_Point p0, p1, p2, p3;
    SDL_Rect rect1;
    SDL_Rect rect2;
    SDL_Rect rect3;
    SDL_Rect rect4;

    BezierCurve(SDL_Point point0, SDL_Point point1, SDL_Point point2, SDL_Point point3)
        : p0(point0), p1(point1), p2(point2), p3(point3), rect1{p0.x, p0.y, 10, 10}, rect2{p1.x, p1.y, 10, 10},
          rect3{p2.x, p2.y, 10, 10}, rect4{p3.x, p3.y, 10, 10}
    {
    }
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
    void render(SDL_Renderer *renderer, int numOfPoints) const
    {
        for (int i = 0; i < numOfPoints; ++i)
        {
            float t = static_cast<float>(i) / numOfPoints;
            SDL_FPoint p = evaluate(t);
            SDL_RenderDrawPoint(renderer, p.x, p.y);
        }
    }
    void updateRectangles()
    {
        rect1 = {p0.x, p0.y, 10, 10};
        rect2 = {p1.x, p1.y, 10, 10};
        rect3 = {p2.x, p2.y, 10, 10};
        rect4 = {p3.x, p3.y, 10, 10};
    }
    void setControlPoint(int index, const SDL_Point &point)
    {
        switch (index)
        {
        case 0:
            p0 = point;
            break;
        case 1:
            p1 = point;
            break;
        case 2:
            p2 = point;
            break;
        case 3:
            p3 = point;
            break;
        }
        updateRectangles();
    }
    void renderPoints(SDL_Renderer *renderer) const
    {
        SDL_RenderDrawRect(renderer, &rect1);
        SDL_RenderDrawRect(renderer, &rect2);
        SDL_RenderDrawRect(renderer, &rect3);
        SDL_RenderDrawRect(renderer, &rect4);

        SDL_RenderDrawLine(renderer, p0.x + 5, p0.y + 5, p1.x + 5, p1.y + 5);
        SDL_RenderDrawLine(renderer, p1.x + 5, p1.y + 5, p2.x + 5, p2.y + 5);
        SDL_RenderDrawLine(renderer, p2.x + 5, p2.y + 5, p3.x + 5, p3.y + 5);
    }
};
class BezierSpline
{
  private:
    std::list<BezierCurve> curves;

  public:
    void addCurve(const BezierCurve &curve)
    {
        curves.push_back(curve);
    }
    SDL_FPoint evaluate(float t) const
    {
        if (curves.empty())
            return {0, 0};

        int numCurves = curves.size();
        float curveT = t * numCurves;
        int curveIndex = static_cast<int>(curveT);
        if (curveIndex >= numCurves)
            curveIndex = numCurves - 1;

        float localT = curveT - curveIndex;
        auto it = curves.begin();
        std::advance(it, curveIndex);
        return it->evaluate(localT);
    }
    // void render(SDL_Renderer *renderer, int numPoints = 100) const
    // {
    //     if (curves.empty())
    //         return;
    //     SDL_FPoint previousPoint = evaluate(0.0f);
    //     for (int i = 0; i < numPoints; i++)
    //     {
    //         float t = static_cast<float>(i) / (numPoints - 1);
    //         SDL_FPoint currentPoint = evaluate(t);
    //         // SDL_RenderDrawLine(renderer, previousPoint.x, previousPoint.y, currentPoint.x, currentPoint.y);
    //         SDL_RenderDrawPoint(renderer, currentPoint.x, currentPoint.y);
    //         previousPoint = currentPoint;
    //     }
    // }

    void render(SDL_Renderer *renderer, int pointsPerCurve = 10)
    {
        for (auto &curve : curves)
        {
            curve.render(renderer, pointsPerCurve);
            curve.renderPoints(renderer);
        }
    }

    SDL_Rect *getClickedRect(const SDL_Point &mousePos)
    {
        for (auto &curve : curves)
        {
            if (SDL_PointInRect(&mousePos, &curve.rect1))
                return &curve.rect1;
            if (SDL_PointInRect(&mousePos, &curve.rect2))
                return &curve.rect2;
            if (SDL_PointInRect(&mousePos, &curve.rect3))
                return &curve.rect3;
            if (SDL_PointInRect(&mousePos, &curve.rect4))
                return &curve.rect4;
        }
        return nullptr;
    }
    int getSelectedPointIndex(const SDL_Rect *rect) const
    {
        if (!rect)
            return -1;

        for (const auto &curve : curves)
        {
            if (rect == &curve.rect1)
                return 0;
            if (rect == &curve.rect2)
                return 1;
            if (rect == &curve.rect3)
                return 2;
            if (rect == &curve.rect4)
                return 3;
        }
        return -1;
    }
    void updatePointPosition(SDL_Rect *rect, const SDL_Point &newPoint)
    {
        if (!rect)
            return;

        for (auto &curve : curves)
        {
            if (rect == &curve.rect1)
                curve.setControlPoint(0, newPoint);
            else if (rect == &curve.rect2)
                curve.setControlPoint(1, newPoint);
            else if (rect == &curve.rect3)
                curve.setControlPoint(2, newPoint);
            else if (rect == &curve.rect4)
                curve.setControlPoint(3, newPoint);

            curve.updateRectangles();
        }
    }
};

int main(int argc, char **argv)
{
    bool quit = false;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("rect", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Rect *selectedRect = NULL;
    bool leftMouseButtonDown = false;
    SDL_Point mousePos;

    BezierCurve curve1({100, 100}, {150, 300}, {300, 300}, {400, 100});
    BezierCurve curve2({400, 100}, {450, 10}, {600, 10}, {650, 200});
    BezierCurve curve3({650, 200}, {700, 200}, {750, 200}, {800, 100});
    BezierSpline spline;
    spline.addCurve(curve1);
    spline.addCurve(curve2);
    spline.addCurve(curve3);

    while (!quit)
    {
        SDL_Delay(10);
        SDL_PollEvent(&event);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

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
            if (leftMouseButtonDown && selectedRect)
            {
                selectedRect->x = mousePos.x - selectedRect->w / 2;
                selectedRect->y = mousePos.y - selectedRect->h / 2;
                SDL_Point newPoint = {selectedRect->x, selectedRect->y};
                spline.updatePointPosition(selectedRect, newPoint);
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
                selectedRect = spline.getClickedRect(mousePos);
                if (selectedRect != NULL)
                {
                    std::cout << selectedRect->x << ',' << selectedRect->y << std::endl;
                }
            }
            break;
        default:
            break;
        }
        spline.render(renderer);
        // for (auto it = rectangles.begin(); it != rectangles.end(); ++it)
        // {
        //     auto next_it = std::next(it);
        //     if (next_it != rectangles.end())
        //     {
        //         SDL_Rect *rect1 = *it;
        //         SDL_Rect *rect2 = *next_it;
        //         SDL_RenderDrawLine(renderer, rect1->x + rect1->w / 2, rect1->y + rect1->h / 2, rect2->x +
        //         rect2->w / 2,
        //                            rect2->y + rect2->h / 2);
        //     }
        // }

        // // draw cubic bezier curve
        // SDL_Point p0 = {rect1.x + rect1.w / 2, rect1.y + rect1.h / 2};
        // SDL_Point p1 = {rect2.x + rect2.w / 2, rect2.y + rect2.h / 2};
        // SDL_Point p2 = {rect3.x + rect3.w / 2, rect3.y + rect3.h / 2};
        // SDL_Point p3 = {rect4.x + rect4.w / 2, rect4.y + rect4.h / 2};

        // // Draw the cubic Bezier curve
        // SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Red color for the curve
        // SDL_Point prevPoint = p0;
        // for (float t = 0.0; t <= 1.0; t += 0.01)
        // {
        //     SDL_Point currentPoint = BezierPoint(t, p0, p1, p2, p3);
        //     SDL_RenderDrawLine(renderer, prevPoint.x, prevPoint.y, currentPoint.x, currentPoint.y);
        //     prevPoint = currentPoint;
        // }

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}