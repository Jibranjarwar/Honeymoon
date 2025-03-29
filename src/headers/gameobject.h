#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include "json.hpp"
#include "collision.h"

using json = nlohmann::json;

class GameObject{

// public func
public:
    GameObject();
    GameObject(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a);
    GameObject(SDL_Renderer* renderer, std::string filename, int width, int height, int x, int y);
    GameObject(SDL_Renderer* renderer, std::string filename, std::string name, int width, int height, int x, int y);
    ~GameObject();

    void RenderPreview(SDL_Renderer* preview_renderer, int offset_x, int offset_y);

    void Render(int gridMinX = 0, int gridMinY = 0, int gridMaxX = 0, int gridMaxY = 0);

    void UpdatePosX(int diff_x);

    void UpdatePosY(int diff_y);

    void UpdatePosAll_X(int diff_x);

    void UpdatePosAll_Y(int diff_y);

    void AddCollision(SDL_Renderer* renderer);

    void RenderCollisionBox(int thickness, int gridMinX, int gridMinY, int gridMaxX, int gridMaxY);

    inline int getX() const { return _x; }

    inline int getY() const { return _y; }

    inline int getWidth() const { return _width; }

    inline int getHeight() const { return _height; }

    void static setEvent(SDL_Event* event);

    // Movement
    void Movement(SDL_Event &event);
    void Lua_Movement();

    // Texture
    SDL_Texture* Texture(const std::string filename, SDL_Renderer* renderer);
    
    // inline function which returns id
    inline int GetID() const { return _id; }

    void Setter();

    bool operator==(const GameObject& other) const {
        return _id == other._id;
    }

    bool operator!=(const GameObject& other) const {
        return !(*this == other);
    }

public:
    int _x, _y;
    int _screen_x, _screen_y;
    int _width, _height;
    int _original_w, _original_h;
    int _original_x, _original_y;
    int _id;
    std::string _filename;
    std::string _name;
    SDL_Rect _dest_rect;
    SDL_Texture* _objTexture = nullptr;
    SDL_Texture* _previewTexture = nullptr;
    bool addedCollision = false;
    Collision collisionBox;
    

// private members
private:
    static int _current_id;
    static SDL_Event* globalEvent;
    int _r, _g, _b;
    int _a = 255;
    SDL_Renderer* _objRenderer;
};