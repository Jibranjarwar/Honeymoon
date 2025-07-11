#include "Sol/sol.hpp"
#include <SDL2/SDL.h>
#include "LuaFunctions.h"
#include "gameobject.h"
#include <iostream>
#include <algorithm>
#include "gamescreen.h"
#include <chrono>

using Clock = std::chrono::steady_clock;

// gets gameObjects vector from Main.cpp
//extern std::vector<GameObject> gameObjects; 

// STORE IDS OF CHILD IN GAMEOBJECT THEN GO THROUGH GAMEOBJECT VECTOR AND FIND CHILD OBJECTS THAT WAY EASY

// creates a type for Lua to understand GameObject class
void RegisterGameObjectWithLua(sol::state &lua) {
    
    lua.new_usertype<Camera>("Camera",
        "x", 
        sol::property(
            [](Camera& obj) { return obj._x - GameScreen::InitialMatrix->_x; },
            [](Camera& obj, int value) {
                int desired_x = GameScreen::InitialMatrix->_x + value;
                obj._x = desired_x;
            }
        ),
        "y",
        sol::property(
            [](Camera& obj) { return (obj._y - GameScreen::InitialMatrix->_y) * -1; },
            [](Camera& obj, int value) {
                int desired_y = GameScreen::InitialMatrix->_y + (-value);
                obj._y = desired_y;
            }
        ),
        "width", &Camera::_width,
        "height", &Camera::_height,
        "FollowObject", [](Camera& self, GameObject& obj) {
            // Center of the object
            int obj_center_x = obj._x + (obj._width / 2);
            int obj_center_y = obj._y + (obj._height / 2);

            // Camera's top-left should be moved to center on obj center
            self._x = obj_center_x - (self._width / 2);
            self._y = obj_center_y - (self._height / 2);
        }
    );

    lua.new_usertype<GameObject>("GameObject", 
        
        "Copy", [&lua](GameObject& self) -> GameObject* {
            GameObject copy = self.Copy();
            gameObjects.push_back(copy);
            //RegisterGameObjectsWithLua(lua, gameObjects);
            std::cout << copy._name << " and " << copy.GetID() << std::endl;
            deleteObjects.push_back(copy);

            auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
                                                   [&](const GameObject &obj)
                                                   { return obj.GetID() == copy.GetID(); });
            
            if(it != gameObjects.end()){
                return &(*it);
            }else{
                return nullptr;
            }
        },

        "Collide", [](GameObject& self){
            self.collisionBox.On_Collision(self, gameObjects);
        },
        "Movement", &GameObject::Lua_Movement,
        "OnCollision", [](GameObject& self) -> bool {
            return self.collisionBox.Collision_Check_Bool(self, gameObjects);
        },
        "AddCollision", [](GameObject& self){
            self.addedCollision = true;
            self.AddCollision(self._objRenderer);
        },

        // ISSUE: GLOBAL STATE BUT WE NEED FOR DIFFERENT STATES
        // FIX: we capture the reference in the lambda function
        // not working feature because causes lua to not go beyond calling this function
        
        /*"OnCollisionReturn", [&lua](GameObject& self) -> sol::table {
            sol::table collidedObject = self.collisionBox.Collision_Check_Lua(self, gameObjects, lua);
            return collidedObject.valid() ? collidedObject : sol::nil;
        },*/

        // working feature that returns ref to gameObject 
        "OnCollisionReturn", [](GameObject& self) -> GameObject* {
            return self.collisionBox.Collision_Check(self, gameObjects);
        },
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
            [](GameObject& obj) { return obj._x - GameScreen::InitialMatrix->_x; },            
            // lambda Setter
            [](GameObject& obj, int value) { 
                
                int desired_x = GameScreen::InitialMatrix->_x + value;
                // makes sure that collision Box stays attached to gameObject how it was set in editor when changing position through Lua
                obj.collisionBox._x += (desired_x - obj._x);
                //obj.collisionBox._x = (obj.collisionBox._x - obj._x) + value;
                //std::cout << "No children" << std::endl;
                obj._x = desired_x; 
            }
        ), 
        "y",
        sol::property(
            // lambda Getter
            [](GameObject& obj) { return (obj._y - GameScreen::InitialMatrix->_y) * -1; },   
            
            // lambda Setter
            [](GameObject& obj, int value) { 
                
                int desired_y = GameScreen::InitialMatrix->_y + (-value);
                // makes sure that collision Box stays attached to gameObject how it was set in editor when changing position through Lua
                obj.collisionBox._y += (desired_y - obj._y);
                //obj.collisionBox._y = (obj.collisionBox._y - obj._y) + value;
                obj._y = desired_y; 
            }
        ), 
        "width", &GameObject::_width, 
        "height", &GameObject::_height, 
        "name", &GameObject::_name,
        "oppacity", &GameObject::_a

    );

    RegisterLuaFunctions(lua);
}

// registers gameObject vector as a table for Lua
void RegisterGameObjectsWithLua(sol::state& lua, std::vector<GameObject>& gameObjects, Camera& camera) {

    sol::table gameObjectsTable = lua.create_table();
    for (size_t i = 0; i < gameObjects.size(); ++i) {
        // made it so it saves with name, obj pair since it would be hard to identify objects by ID too users
        gameObjectsTable[gameObjects[i]._name] = &gameObjects[i];
    }
    lua["gameObjects"] = gameObjectsTable;
    lua["Camera"] = &camera;
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

void RegisterLuaFunctions(sol::state &lua){
    lua["A_key"] = SDLK_a;
    lua["D_key"] = SDLK_d;
    lua["W_key"] = SDLK_w;
    lua["S_key"] = SDLK_s;
    lua["Q_key"] = SDLK_q;
    lua["E_key"] = SDLK_e;
    lua["F_key"] = SDLK_f;
    lua["C_key"] = SDLK_c;
    lua["LEFT_key"] = SDLK_LEFT;
    lua["RIGHT_key"] = SDLK_RIGHT;
    lua["UP_key"] = SDLK_UP;
    lua["DOWN_key"] = SDLK_DOWN;
    lua["SPACE_key"] = SDLK_SPACE;

    // converts keycode to scancode since I didnt want to write both over 
    lua["KeycodeToScancode"] = [](SDL_Keycode keycode) {
        return SDL_GetScancodeFromKey(keycode);
    };

    // if key is only pressed
    lua["IsKeyPressed"] = [](SDL_Keycode key) {
        return GameObject::globalEvent && 
               GameObject::globalEvent->type == SDL_KEYDOWN &&
               GameObject::globalEvent->key.keysym.sym == key;
    };

    // if key is held down
    lua["IsKeyHeld"] = [](SDL_Scancode scancode) {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        return state[scancode] != 0;
    };

    lua.set_function("DeleteGameObjects", [&lua](sol::table toDelete){
        std::vector<int> idsToRemove;
        //std::cout << "work" << std::endl;

        // goes through table and adds gameObject ids that need to be deleted
        for(auto& item_pair : toDelete ){
            GameObject* obj = item_pair.second.as<GameObject*>();
            //std::cout << "gameObject name: " << obj->_name << std::endl;
            if(obj->_lua_id != -1){
                std::cout << "obj:" << obj->_name << std::endl;
                idsToRemove.push_back(obj->_lua_id);
            }
        }

        // removes gameObjects from gameObject vector
        gameObjects.erase(
            std::remove_if(gameObjects.begin(), gameObjects.end(), [&](const GameObject& obj) {
                return std::find(idsToRemove.begin(), idsToRemove.end(), obj._lua_id) != idsToRemove.end();
            }), gameObjects.end());
        
        for(auto lua_id : idsToRemove){
            for(auto& pair : gameObjectsCopy){
                //std::cout << "name: " << pair.second._name << std::endl;
                if(pair.second._lua_id == lua_id){
                    gameObjectsCopy.erase(pair.second.GetID());
                    break;  
                }
            }
        }
        
        // updates lua table after deleting gameObjects (might not work while running mid state)
        sol::table updatedTable = lua.create_table();
        for (const auto& obj : gameObjects) {
            updatedTable[obj._name] = const_cast<GameObject*>(&obj);
        }
        lua["gameObjects"] = updatedTable;
        idsToRemove.clear();
    });

    lua.set_function("Delay", [](float delaySeconds){
        static auto lastTime = Clock::now();
        auto now = Clock::now();
        std::chrono::duration<float> elapsed = now - lastTime;

        if (elapsed.count() >= delaySeconds) {
            lastTime = now;
            return true;
        }

        return false;
    });
}