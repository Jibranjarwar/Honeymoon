#pragma once

#include <SDL2/SDL.h>
#include "gameobject.h"
#include <vector>

class GameScreen{

public:
    void DrawGraph(SDL_Window *window);
    
    bool Zoomed(SDL_Event event);
    
    void ZoomInAndOut(SDL_Event event, std::vector<GameObject>& array);
    
    void InitalDragState(SDL_Event event, std::vector<GameObject>& array);
    
    void DragScreen(Uint32 mouseState, std::vector<GameObject>& array);
    
    void ScreenOffset();


public:
    int mouse_wheel_y = 0;
    int window_width, window_height;
    int temp_offset_height;
    int offset_height_rounded;
    int offset_height;
    int offset_width;
    int current_window_position_x = 0, current_window_position_y = 0;
    int mouse_wheel_max = 1;
    float zoomfactor = 0.0f;
    int prev_drag_window_position_x, prev_drag_window_position_y;
    int window_x, window_y;
    int screen_x, screen_y;
    int inital_drag_window_x, inital_drag_window_y;
    int difference_x = 0, difference_y = 0;
    int width_difference = 0, height_difference = 0;
    bool wasPressed = false;
};