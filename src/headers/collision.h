#pragma once
#include <SDL2/SDL.h>
#include "gameobject.h"

class Collision{

public:
    Collision(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a);
    void Collision_Render(int thickness, int gridMinX, int gridMinY, int gridMaxX, int gridMaxY);

public:
    SDL_Rect _dest_rect;
    SDL_Renderer* _objRenderer;
    int _x, _y;
    int _original_x, _original_y;
    int _original_w, _original_h;
    int _width, _height;
    int _r, _g, _b, _a;
};