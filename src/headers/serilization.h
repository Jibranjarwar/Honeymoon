#pragma once

#include <vector>
#include "gameobject.h"
#include "gamescreen.h"
#include <string>
#include "json.hpp"

// setting alias
using json = nlohmann::json;

void tester(std::vector<GameObject> gameObjects, GameScreen* gamescreen);

json reader_tester();

json findGameObject(int id);