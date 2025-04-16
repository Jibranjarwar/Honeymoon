#pragma once

#include <vector>
#include "gameobject.h"
#include "gamescreen.h"
#include "camera.h"
#include <string>
#include "json.hpp"

// setting alias
using json = nlohmann::json;

void tester(std::vector<GameObject> gameObjects, std::vector<Camera> cameraObjects, GameScreen* gamescreen);

json reader_tester();

json findGameObject(int id);