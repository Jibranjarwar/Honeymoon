#include "window.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <chrono>

using Clock = std::chrono::steady_clock;

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

    SDL_Surface* iconSurface = IMG_Load("scripts\\Engine_Logo_icon.png");
    if (iconSurface == nullptr) {
        std::cerr << "Failed to load icon: " << IMG_GetError() << std::endl;
        return false;
    } else {
        SDL_SetWindowIcon(window, iconSurface);
        SDL_FreeSurface(iconSurface);
    }

    return true;
}

// handles events right now only reacts too ESC key and the X Button
void Window::pollEvents(SDL_Event &event){
    
    switch (event.type)
    {
    case SDL_WINDOWEVENT:
        if(event.window.event == SDL_WINDOWEVENT_CLOSE){
            _closed = true;
            break;
        }
    
    case SDL_KEYDOWN:
        switch(event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                std::cout << "Escape" << std::endl;
                _closed = true;
                break;
        }
    
    }
}

void Window::showBootUpScreen() {
    // Load the splash image
    SDL_Texture* splashTexture = IMG_LoadTexture(renderer, "scripts\\Engine_Logo.PNG");  // Replace with your image path
    if (!splashTexture) {
        std::cerr << "Error loading splash image: " << IMG_GetError() << std::endl;
        return;
    }
    auto startTime = Clock::now();
    bool done = false;
    bool animate_text = true;
    int value = _width / 2 - 300;
    int width_box = 550;
    SDL_Event event;

    while (!done) {
        auto elapsed = Clock::now() - startTime;
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

        // Clear the screen
        SDL_RenderClear(renderer);

        // Render the splash image
        SDL_RenderCopy(renderer, splashTexture, nullptr, nullptr);
        
        // give an animated effect for text
        while(animate_text){
            value += 5;
            width_box -= 5;
            // reset the render to update new values and image
            SDL_RenderClear(renderer);

            // Draw the background again
            SDL_RenderCopy(renderer, splashTexture, nullptr, nullptr);
            
            // illusion of animated logo name
            SDL_Rect _dest_rect = {value, _height - 150, width_box, 100};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &_dest_rect);

            if(width_box <= 0){
                animate_text = false;
            }
            
            // present to screen
            SDL_RenderPresent(renderer);
        }

        // Update the screen
        SDL_RenderPresent(renderer);

        // Exit the loop after 1 seconds or when animation is done
        if (elapsedMs >= 1500) {
            done = true;
        }
    }

    // Clean up the texture
    SDL_DestroyTexture(splashTexture);
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