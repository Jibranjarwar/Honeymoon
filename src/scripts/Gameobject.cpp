#include <gameobject.h>
#include <iostream>

int GameObject::_current_id = 0;

// Constructor
GameObject::GameObject(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a):
_objRenderer(renderer), _width(width), _height(height), _x(x), _y(y), _r(r), _g(g), _b(b), _a(a){
_id = _current_id++;
}

// Destructor
GameObject::~GameObject(){

}

// Creates a Layer for the GameObject and saves it too renderer
void GameObject::Render(){
    _dest_rect = { _x, _y, _width, _height };
    SDL_SetRenderDrawColor(_objRenderer, _r, _g, _b, _a);
    SDL_RenderFillRect(_objRenderer, &_dest_rect);
}