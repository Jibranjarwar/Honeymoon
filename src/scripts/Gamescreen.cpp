#include "gamescreen.h"
#include "window.h"
#include <iostream>

int GameScreen::difference_x = 0;
int GameScreen::difference_y = 0;

GameScreen::GameScreen(SDL_Renderer* renderer): renderer(renderer){
}

void GameScreen::DrawGraph(SDL_Window *window){
    
    // gets the window width and height and assigns it
    SDL_GetWindowSize(window, &window_width, &window_height);

    // sets colour to white for the lines
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // rounds to the next 10th e.g 453 -> 460 or e.g 337 -> 340
    temp_offset_height = (int)(window_height / 3.5);
    offset_height_rounded = 10 - (temp_offset_height % 10);
    offset_height = temp_offset_height + offset_height_rounded;
    //offset_height = (int)(window_height / 2);

    offset_width = (int)(window_width / 1.25);

    // creates grid
    count = 0;

    for(int w = window_width - offset_width; w <= window_width; w += 10 + mouse_wheel_y){
        for(int h = 0; h <= window_height - offset_height; h += 10 + mouse_wheel_y){
    
            SDL_RenderDrawLine(renderer, w, h, window_width, h);
            SDL_RenderDrawLine(renderer, w, h, w, offset_height);
            width_difference = w;
            height_difference = h;
            count++;
        }    
    } 

    // Edge case so that we finish up the graph with whatever remaining amount there is so its not a 10*10 pixel square intially like the rest
    if(height_difference < window_height){
        
        for(int w = window_width - offset_width; w <= window_width; w += 10 + mouse_wheel_y){
            SDL_RenderDrawLine(renderer, w, height_difference, w, window_height - offset_height);
        }
    }

    //std::cout << "amount: " << count << std::endl;
    SDL_RenderDrawLine(renderer, (window_width - offset_width), (window_height - offset_height), window_width, (window_height - offset_height));
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

void GameScreen::ZoomInAndOut(SDL_Event event, std::vector<GameObject>& array, std::vector<Camera>& cameras){

    if(Zoomed(event)){
        // changes value of zoom but never goes lower than -3 because then we need to draw to many lines and program crashes
        if(mouse_wheel_y + event.wheel.y > -4){
            mouse_wheel_y += event.wheel.y;
            
            if(mouse_wheel_y > mouse_wheel_max){
                mouse_wheel_max = mouse_wheel_y;
            }
        }

        SDL_GetMouseState(&window_x, &window_y);
        
        float reference_point_x = window_x - (window_width - offset_width);
        float reference_point_y = window_y + offset_height;

        std::cout << "ref x: " << reference_point_x << std::endl;
        std::cout << "ref y: " << reference_point_y << std::endl;

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

            std::cout << "original x: " << array[1]._original_x << std::endl;
            std::cout << "original y: " << array[1]._original_y << std::endl;

            // CALL FUNCTION HERE
            for (int i = 0; i < array.size(); i++) {
                // Scale object size based on zoomfactor
                array[i]._width = array[i]._original_w * zoomfactor;
                array[i]._height = array[i]._original_h * zoomfactor;

                // Calculate distance from reference using original values
                float dx = array[i]._original_x - difference_x - reference_point_x;
                float dy = array[i]._original_y - difference_y - reference_point_y;

                std::cout << "dx: " << dx << std::endl;
                std::cout << "dy: " << dy << std::endl; 

                // Scale distance based on zoom factor and update position
                array[i]._x = reference_point_x + dx * zoomfactor;
                array[i]._y = reference_point_y + dy * zoomfactor;
            }

            std::cout << "x: " << array[1]._x << std::endl;
            std::cout << "y: " << array[1]._y << std::endl;
            /*for(int i = 0; i < array.size(); i++){
                array[i]._width = array[i]._original_w * zoomfactor;
                array[i]._height = array[i]._original_h * zoomfactor;
            }*/
            //array[0]._width = array[0]._original_w * zoomfactor;
            //array[0]._height = array[0]._original_h * zoomfactor; 
            
            for (int i = 0; i < cameras.size(); i++) {
                // Scale camera size based on zoom factor
                cameras[i]._width = cameras[i]._original_w * zoomfactor;
                cameras[i]._height = cameras[i]._original_h * zoomfactor;

                // Calculates the distance from reference using original values
                float dx = cameras[i]._original_x - difference_x - reference_point_x;
                float dy = cameras[i]._original_y - difference_y - reference_point_y;

                std::cout << "Camera dx: " << dx << ", dy: " << dy << std::endl;

                // Scale distance based on zoom factor and update position
                cameras[i]._x = reference_point_x + dx * zoomfactor;
                cameras[i]._y = reference_point_y + dy * zoomfactor;
            }
        }else{
            // if values go below < 0 then we just use preset scales since we only go max of -3

            float scales[3] = {0.8, 0.6, 0.3};
            // CALL FUNCTION HERE
            //player->_width = player->_original_w * scales[-1 * (mouse_wheel_y + 1)];
            //player->_height = player->_original_h * scales[-1 * (mouse_wheel_y + 1)];
        }

        std::cout << "mouse wheel y: " << mouse_wheel_y << std::endl;
    }
}

//template <typename T>
void GameScreen::InitalDragState(SDL_Event event, std::vector<GameObject>& array, std::vector<Camera>& other_array){
    
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

                inital_drag_window_x = prev_drag_window_position_x;
                inital_drag_window_y = prev_drag_window_position_y;
            }
        }
    }

    DragScreen(mouseState, array, other_array);
}

void GameScreen::DragScreen(Uint32 mouseState, std::vector<GameObject>& array, std::vector<Camera>& other_array){
    
    // if left mouse button is held down and moved
    if(mouseState && SDL_BUTTON(SDL_BUTTON_LEFT)){
        //std::cout << "left" << std::endl;       
        wasPressed = true;
        // updates drag position and takes it away from intial drag start position to get total offset to be moved
        
        current_window_position_x = screen_x - prev_drag_window_position_x;
        current_window_position_y = screen_y - prev_drag_window_position_y;
        

        for(int i = 0; i < array.size(); i++){
            //std::cout << array[i].GetID() << std::endl;
            array[i]._x += current_window_position_x;
            array[i]._y += current_window_position_y;
        }
        for(int i = 0; i < other_array.size(); i++){
            //std::cout << array[i].GetID() << std::endl;
            other_array[i]._x += current_window_position_x;
            other_array[i]._y += current_window_position_y;
        }
        //player->_x += current_window_position_x;
        //player->_y += current_window_position_y;

        //std::cout << "x: " << array[0]._x << std::endl;
        //std::cout << "y: " << array[0]._y << std::endl;

        prev_drag_window_position_x = screen_x;
        prev_drag_window_position_y = screen_y;

    }
    
    // If mouseButton was let go and wasPressed is true means we were dragging
    if(!(mouseState) && wasPressed){
        wasPressed = false;

        // changes the co-ordinates so when we drag the origin co-ordinates change with the drag
        difference_x += inital_drag_window_x - screen_x;
        difference_y += inital_drag_window_y - screen_y;
    }
}

void GameScreen::ScreenOffset(){
    // sets offset so that where grid starts is 0,0 and adds difference of whatever the drag is intially being 0
    // NOTE: THIS WILL BE USED FOR THE DRAG 
    screen_x = window_x - (window_width - offset_width) + difference_x;
    screen_y = window_y - (window_height - offset_height) + difference_y;

    float zoom_offset_x = offset_width / zoomfactor;
    float zoom_offset_y = (window_height - offset_height) / zoomfactor;

    //std::cout << "screen x cords: " << screen_x << std::endl;
    //std::cout << "screen y cords: " << screen_y << std::endl;
    //std::cout << "range [" << min_x << " , " << max_x << "]" << std::endl;
    //std::cout << "screen y cords: " << zoom_offset_y << std::endl;
}

void GameScreen::Setter(json data){
    
    current_window_position_x = data["gameScreen"]["current_window_position_x"];
    current_window_position_y = data["gameScreen"]["current_window_position_y"];
    difference_x = data["gameScreen"]["difference_x"];
    difference_y = data["gameScreen"]["difference_y"];
    height_difference = data["gameScreen"]["height_difference"];
    mouse_wheel_max = data["gameScreen"]["mouse_wheel_max"];
    mouse_wheel_y = data["gameScreen"]["mouse_wheel_y"];
    viewport_offset_x = data["gameScreen"]["viewport_offset_x"];
    viewport_offset_y = data["gameScreen"]["viewport_offset_y"];
    width_difference = data["gameScreen"]["width_difference"];
    zoomfactor = data["gameScreen"]["zoomfactor"];
}