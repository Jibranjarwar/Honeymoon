#include "Sol/sol.hpp"
#include "LuaFunctions.h"
#include "gameobject.h"
#include <iostream>
#include <algorithm>

// gets gameObjects vector from Main.cpp
//extern std::vector<GameObject> gameObjects; 

// STORE IDS OF CHILD IN GAMEOBJECT THEN GO THROUGH GAMEOBJECT VECTOR AND FIND CHILD OBJECTS THAT WAY EASY

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

        // ISSUE: GLOBAL STATE BUT WE NEED FOR DIFFERENT STATES
        // FIX: we capture the reference in the lambda function
        "OnCollisionReturn", [&lua](GameObject& self) -> sol::table {
            sol::table collidedObject = self.collisionBox.Collision_Check_Lua(self, gameObjects, lua);
            return collidedObject.valid() ? collidedObject : sol::nil;
        },
        "UpdatePosX", &GameObject::UpdatePosX, 
        "UpdatePosY", &GameObject::UpdatePosY, 
        "Render", &GameObject::Render, 
        "GetID", &GameObject::GetID,

        // ISSUE: GLOBAL STATE BUT WE NEED FOR DIFFERENT STATES
        // FIX: we capture the reference in the lambda function
        "children", sol::property([&lua](GameObject& obj) {
        sol::table childrenTable = lua.create_table();
            for (int child_id : obj.childrenIDs) {
                // find obj in gameObjects
                //std::cout << "child id :" << child_id << std::endl;
                
                auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
                                                   [child_id](const GameObject &obj)
                                                   { return obj.GetID() == child_id; });
                if (it != gameObjects.end()) {
                    // store by name
                    childrenTable[it->_name] = &(*it);
                }else{
                    std::cout << "something wrong or didnt find anything" << std::endl;
                }
            }
            return childrenTable;
        }),
    
        // Properties
        "x", 
        sol::property(
            // lambda Getter
            [](GameObject& obj) { return obj._x; },            
            // lambda Setter
            [](GameObject& obj, int value) { 
                // makes sure that collision Box stays attached to gameObject how it was set in editor when changing position through Lua
                obj.collisionBox._x = (obj.collisionBox._x - obj._x) + value;
                //std::cout << "No children" << std::endl;
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
        // made it so it saves with name, obj pair since it would be hard to identify objects by ID too users
        gameObjectsTable[gameObjects[i]._name] = &gameObjects[i];
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

// Update key value pair in lua by finding old key and replacing it with new changed name so we can always access in Lua scripting
void UpdateKeyTableLua(sol::state& lua, std::string newName, GameObject& newGameObject){
    sol::table gameObjectsTable = lua["gameObjects"];
    std::string oldKey;
    for(auto& pair : gameObjectsTable){
        if(pair.second.is<GameObject*>()){
            GameObject* obj = pair.second.as<GameObject*>();
            if(obj == &newGameObject){
                std::cout << "print worked" << std::endl;
                oldKey = pair.first.as<std::string>();
                break;
            }
        }
    }

    if(!oldKey.empty()){
        gameObjectsTable[oldKey] = sol::nil;
    }

    gameObjectsTable[newName] = &newGameObject;
}