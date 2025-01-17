#pragma once
#include <SDL2/SDL.h>
#include "gameobject.h"

class Camera{

public:
    Camera(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a);
    void Camera_Render(int thickness);
    bool Game_Camera_Objects(GameObject object);
    void Resize(GameObject& object, int window_width, int window_height);

public:
    SDL_Rect _dest_rect;
    SDL_Renderer* _objRenderer;
    int _x, _y;
    int _width, _height;
    int _r, _g, _b, _a;
};