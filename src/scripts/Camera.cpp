#include "camera.h"
#include <iostream>

Camera::Camera(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a):
_objRenderer(renderer), _width(width), _height(height), _x(x), _y(y), _r(r), _g(g), _b(b), _a(a){
}

void Camera::Camera_Render(int thickness){
    _dest_rect = {_width, _height, _x, _y};

    // Fills colour of borders
    SDL_SetRenderDrawColor(_objRenderer, _r, _g, _b, _a);

    // Top border
    SDL_Rect top = { _dest_rect.x, _dest_rect.y, _dest_rect.w, thickness };
    SDL_RenderFillRect(_objRenderer, &top);

    // Bottom border
    SDL_Rect bottom = { _dest_rect.x, _dest_rect.y + _dest_rect.h - thickness, _dest_rect.w, thickness };
    SDL_RenderFillRect(_objRenderer, &bottom);

    // Left border
    SDL_Rect left = { _dest_rect.x, _dest_rect.y, thickness, _dest_rect.h };
    SDL_RenderFillRect(_objRenderer, &left);

    // Right border
    SDL_Rect right = { _dest_rect.x + _dest_rect.w - thickness, _dest_rect.y, thickness, _dest_rect.h };
    SDL_RenderFillRect(_objRenderer, &right);
}

bool Camera::Game_Camera_Objects(GameObject object){
    //FIRST STEP: POSSIBLY GO THROUGH ALL GAMEOBJECTS THAT EXIST AND SEE IF ANY INTERSECT WITH OUR CAMERA?
    //std::cout << "HAS INTERACTION WITH: " << object._filename << ": " << SDL_HasIntersection(&_dest_rect, &object._dest_rect) << std::endl;
    return SDL_HasIntersection(&_dest_rect, &object._dest_rect);
}

void Camera::Resize(GameObject& object, int window_width, int window_height){
    // NEED TO CHANGE THE POSITIONS BASED ON DISTANCE AND SIZE
    // TO DO: Get Aspect ratio between gameObject and camera sizes
    // ISSUE: KEEPS EXPANDING INFINITLY BECAUSE OF THE LOOP IN MAIN

    // change window height aND Width might work?
    //std::cout << "RAHAHAHAH";
    float aspectRatio_height = static_cast<float>(_height / (float)object._original_h);
    float aspectRatio_width = static_cast<float>(_width / (float)object._original_w);
    int distance_y = _y - object._original_y;
    int distance_x = _x - object._original_x;
    float aspectRatio_x = static_cast<float>(_height / (float)distance_y);
    float aspectRatio_y = static_cast<float>(_width / (float)distance_x);

    //object._width = static_cast<int>(window_width / aspectRatio_width);
    int width_difference = static_cast<int>(window_width / aspectRatio_width);
    std::cout << "x difference: " << aspectRatio_x << " and y difference: " << aspectRatio_y << std::endl;
    //object._width = width_difference;
    //object._height = static_cast<int>(window_height / aspectRatio_height);
    //object._x = static_cast<int>(window_width / aspectRatio_x);
    //object._y = static_cast<int>(window_height / aspectRatio_y);

}