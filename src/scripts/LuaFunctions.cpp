#include "Sol/sol.hpp"
#include "LuaFunctions.h"
#include "gameobject.h"
#include <iostream>

// gets gameObjects vector from Main.cpp
//extern std::vector<GameObject> gameObjects; 

// creates a type for Lua to understand GameObject class
void RegisterGameObjectWithLua(sol::state &lua) {
    lua.new_usertype<GameObject>("GameObject", 
        sol::constructors<
            GameObject(SDL_Renderer*, int, int, int, int, int, int, int, int),
            GameObject(SDL_Renderer*, std::string, int, int, int, int),
            GameObject(SDL_Renderer*, std::string, std::string, int, int, int, int)
        >(),
        
        "Movement", &GameObject::Lua_Movement,
        "OnCollision", [](GameObject& self) -> bool {
            return self.collisionBox.Collision_Check_Bool(self, gameObjects);
        },
        "UpdatePosX", &GameObject::UpdatePosX, 
        "UpdatePosY", &GameObject::UpdatePosY, 
        "Render", &GameObject::Render, 
        "GetID", &GameObject::GetID,
    
        // Properties
        "x", 
        sol::property(
            // lambda Getter
            [](GameObject& obj) { return obj._x; },   
            
            // lambda Setter
            [](GameObject& obj, int value) { 
                // makes sure that collision Box stays attached to gameObject how it was set in editor when changing position through Lua
                obj.collisionBox._x = (obj.collisionBox._x - obj._x) + value;
                obj._x = value; 
            }
        ), 
        "y",
        sol::property(
            // lambda Getter
            [](GameObject& obj) { return obj._y; },   
            
            // lambda Setter
            [](GameObject& obj, int value) { 
                // makes sure that collision Box stays attached to gameObject how it was set in editor when changing position through Lua
                obj.collisionBox._y = (obj.collisionBox._y - obj._y) + value;
                obj._y = value; 
            }
        ), 
        "width", &GameObject::_width, 
        "height", &GameObject::_height, 
        "name", &GameObject::_name, 
        "screen_x", &GameObject::_screen_x, 
        "screen_y", &GameObject::_screen_y
    );
}

// registers gameObject vector as a table for Lua
void RegisterGameObjectsWithLua(sol::state& lua, std::vector<GameObject>& gameObjects) {
    
    sol::table gameObjectsTable = lua.create_table();
    for (size_t i = 0; i < gameObjects.size(); ++i) {
        gameObjectsTable[i + 1] = &gameObjects[i]; 
    }
    lua["gameObjects"] = gameObjectsTable; 
}

// adds New instances of GameObject to Lua table
// ISSUE: CAUSES CRASH BECAUSE OF VECTOR REALOCATION 
void AddGameObjectToLua(sol::state& lua, GameObject& newGameObject) {
    sol::table gameObjectsTable = lua["gameObjects"];
    size_t newIndex = gameObjectsTable.size() + 1;
    gameObjectsTable[newIndex] = &newGameObject;
}