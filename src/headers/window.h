#pragma once

#include <string>
#include <SDL2/SDL.h>

class Window{

//public func
public:
    Window(const std::string& title, int width, int height);
    ~Window();
    void pollEvents(SDL_Event& event);
    void showBootUpScreen();

    // function is not allowed to modify any members with the const
    void createDisplay() const;
    
    // inline function which checks if the application should close or not
    inline bool isClosed() const { return _closed; }

// private func
private:
    bool init();

// public members
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool _closed = false;


// private members
private:
    std::string _title;
    int _width = 640;
    int _height = 640;
};