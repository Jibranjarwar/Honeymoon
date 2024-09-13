#pragma once

#include <SDL2/SDL.h>

class GameObject{

// public func
public:
    GameObject(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a);
    ~GameObject();

    // renderer
    void Render();
    
    // inline function which returns id
    inline int GetID() const { return _id; }

// private members
private:
    static int _current_id;
    int _id;
    int _width;
    int _height;
    int _x, _y;
    int _r, _g, _b;
    int _a = 255;
    SDL_Rect _dest_rect;
    SDL_Renderer* _objRenderer;

};