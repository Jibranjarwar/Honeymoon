#include "gameobject.h"
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
    _original_w = width;
    _original_h = height;
}

// Destructor
GameObject::~GameObject(){

}

// Creates a Layer for the GameObject and saves it too renderer
void GameObject::Render(int gridMinX, int gridMinY, int gridMaxX, int gridMaxY){
    
    _dest_rect = { _x, _y, _width, _height };
    SDL_Rect _src_rect = {0, 0, 0, 0};
    SDL_Rect new_rect = _dest_rect;
    int texture_width, texture_height;

    SDL_QueryTexture(_objTexture, nullptr, nullptr, &texture_width, &texture_height);

    if(_id == 0){

        // set size of src_rect to texture size so we keep orignal aspect ratio of _dest_rect
        _src_rect.w = texture_width;
        _src_rect.h = texture_height;
        // this is for X axis only
        if(_x < gridMinX){
            //std::cout << "gone beyond left grid side" << std::endl;
            //std::cout << "x cord:" << std::to_string(_x) << std::endl;
            //std::cout << "the width: " << _width - (gridMinX - _x) << std::endl;

            /*create new rectangle which takes away from the old rectangle to get the cut size needed to draw from the side so
            
            all of this is old rect
            
            ======================
            =        =           =
            =  part  =  new rect =
            =  that  =           =
            =  cuts  =           =
            ======================
            
            */
            
            // this is the maths part for the diagram above
            new_rect.x = gridMinX;
            new_rect.w = _width - (gridMinX - _x);
            
            /* more math for cutting textures with not just pure colour.
            since _dest_rect fits the texture too size we need to actually get the original size of texture
            so we _src_rect doesnt change in size and keeps same aspect ratio that dest_rect gave and basically
            just multiply new_rect by the aspect ratio and set the x co-ordinates to locate from where we clip */
            float aspectRatio_x = static_cast<float>(texture_width / (float)_width);
            _src_rect.x = texture_width - (new_rect.w * aspectRatio_x);
        }

        if(_y + _height + 10 > gridMaxY){        
            //std::cout << "gone beyond bottom of grid side" << std::endl;
            //std::cout << "y cord:" << std::to_string(_y) << std::endl;


            /*create new rectangle which takes away from the old rectangle to get the cut size needed to draw from the bottom so
            
            all of this is old rect
            
            ======================
            =                    =
            =     new rect       =
            =                    =
            =                    =
            =                    =
            ======================
            =  part that gets    =
            =      cut           =
            ======================
            
            */
            // this is the maths part for the diagram above
            new_rect.h = _height - ((_y + _height + 10) - gridMaxY);

            /* more math for cutting textures with not just pure colour.
            since _dest_rect fits the texture too size we need to actually get the original size of texture
            so we _src_rect doesnt change in size and keeps same aspect ratio that dest_rect gave and basically
            just multiply new_rect by the aspect ratio and set the y co-ordinates to locate from where we clip */
            float aspectRatio_y = static_cast<float>(texture_height / (float)_height);
            _src_rect.y = -1 * (texture_height - (new_rect.h * aspectRatio_y));
        }
    }

    // checks if we created a Texture in constructor or not and depending on which is the way we render the object
    if(!_objTexture){    
        SDL_SetRenderDrawColor(_objRenderer, _r, _g, _b, _a);
        SDL_RenderFillRect(_objRenderer, &_dest_rect);
    }else{
        // change this back to id = 0 if needed right now its 6 because doesnt exist
        if(_id == 0 && (_x < gridMinX || _y + _height + 10 > gridMaxY)){
            SDL_RenderCopyEx(_objRenderer, _objTexture, &_src_rect , &new_rect, 0, nullptr , SDL_FLIP_NONE);
        }else{
            SDL_RenderCopy(_objRenderer, _objTexture, nullptr, &_dest_rect);
        }
    }
}

// Movement for GameObject just to test game loop at the moment and rendering (can be updated later)
void GameObject::Movement(SDL_Event &event){
    switch(event.key.keysym.sym){
        case SDLK_d:
            _x += 10;
            break;
        case SDLK_a:
            _x -= 10;
            break;
        case SDLK_w:
            _y -= 10;
            break;
        case SDLK_s:
            _y += 10;
            break; 
    }
}

void GameObject::UpdatePos(int new_x, int new_y){
    _x -= new_x;
    std::cout << _x << std::endl;
    _y -= new_y;
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