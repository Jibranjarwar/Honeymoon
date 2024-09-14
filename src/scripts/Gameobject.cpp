#include <gameobject.h>
#include <iostream>

int GameObject::_current_id = 0;

// Constructor
GameObject::GameObject(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a):
_objRenderer(renderer), _width(width), _height(height), _x(x), _y(y), _r(r), _g(g), _b(b), _a(a)
{
_id = _current_id++;
}

// Another Optional Constructor
GameObject::GameObject(SDL_Renderer* renderer, std::string filename, int width, int height, int x, int y):
_objRenderer(renderer), _width(width), _height(height), _x(x), _y(y){
    _id = _current_id++;
    _objTexture = Texture(filename);
}

// Destructor
GameObject::~GameObject(){

}

// Creates a Layer for the GameObject and saves it too renderer
void GameObject::Render(){
    
    _dest_rect = { _x, _y, _width, _height };

    // checks if we created a Texture in constructor or not and depending on which is the way we render the object
    if(!_objTexture){    
        SDL_SetRenderDrawColor(_objRenderer, _r, _g, _b, _a);
        SDL_RenderFillRect(_objRenderer, &_dest_rect);
    }else{
        SDL_RenderCopy(_objRenderer, _objTexture, nullptr, &_dest_rect);
    }
}

SDL_Texture* GameObject::Texture(const std::string filename){
    
    // Loads PNG or JPG from existing file
    SDL_Surface* tempSurface = IMG_Load(filename.c_str());
    
    // Adds PNG or JPG on too texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(_objRenderer, tempSurface);
    
    // Frees the Surface from memory to prevent memory leak
    SDL_FreeSurface(tempSurface);
    
    return texture;
}