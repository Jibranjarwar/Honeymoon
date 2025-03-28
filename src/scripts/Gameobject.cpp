#include "gameobject.h"
#include "serilization.h"
#include <iostream>

int GameObject::_current_id = 0;

// NOTE: THIS COULD HAVE POTENTIAL ISSUES WITH NULLPTR WITH NOT CHECKS
SDL_Event* GameObject::globalEvent = nullptr;

// Constructor

GameObject::GameObject(){
    _id = -1;
}

GameObject::GameObject(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a):
_objRenderer(renderer), _width(width), _height(height), _x(x), _y(y), _r(r), _g(g), _b(b), _a(a)
{
_id = _current_id++;
}

// Another Optional Constructor
GameObject::GameObject(SDL_Renderer* renderer, std::string filename, int width, int height, int x, int y):
_objRenderer(renderer), _width(width), _height(height), _x(x), _y(y){
    _id = _current_id++;
    _objTexture = Texture(filename, renderer=renderer);
    //_previewTexture = Texture(filename, renderer=renderer);
    _filename = filename;
    _original_w = width;
    _original_h = height;
    _original_x = x;
    _original_y = y;

    Setter();
}

//copy paste add attribute std::string name, new attribute name, 
GameObject::GameObject(SDL_Renderer* renderer, std::string filename, std::string name, int width, int height, int x, int y):
_objRenderer(renderer), _width(width), _height(height), _x(x), _y(y){
    _id = _current_id++;
    _objTexture = Texture(filename, renderer=renderer);
    //_previewTexture = Texture(filename, renderer=renderer);
    _filename = filename;
    _name = name; 
    _original_w = width;
    _original_h = height;
    _original_x = x;
    _original_y = y;
    _screen_x = x;
    _screen_y = y;
    Setter();
}
// Destructor
GameObject::~GameObject(){

}
/*SDL_Rect new_dest_rect = { _x, _y, _width, _height};
    /*SDL_Texture* preview_texture = Texture(_filename, preview_renderer);
    SDL_RenderCopy(preview_renderer, preview_texture, nullptr, &new_dest_rect);
    SDL_SetRenderDrawColor(preview_renderer, _r, _g, _b, _a);
    SDL_RenderFillRect(preview_renderer, &new_dest_rect);
*/

void GameObject::RenderPreview(SDL_Renderer* preview_renderer, int offset_x, int offset_y){
    SDL_Rect new_dest_rect = { _x, _y, _width, _height};
    std::cout << _width << std::endl;
    //SDL_Texture* preview_texture = Texture(_filename, preview_renderer);
    
    // THIS WAS THE ISSUE WITH SECOND WINDOW DOWN PERFORMANCE BECAUSE FOR SOME
    // GOD KNOWS REASON I THOUGHT IT WOULD BE A GOOD IDEA TO LOAD A NEW TEXTURE EACH TIME
    // WE RENDER THE OBJECT WHEN IN THEORY IS THE WORST THING YOU CAN DO SO THIS IF STATEMENT
    // FIXED IT
    if(_previewTexture == nullptr){
        _previewTexture = Texture(_filename, preview_renderer);
    }
    SDL_RenderCopy(preview_renderer, _previewTexture, nullptr, &new_dest_rect);
    //SDL_SetRenderDrawColor(preview_renderer, _r, _g, _b, _a);
    //SDL_RenderFillRect(preview_renderer, &new_dest_rect);
}

// Creates a Layer for the GameObject and saves it too renderer
void GameObject::Render(int gridMinX, int gridMinY, int gridMaxX, int gridMaxY){
    
    _dest_rect = { _x, _y, _width, _height };
    SDL_Rect _src_rect = {0, 0, 0, 0};
    SDL_Rect new_rect = _dest_rect;
    int texture_width, texture_height;

    SDL_QueryTexture(_objTexture, nullptr, nullptr, &texture_width, &texture_height);


    // set size of src_rect to texture size so we keep orignal aspect ratio of _dest_rect
    if(!((_y + _height > gridMaxY && _y < gridMinY) || (_x + _width > gridMaxX && _x < gridMinX))){
        _src_rect.w = texture_width;
        _src_rect.h = texture_height;
    }
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
        if(_x + _width <= gridMaxX){
            new_rect.x = gridMinX;
            new_rect.w = _width - (gridMinX - _x);
            
            /* more math for cutting textures with not just pure colour.
            since _dest_rect fits the texture too size we need to actually get the original size of texture
            so we _src_rect doesnt change in size and keeps same aspect ratio that dest_rect gave and basically
            just multiply new_rect by the aspect ratio and set the x co-ordinates to locate from where we clip */
            float aspectRatio_x = static_cast<float>(texture_width / (float)_width);
            _src_rect.x = texture_width - (new_rect.w * aspectRatio_x);
        }
    }

    // everything here is same logic as for gridMaxY just switched around for X
    if(_x + _width > gridMaxX){
        //std::cout << "went past right part" << std::endl;
        if(_x >= gridMinX){
            new_rect.w = _width - ((_x + _width) - gridMaxX);

            float aspectRatio_x = static_cast<float>(texture_width / (float)_width);
            _src_rect.x = -1 * (texture_width - (new_rect.w * aspectRatio_x));
        }
    }

    // everything here is same logic as for gridMinX just switched around for Y
    if(_y < gridMinY){
        //std::cout << "went past top part" << std::endl;
        if(_y + _height < gridMaxY){
                
            new_rect.y = gridMinY;
            new_rect.h = _height - (gridMinY - _y);

            float aspectRatio_y = static_cast<float>(texture_height / (float)_height);
            _src_rect.y = texture_height - (new_rect.h * aspectRatio_y);
        }

        //std::cout << "new rect height: " << _src_rect.h << std::endl;
    }

    if(_y + _height > gridMaxY){        
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
        if(_y >= gridMinY){
            new_rect.h = _height - ((_y + _height) - gridMaxY);
            /* more math for cutting textures with not just pure colour.
            since _dest_rect fits the texture too size we need to actually get the original size of texture
            so we _src_rect doesnt change in size and keeps same aspect ratio that dest_rect gave and basically
            just multiply new_rect by the aspect ratio and set the y co-ordinates to locate from where we clip */
            float aspectRatio_y = static_cast<float>(texture_height / (float)_height);
            _src_rect.y = -1 * (texture_height - (new_rect.h * aspectRatio_y));
        }
    }

    if(_y + _height > gridMaxY && _y < gridMinY){
        // this shoyld be the logic if the top and bottom are touching
        new_rect.y = gridMinY;
        new_rect.h = gridMaxY;
        float aspectRatio_y = static_cast<float>(texture_height / (float)_height);
        
        // keep the texture_width so that we dont stretch
        if(!(_x + _width > gridMaxX && _x < gridMinX)){
            _src_rect.w = texture_width;
        }

        // the cut size is basically the difference between gridMinY and _y but since gridMinY is 0 we just use _y as the distance
        // value and multiply it by -1 since its in the minus range
        _src_rect.y = (-1 * _y) * aspectRatio_y;

        // this logic is hard to explain in words so that its short so theres a example.png that gives the general gist
        _src_rect.h = texture_height - (((gridMinY - _y) + (_y + _height - gridMaxY)) * aspectRatio_y);
    }

    if(_x + _width > gridMaxX && _x < gridMinX){
        new_rect.x = gridMinX;
        
        // we cant just set it as gridMaxX as above we did with gridMaxY since the value isnt 0 
        // since we dont use the full window screen for the gameScreen but like 2/3 of it
        new_rect.w = gridMaxX - gridMinX;
        float aspectRatio_x = static_cast<float>(texture_width / (float)_width);

        if(!(_y + _height > gridMaxY && _y < gridMinY)){
            _src_rect.h = texture_height;
        }
        
        _src_rect.x = (gridMinX - _x) * aspectRatio_x;

        _src_rect.w = texture_width - (((gridMinX - _x) + (_x + _width - gridMaxX)) * aspectRatio_x);
    }


    // checks if we created a Texture in constructor or not and depending on which is the way we render the object
    if(!_objTexture){    
        SDL_SetRenderDrawColor(_objRenderer, _r, _g, _b, _a);
        SDL_RenderFillRect(_objRenderer, &_dest_rect);
    }else{
        // change this back to id = 0 if needed right now its 6 because doesnt exist
        if(_x < gridMinX || _x + _width > gridMaxX || _y < gridMinY || _y + _height > gridMaxY){
            
            // checks wether the game object is within the gamescreen or not and if not then dont call function to save performance 
            if(new_rect.w > 0 && new_rect.h > 0){
                SDL_RenderCopyEx(_objRenderer, _objTexture, &_src_rect , &new_rect, 0, nullptr , SDL_FLIP_NONE);
            }else{
                //std::cout << "gone off screen" << std::endl;
            }
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

// Movement function for Lua which doesnt take parameter since we pass event as a global static variable saved in GameObject class
void GameObject::Lua_Movement(){
    Movement(*globalEvent);
}

void GameObject::setEvent(SDL_Event* event){
    globalEvent = event;
}

void GameObject::UpdatePosX(int diff_x){
    _x += diff_x;
    std::cout << "Updated X: " << _x << std::endl;  // Debug print

}

void GameObject::UpdatePosY(int diff_y){
    _y += diff_y;
    std::cout << "Updated Y: " << _y << std::endl;  // Debug print

}

SDL_Texture* GameObject::Texture(const std::string filename, SDL_Renderer* renderer = nullptr){
    
    // Loads PNG or JPG from existing file
    SDL_Surface* tempSurface = IMG_Load(filename.c_str());
    
    // Adds PNG or JPG on too texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    // Frees the Surface from memory to prevent memory leak
    SDL_FreeSurface(tempSurface);
    
    return texture;
}

// TO DO: NEED TO FIX DATA SERILIZATION SO CAN STORE AND FETCH ORIGINAL VALUES
void GameObject::Setter(){
    json something = findGameObject(_id);
    std::cout << "id: " << _id << " " << something.dump(4) << std::endl;
}