#include "collision.h"
#include "gameobject.h"
#include <iostream>

Collision::Collision(){
}

Collision::Collision(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a, int gameObject_screen_x, int gameObject_screen_y):
_objRenderer(renderer), _width(width), _original_w(width), _height(height), _original_h(height), _x(x), _original_x(x), _y(y), _original_y(y), _r(r), _g(g), _b(b), _a(a){
    _screen_x = gameObject_screen_x;
    _screen_y = gameObject_screen_y;
}

Collision::~Collision(){

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

void Collision::UpdatePosX(int diff_x){
    _x += diff_x;
}

void Collision::UpdatePosY(int diff_y){
    _y += diff_y;
}

GameObject* Collision::Collision_Check(GameObject& self, std::vector<GameObject>& gameObjects){
    
    for(int i = 0; i < gameObjects.size(); i++){
        if(gameObjects[i].addedCollision && self.GetID() != gameObjects[i].GetID()){
            if(SDL_HasIntersection(&self.collisionBox._dest_rect, &gameObjects[i].collisionBox._dest_rect)){
                std::cout << self.GetID() << " collided with " << gameObjects[i].GetID() << std::endl;
                return &gameObjects[i];
            }
        }else{
            continue;
        }
    }

    return nullptr;
}

void Collision::Del(GameObject* obj, std::vector<GameObject>& gameObjects, std::unordered_map<int, GameObject>& previewMap){
    auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
        [obj](const GameObject& g) { return &g == obj; });  // Find the object in vector
    
    if (it != gameObjects.end()) {
        previewMap.erase(it->GetID());
        gameObjects.erase(it);
        
    }
}

void Collision::On_Collision(GameObject& self, std::vector<GameObject>& gameObjects) {
    GameObject* collidedObject = Collision_Check(self, gameObjects);

    if(collidedObject != nullptr){
        std::cout << "ID: " << collidedObject->GetID() << " and self: " << self.GetID() << std::endl;
    }
    
    if (collidedObject) {
        SDL_Rect& selfBox = self.collisionBox._dest_rect;
        SDL_Rect& otherBox = collidedObject->collisionBox._dest_rect;

        // Calculate overlap (intersection) area
        int overlapLeft = otherBox.x + otherBox.w - selfBox.x;  // Overlap from left side
        int overlapRight = selfBox.x + selfBox.w - otherBox.x;  // Overlap from right side
        int overlapTop = otherBox.y + otherBox.h - selfBox.y;    // Overlap from top
        int overlapBottom = selfBox.y + selfBox.h - otherBox.y;  // Overlap from bottom

        // Determine the smallest overlap and resolve collision in that direction
        if (overlapRight > 0 && overlapRight > 0 && overlapTop > 0 && overlapBottom > 0) {
            
            if(overlapBottom < overlapTop && overlapBottom < overlapRight && overlapBottom < overlapLeft){
                self.UpdatePosAll_Y(-overlapBottom);
            }
            else if(overlapTop < overlapBottom && overlapTop < overlapLeft && overlapTop < overlapRight){
                self.UpdatePosAll_Y(overlapTop);
            }

            else if(overlapRight < overlapLeft && overlapRight < overlapBottom && overlapRight << overlapTop){
                std::cout << "value: " << overlapRight << std::endl;
                std::cout << "value left: " << overlapLeft << std::endl;  
                self.UpdatePosAll_X(-overlapRight);  // Push object left
            }
            else if(overlapLeft < overlapRight && overlapLeft << overlapBottom && overlapLeft << overlapTop){
                self.UpdatePosAll_X(overlapLeft);
            }
            
        }
    }
}
