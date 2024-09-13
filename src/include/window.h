#pragma once

#include <string>
#include <SDL2/SDL.h>

class Window{

//public func
public:
    Window(const std::string &title, int width, int height);
    ~Window();
    void pollEvents(SDL_Event &event);
    
    // inline function which checks if the application should close or not
    inline bool isClosed() const { return _closed; }

// private func
private:
    bool init();

// public members
public:
    static SDL_Window* window;


// private members
private:
    std::string _title;
    int _width = 640;
    int _height = 640;

    bool _closed = false;
};