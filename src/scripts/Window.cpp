#include <window.h>
#include <iostream>

// Gives value too static window value
SDL_Window* Window::window = nullptr;

// Constructor
Window::Window(const std::string &title, int width, int height) : _title(title), _width(width), _height(height)
{
    _closed = !init();
}

// Destructor
Window::~Window(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::init(){
    
    // Initialize SDL VIDEO as of now
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr << "Failed to initialize SDL." << std::endl;
        return false;
    }

    // creates the SDL window application
    window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_RESIZABLE);

    // checks if there were any issues with the creation of the window if so return error 
    if(window == nullptr){
        std::cerr << "Failed to create window." << std::endl;
        return false;
    }

    return true;
}

// handles events right now only reacts too ESC key and the X Button
void Window::pollEvents(SDL_Event &event){
    
    if(SDL_PollEvent(&event)){
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
}