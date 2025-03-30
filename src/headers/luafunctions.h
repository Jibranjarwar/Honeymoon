#pragma once

#include "Sol/sol.hpp"
#include "gameobject.h"

// defines external gameObjects from Main.cpp
extern std::vector<GameObject> gameObjects;

void RegisterGameObjectWithLua(sol::state& lua);
void RegisterGameObjectsWithLua(sol::state& lua, std::vector<GameObject>& gameObjects);
void AddGameObjectToLua(sol::state& lua, GameObject& newGameObject);
