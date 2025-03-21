#ifndef GAMEOBJECTUI_H
#define GAMEOBJECTUI_H

#include "gameobject.h"
#include <vector>

struct GameObjectUI {
    GameObject name;
    std::vector<GameObject> children;
};

#endif