#include "window.h"
#include <SDL2/SDL_image.h>
#include <iostream>

// Gives value too static window value

// Constructor
Window::Window(const std::string &title, int width, int height) : _title(title), _width(width), _height(height)
{
    _closed = !init();
}

// Destructor
Window::~Window(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

bool Window::init(){
    
    // Initialize SDL VIDEO as of now
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr << "Failed to initialize SDL." << std::endl;
        return false;
    }

    // setting FLags variable for checking against Initalization
    int IMG_Flags = IMG_INIT_PNG | IMG_INIT_JPG;

    // Initialize SDL Image to take in PNG and JPG images
    if(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != IMG_Flags){
        std::cerr << "Failed to initialize SDL_image." << std::endl;
        return false;
    }

    // creates the SDL window application
    window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_RESIZABLE);

    // checks if there were any issues with the creation of the window if so return error 
    if(window == nullptr){
        std::cerr << "Failed to create window." << std::endl;
        return false;
    }

    // creates the SDL renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    // checks if there were any issues with the creation of the renderer if so return error
    if(renderer == nullptr){
        std::cerr << "Failed to create renderer." << std::endl;
        return false;
    }

    return true;
}

// handles events right now only reacts too ESC key and the X Button
void Window::pollEvents(SDL_Event &event){
    
    switch (event.type)
    {
    case SDL_QUIT:
        _closed = true;
        break;
    
    case SDL_KEYDOWN:
        switch(event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                _closed = true;
                break;
        }
    
    }
}

// renders everything including background NOTE: needs to be the last thing called in Main.cpp
void Window::createDisplay() const{
    
    // presents everything to the window
    SDL_RenderPresent(renderer);

    // sets background colour
    SDL_SetRenderDrawColor(renderer, 112, 112, 108, 255);
    
    // Clears the current rendering target
    SDL_RenderClear(renderer);
}