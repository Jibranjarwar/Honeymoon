#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

class GameObject{

// public func
public:
    GameObject();
    GameObject(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a);
    GameObject(SDL_Renderer* renderer, std::string filename, int width, int height, int x, int y);
    ~GameObject();

    void RenderPreview(SDL_Renderer* preview_renderer, int offset_x, int offset_y);

    void Render(int gridMinX = 0, int gridMinY = 0, int gridMaxX = 0, int gridMaxY = 0);

    void UpdatePos(int new_x, int new_y);

    inline int getX() const { return _x; }

    inline int getY() const { return _y; }

    inline int getWidth() const { return _width; }

    inline int getHeight() const { return _height; }

    // Movement
    void Movement(SDL_Event &event);

    // Texture
    SDL_Texture* Texture(const std::string filename, SDL_Renderer* renderer);
    
    // inline function which returns id
    inline int GetID() const { return _id; }

    void Setter();

public:
    int _x, _y;
    int _width, _height;
    int _original_w, _original_h;
    int _original_x, _original_y;
    int _id;
    std::string _filename;

// private members
private:
    static int _current_id;
    int _r, _g, _b;
    int _a = 255;
    SDL_Rect _dest_rect;
    SDL_Renderer* _objRenderer;
    SDL_Texture* _objTexture = nullptr;
    SDL_Texture* _previewTexture = nullptr;
};