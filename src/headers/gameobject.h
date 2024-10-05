#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class GameObject{

// public func
public:
    GameObject(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a);
    GameObject(SDL_Renderer* renderer, std::string filename, int width, int height, int x, int y);
    ~GameObject();

    // Renderer
    void Render(int gridMinX = 0, int gridMinY = 0, int gridMaxX = 0, int gridMaxY = 0);

    void UpdatePos(int new_x, int new_y);

    // Movement
    void Movement(SDL_Event &event);

    // Texture
    SDL_Texture* Texture(const std::string filename);
    
    // inline function which returns id
    inline int GetID() const { return _id; }

public:
    int _x, _y;
    int _width, _height;
    int _original_w, _original_h;

// private members
private:
    static int _current_id;
    int _id;
    int _r, _g, _b;
    int _a = 255;
    SDL_Rect _dest_rect;
    SDL_Renderer* _objRenderer;
    SDL_Texture* _objTexture = nullptr;
};