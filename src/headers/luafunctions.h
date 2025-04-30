#pragma once

#include "Sol/sol.hpp"
#include "gameobject.h"
#include "gameobjectui.h"
#include "camera.h"

// defines external gameObjects from Main.cpp
extern std::vector<GameObject> gameObjects;
extern sol::state global_lua;
extern std::vector<GameObjectUI> gameObjectsUI;
extern std::vector<GameObject> deleteObjects;
extern std::unordered_map<int, GameObject> gameObjectsCopy;

void RegisterGameObjectWithLua(sol::state& lua);
void RegisterGameObjectsWithLua(sol::state& lua, std::vector<GameObject>& gameObjects, Camera& camera);
void AddGameObjectToLua(sol::state& lua, GameObject& newGameObject);
void UpdateKeyTableLua(sol::state& lua, std::string newName, GameObject& newGameObject);
void RegisterLuaFunctions(sol::state &lua);
