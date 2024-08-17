#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

void drawLine(SDL_Renderer* renderer, SDL_Point p1, SDL_Point p2) {
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
}

void drawBezierCurve(SDL_Renderer* renderer, SDL_Point p0, SDL_Point p1, SDL_Point p2) {
    const int num_points = 100;
    SDL_Point prev_point = p0;
    for (int i = 0; i <= num_points; ++i) {
        float t = i / static_cast<float>(num_points);
        float x = std::pow(1 - t, 2) * p0.x + 2 * (1 - t) * t * p1.x + std::pow(t, 2) * p2.x;
        float y = std::pow(1 - t, 2) * p0.y + 2 * (1 - t) * t * p1.y + std::pow(t, 2) * p2.y;
        SDL_Point point = { static_cast<int>(x), static_cast<int>(y) };
        if (i > 0) {
            drawLine(renderer, prev_point, point);
        }
        prev_point = point;
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Quadratic Bézier Curve", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderClear(renderer);

    // Get user input
    SDL_Point p0, p1, p2;
    std::cout << "Enter coordinates for point P0 (x y): ";
    std::cin >> p0.x >> p0.y;
    std::cout << "Enter coordinates for point P1 (x y): ";
    std::cin >> p1.x >> p1.y;
    std::cout << "Enter coordinates for point P2 (x y): ";
    std::cin >> p2.x >> p2.y;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 

    drawBezierCurve(renderer, p0, p1, p2);

    SDL_RenderPresent(renderer);

    SDL_Delay(5000);

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
