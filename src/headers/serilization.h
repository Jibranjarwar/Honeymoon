#pragma once

#include <vector>
#include "gameobject.h"
#include "gamescreen.h"
#include "camera.h"
#include <string>
#include "json.hpp"

// setting alias
using json = nlohmann::json;

void SaveState(std::string projectName, std::vector<GameObject> gameObjects, std::vector<Camera> cameraObjects);

json LoadState(std::string project);

void rename_file(std::string project, std::string newName);

json findGameObject(int id);