#include "collision.h"

Collision::Collision(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a):
_objRenderer(renderer), _width(width), _original_w(width), _height(height), _original_h(height), _x(x), _original_x(x), _y(y), _original_y(y), _r(r), _g(g), _b(b), _a(a){
}

void Collision::Collision_Render(int thickness, int gridMinX, int gridMinY, int gridMaxX, int gridMaxY){
    
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