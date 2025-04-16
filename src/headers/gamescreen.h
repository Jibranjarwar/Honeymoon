#pragma once

#include <SDL2/SDL.h>
//#include "gameobject.h"
#include "camera.h"
#include <vector>
#include "json.hpp"

using json = nlohmann::json;
class GameObject;

class GameScreen{

private:
    GameObject* selectedGameObject = nullptr;

public:
    void SetSelectedGameObject(GameObject* obj) { selectedGameObject = obj; }
    GameObject* GetSelectedGameObject() { return selectedGameObject; }

public:

    GameScreen(SDL_Renderer* renderer);

    ~GameScreen();

    void DrawGraph(SDL_Window *window);
    
    bool Zoomed(SDL_Event event);
    
    void ZoomInAndOut(SDL_Event event, std::vector<GameObject>& array,  std::vector<Camera>& cameras);
    
    // TO DO: MAKE THIS SO THAT IT CAN TAKE GAMEOBJECT AND CAMERA CLASSES
    // ISSUE: DOESNT LET ME USE TYPENAME T CAUSE OF SOME REASION BUT ALLOWS IT FOR DRAGSTATE
    // DONE: FIXED ISSUE WITH TYPENAME T BUT ONLY ONE FUNCTION WAS BEEN CALLED SO INSTEAD JUST ADDED ANOTHER PARAMETER
    //       FOR CAMERA OBJECT ARRAY.

    void InitalDragState(SDL_Event event, std::vector<GameObject>& array, std::vector<Camera>& other_array);

    void DragScreen(Uint32 mouseState, std::vector<GameObject>& array, std::vector<Camera>& other_array);
    
    void ScreenOffset();

    void CreateInitialMatrix(SDL_Renderer* renderer, std::string filename, std::string name, int width, int height, int x, int y);

    void UpdateMatrix(int x_position, int y_position);

    bool WindowChangeCheck(int window_width, int window_height);

    struct Vector2 {
        float x;
        float y;

        Vector2(float x = 0, float y = 0) : x(x), y(y) {}
    };

    void Setter(json data);



public:
    SDL_Renderer* renderer;
    int mouse_wheel_y = 0;
    int window_width, window_height;
    int prev_window_width = 0, prev_window_height = 0;
    int temp_offset_height;
    int offset_height_rounded;
    int offset_height;
    int offset_width;
    int current_window_position_x = 0, current_window_position_y = 0;
    int mouse_wheel_max = 1;
    float zoomfactor = 1.0f;
    float zoomed_offset_x, zoomed_offset_y;
    float viewport_offset_x = 0.0f, viewport_offset_y = 0.0f;
    int prev_drag_window_position_x, prev_drag_window_position_y;
    int window_x, window_y;
    int screen_x, screen_y;
    int inital_drag_window_x, inital_drag_window_y;
    static int difference_x, difference_y;
    int width_difference = 0, height_difference = 0;
    bool wasPressed = false;
    int count;
    // needs to be a pointer since we use it in GameObject cpp which hasnt finished compiling
    static GameObject* InitialMatrix;
};