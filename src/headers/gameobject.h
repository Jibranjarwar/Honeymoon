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
    void Render();

    // Movement
    void Movement(SDL_Event &event);

    // Texture
    SDL_Texture* Texture(const std::string filename);
    
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
    SDL_Texture* _objTexture = nullptr;
};