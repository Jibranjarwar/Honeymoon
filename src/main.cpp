#include <iostream>
#include <SDL2/SDL.h>

const int WIDTH = 800, HEIGHT=600;

int main(int argc, char **argv){
    
    // Init for using SDL (can be changed later for specific items i.e audio, rendering)
    SDL_Init(SDL_INIT_EVERYTHING);

    // Creates Window Application
    SDL_Window *window = SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    
    // Checks wether SDL succeeded or not if not throws error
    if(NULL == window){
        std::cout << "could not create window" << SDL_GetError() << std::endl;
    }

    // Create a pointer to an SDL Event
    SDL_Event windowsEvent;

    // Keeps the application going unit esc is pressed or the X button at the top
    while(true){
        if(SDL_PollEvent(&windowsEvent)){
            if(windowsEvent.key.keysym.sym == SDLK_ESCAPE || windowsEvent.type == SDL_QUIT){
                break;
            }

        }
    }

    // Cleans up SDL elements
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}