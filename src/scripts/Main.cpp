#include <SDL2/SDL.h>
#include <window.h>
#include <gameobject.h>
#include <iostream>

int main(int argc, char **argv){
    
    // created a window object
    Window window("GAME ENGINE", 800, 640);
    
    // creates gameObject types 
    GameObject player(Window::renderer, 100, 100, 500, 200, 107, 55, 200, 255);
    GameObject player2(Window::renderer, 100, 100, 200, 200, 55, 55, 200, 255);

    
    // creates a SDL event pointer
    SDL_Event event;
    
    /*
    makes sure that the id's are static and being incremented each time new object of type
    GameObject class is created
    */
    std::cout << "object id: " << player.GetID() << std::endl;
    std::cout << "object2 id: " << player2.GetID() << std::endl;    
    
    while(!window.isClosed()){        
        
        // needs the be called to register events like key presses
        if(SDL_PollEvent(&event)){
            window.pollEvents(event);
            // renders the objects to the screen without this wont display
            player.Render();
            player2.Render();
            // displays whole window with all the rendering
            window.createDisplay();
        }
    }
    
    return 0;

}