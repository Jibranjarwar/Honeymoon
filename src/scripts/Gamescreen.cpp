#include "gamescreen.h"
#include "window.h"
#include <iostream>

void GameScreen::DrawGraph(SDL_Window *window){
    
    // gets the window width and height and assigns it
    SDL_GetWindowSize(window, &window_width, &window_height);

    // sets colour to white for the lines
    SDL_SetRenderDrawColor(Window::renderer, 255, 255, 255, 255);

    // rounds to the next 10th e.g 453 -> 460 or e.g 337 -> 340
    temp_offset_height = (int)(window_height / 3.5);
    offset_height_rounded = 10 - (temp_offset_height % 10);
    offset_height = temp_offset_height + offset_height_rounded;
    //offset_height = (int)(window_height / 2);

    offset_width = (int)(window_width / 1.25);

    // creates grid 
    for(int w = window_width - offset_width; w <= window_width; w += 10 + mouse_wheel_y){
        for(int h = 0; h <= window_height - offset_height; h += 10 + mouse_wheel_y){
    
            SDL_RenderDrawLine(Window::renderer, w, h, window_width, h);
            SDL_RenderDrawLine(Window::renderer, w, h, w, offset_height);
            width_difference = w;
            height_difference = h;
        }    
    }

    if(width_difference < window_width && height_difference < window_height){
        
        for(int w = window_width - offset_width; w <= window_width; w += 10 + mouse_wheel_y){
            SDL_RenderDrawLine(Window::renderer, w, height_difference, w, window_height - offset_height);
        }
    }
    SDL_RenderDrawLine(Window::renderer, (window_width - offset_width), (window_height - offset_height), window_width, (window_height - offset_height));
}

bool GameScreen::Zoomed(SDL_Event event){
    switch(event.type)
    {
        case SDL_MOUSEWHEEL:
        {
            std::cout << "the mouse is zoom" << std::endl;
            return true;
        }
    }
    return false;
}

void GameScreen::ZoomInAndOut(SDL_Event event, GameObject* player){

    if(Zoomed(event)){
        // changes value of zoom but never goes lower than -3 because then we need to draw to many lines and program crashes
        if(mouse_wheel_y + event.wheel.y > -4){
            mouse_wheel_y += event.wheel.y;
            
            if(mouse_wheel_y > mouse_wheel_max){
                mouse_wheel_max = mouse_wheel_y;
            }
        }

        /* 
        This if statement is after assignment so that when we zoom out for the first time it registers it unlike
        if this was before the assignment where it would need to go again for it to register properly.
        */
        if(mouse_wheel_y >= 0){
            if(mouse_wheel_y >= mouse_wheel_max){
                std::cout << "zoom in" << std::endl;
                zoomfactor = 1 + ((mouse_wheel_y * (mouse_wheel_y + 1) / 100.0f));
                //std::cout << "scale value: " << 1 + ((mouse_wheel_y * (mouse_wheel_y + 1) / 10.0f)) << std::endl;
            }else{
                std::cout << "zoom out" << std::endl;
                zoomfactor = 1 + ((mouse_wheel_y * (mouse_wheel_y + 1) / 100.0f));
                //std::cout << "scale value: " << 1 + (((mouse_wheel_y + 1) * (mouse_wheel_y + 2) / 10.0f)) << std::endl;
                mouse_wheel_max = mouse_wheel_y;
            }

            // CALL FUNCTION HERE
            player->_width = player->_original_w * zoomfactor;
            player->_height = player->_original_h * zoomfactor; 
        
        }else{
            // if values go below < 0 then we just use preset scales since we only go max of -3

            float scales[3] = {0.8, 0.6, 0.3};
            // CALL FUNCTION HERE
            player->_width = player->_original_w * scales[-1 * (mouse_wheel_y + 1)];
            player->_height = player->_original_h * scales[-1 * (mouse_wheel_y + 1)];
        }

        std::cout << "mouse wheel y: " << mouse_wheel_y << std::endl;
    }
}

void GameScreen::InitalDragState(SDL_Event event, GameObject *player){
    
    Uint32 mouseState = SDL_GetMouseState(&window_x, &window_y);
    
    switch(event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
        {   
            std::cout << "buehuahs" << std::endl;
            if(SDL_BUTTON(SDL_BUTTON_LEFT)){
                std::cout << "should print once" << std::endl;
                prev_drag_window_position_x = screen_x;
                prev_drag_window_position_y = screen_y;
            }
        }
    }

    DragScreen(mouseState, player);
}

void GameScreen::DragScreen(Uint32 mouseState, GameObject *player){

    // if left mouse button is held down and moved
    if(mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)){
        //std::cout << "left" << std::endl;
        
        // updates drag position and takes it away from intial drag start position to get total offset to be moved
        
        current_window_position_x = screen_x - prev_drag_window_position_x;
        current_window_position_y = screen_y - prev_drag_window_position_y;
        
        player->_x += current_window_position_x;
        player->_y += current_window_position_y;

        prev_drag_window_position_x = screen_x;
        prev_drag_window_position_y = screen_y;

    }
}

void GameScreen::ScreenOffset(){
    // sets offset so that where grid starts is 0,0
    screen_x = window_x - (window_width - offset_width);
    screen_y = window_y - (window_height - offset_height - 10);
}