#include "camera.h"
#include <iostream>
#include <cmath>
#include "gamescreen.h"

Camera::Camera(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a):
_objRenderer(renderer), _width(width), _original_w(width), _height(height), _original_h(height), _x(x), _original_x(x), _y(y), _original_y(y), _r(r), _g(g), _b(b), _a(a){
}

void Camera::Camera_Render(int thickness, int gridMinX, int gridMinY, int gridMaxX, int gridMaxY){
    
    int value_y;
    
    _dest_rect = {_x, _y, _width, _height};
    SDL_Rect display_rect = _dest_rect;

    // Fills colour of borders
    SDL_SetRenderDrawColor(_objRenderer, _r, _g, _b, _a);

    // CUTS RIGHT SIDE OF EDGE: LOOK AT gameObject.cpp for more info on logic
    if(_x < gridMinX){
        display_rect.x = gridMinX;
        display_rect.w = _width - (gridMinX - _x);
    }

    if(_x + _width > gridMinX && _y < gridMaxY){

        // Top border
        SDL_Rect top = { display_rect.x, display_rect.y, display_rect.w, thickness};
        SDL_RenderFillRect(_objRenderer, &top);

        // Bottom border
        // CUTS BOTTOM SIDE OF EDGE: LOOK AT gameObject.cpp for more info on logic
        if(_y + _height > gridMaxY){
            display_rect.h = _height - ((_y + _height) - gridMaxY);
        }else{
            SDL_Rect bottom = { display_rect.x, display_rect.y + display_rect.h - thickness, display_rect.w, thickness };
            SDL_RenderFillRect(_objRenderer, &bottom);
        }

        // Left border
        if(_x > gridMinX){
            SDL_Rect left = { display_rect.x, display_rect.y, thickness, display_rect.h};
            SDL_RenderFillRect(_objRenderer, &left);
        }

        // Right border
        SDL_Rect right = { display_rect.w + display_rect.x - thickness, display_rect.y, thickness, display_rect.h};
        SDL_RenderFillRect(_objRenderer, &right);
    }
}

bool Camera::Game_Camera_Objects(GameObject object){
    //FIRST STEP: POSSIBLY GO THROUGH ALL GAMEOBJECTS THAT EXIST AND SEE IF ANY INTERSECT WITH OUR CAMERA?
    //std::cout << "HAS INTERACTION WITH: " << object._filename << ": " << SDL_HasIntersection(&_dest_rect, &object._dest_rect) << std::endl;
    return SDL_HasIntersection(&_dest_rect, &object._dest_rect);
}

void Camera::UpdatePosX(int diff_x){
    _x += diff_x;
    std::cout << "Updated X: " << _x << std::endl;  // Debug print

}

void Camera::UpdatePosY(int diff_y){
    _y += diff_y;
    std::cout << "Updated Y: " << _y << std::endl;  // Debug print

}

void Camera::Resize(GameObject& previewObject, GameObject& editorObject, int window_width, int window_height){
    // NEED TO CHANGE THE POSITIONS BASED ON DISTANCE AND SIZE
    // TO DO: Get Aspect ratio between gameObject and camera sizes
    
    // ISSUE: KEEPS EXPANDING INFINITLY BECAUSE OF THE LOOP IN MAIN
    
    // ISSUE: SOMETHING WITH THE TEXTURE IS MESSY NEEDS TO BE SIZED SO IT DOESNT LOOK STRETCHED
    // FIXED: THINK THE STRETCH IS OKAY NOW SINCE IT DEPENDS ON THE ASPECT RATIO OF THE PREVIEW WINDOW
    
    float aspectRatio_height = static_cast<float>(_height / (float)editorObject._height);
    float aspectRatio_width = static_cast<float>(_width / (float)editorObject._width);
    /*int distance_y = object._original_y - _y;
    int distance_x = object._original_x - _x;
    float aspectRatio_y = static_cast<float>(_height / (float)distance_y);
    float aspectRatio_x = static_cast<float>(_width / (float)distance_x);*/

    // gets the distance between object point and centre of game Camera in terms of width
    int distance_x = editorObject._x - (_x + static_cast<int>(_width / 2));
    
    // ratio between the preview window and the width of the camera
    float aspectRatio_x = static_cast<float>(window_width) / _width;
    
    // the new point for x on the preview window
    int new_x = static_cast<int>(window_width / 2) + (distance_x * aspectRatio_x);

    // gets the distance between object point and centre of game Camera in terms of height
    int distance_y = editorObject._y - (_y + static_cast<int>(_height / 2));
    
    // ratio between the preview window and the height of the camera
    float aspectRatio_y = static_cast<float>(window_height) / _height;
    
    // the new point for y on the preview window
    int new_y = static_cast<int>(window_height / 2) + (distance_y * aspectRatio_y);

    //object._width = static_cast<int>(window_width / aspectRatio_width);
    int width_difference = static_cast<int>(window_width / aspectRatio_width);
    
    //std::cout << "distance x: " << distance_x << " and distance y: " << distance_y << std::endl;
    //std::cout << "aspectRatio x: " << aspectRatio_x << " and aspectRatio y: " << aspectRatio_y << std::endl;
    //std::cout << "new x: " << new_x << " and new y: " << new_y << std::endl;
    
    previewObject._width = width_difference;
    previewObject._height = static_cast<int>(window_height / aspectRatio_height);
    //object._x = static_cast<int>(window_width / aspectRatio_x);
    //object._y = static_cast<int>(window_height / aspectRatio_y);
    previewObject._x = new_x;
    previewObject._y = new_y;
    previewObject._a = editorObject._a;

}