#pragma once
#include <SDL2/SDL.h>
#include "gameobject.h"

class Camera{

public:
    Camera(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a);
    void Camera_Render(int thickness, int gridMinX, int gridMinY, int gridMaxX, int gridMaxY);
    bool Game_Camera_Objects(GameObject object);
    void Resize(GameObject& previewObject, GameObject& editorObject, int window_width, int window_height);
    void UpdatePosX(int diff_x);
    void UpdatePosY(int diff_y);

public:
    SDL_Rect _dest_rect;
    SDL_Renderer* _objRenderer;
    int _x, _y;
    int _original_x, _original_y;
    int _original_w, _original_h;
    int _screen_x, _screen_y;
    int _width, _height;
    int _r, _g, _b, _a;
    bool ChangeScreenCord = false;
};