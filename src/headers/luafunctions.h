#pragma once

#include "Sol/sol.hpp"
#include "gameobject.h"
#include "gameobjectui.h"

// defines external gameObjects from Main.cpp
extern std::vector<GameObject> gameObjects;
extern sol::state global_lua;
extern std::vector<GameObjectUI> gameObjectsUI;

void RegisterGameObjectWithLua(sol::state& lua);
void RegisterGameObjectsWithLua(sol::state& lua, std::vector<GameObject>& gameObjects);
void AddGameObjectToLua(sol::state& lua, GameObject& newGameObject);
