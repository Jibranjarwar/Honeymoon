#include "Sol/sol.hpp"
#include "LuaFunctions.h"
#include "gameobject.h"
#include <iostream>

void RegisterGameObjectWithLua(sol::state &lua) {
    lua.new_usertype<GameObject>("GameObject", 
        sol::constructors<
            GameObject(SDL_Renderer*, int, int, int, int, int, int, int, int),
            GameObject(SDL_Renderer*, std::string, int, int, int, int),
            GameObject(SDL_Renderer*, std::string, std::string, int, int, int, int)
        >(),
        
        "Movement", &GameObject::Movement, 
        "UpdatePosX", &GameObject::UpdatePosX, 
        "UpdatePosY", &GameObject::UpdatePosY, 
        "Render", &GameObject::Render, 
        "GetID", &GameObject::GetID,
    
        // Properties
        "x", &GameObject::_x, 
        "y", &GameObject::_y, 
        "width", &GameObject::_width, 
        "height", &GameObject::_height, 
        "name", &GameObject::_name, 
        "screen_x", &GameObject::_screen_x, 
        "screen_y", &GameObject::_screen_y
    );
}

void RegisterGameObjectsWithLua(sol::state& lua, std::vector<GameObject>& gameObjects) {
    
    sol::table gameObjectsTable = lua.create_table();
    for (size_t i = 0; i < gameObjects.size(); ++i) {
        gameObjectsTable[i + 1] = &gameObjects[i]; 
    }
    lua["gameObjects"] = gameObjectsTable; 
}