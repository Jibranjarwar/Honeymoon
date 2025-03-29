#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>

// forward declaration too avoid compile error
class GameObject;

class Collision{

public:
    Collision();
    Collision(SDL_Renderer* renderer, int width, int height, int x, int y, int r, int g, int b, int a, int gameObject_screen_x = 0, int gameObject_screen_y = 0);
    ~Collision();
    void Collision_Render(int thickness, int gridMinX, int gridMinY, int gridMaxX, int gridMaxY);
    inline int getX() const { return _x; }
    inline int getY() const { return _y; }
    void UpdatePosX(int diff_x);   
    void UpdatePosY(int diff_y);
    GameObject* Collision_Check(GameObject& self, std::vector<GameObject>& gameObjects);
    void On_Collision(GameObject& self, std::vector<GameObject>& gameObjects);
    void Del(GameObject* obj, std::vector<GameObject>& gameObjects, std::unordered_map<int, GameObject>& previewMap);

public:
    SDL_Rect _dest_rect;
    SDL_Renderer* _objRenderer;
    int _x, _y;
    int _original_x, _original_y;
    int _original_w, _original_h;
    int _screen_x, _screen_y;
    int _width, _height;
    int _r, _g, _b, _a;
};