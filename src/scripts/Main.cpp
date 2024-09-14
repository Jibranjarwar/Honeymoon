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
    GameObject player3(Window::renderer, 100, 100, 200, 400, 55, 55, 200, 255);

    // creates game Object with Texture or in game development terms "sprite"
    GameObject player4(Window::renderer, "C:\\Users\\shvdi\\Pictures\\gyro_zepelli.jpg", 500, 500, 100, 100);
    

    
    // creates a SDL event pointer
    SDL_Event event;
    
    /*
    makes sure that the id's are static and being incremented each time new object of type
    GameObject class is created
    */
    std::cout << "object id: " << player.GetID() << std::endl;
    std::cout << "object2 id: " << player2.GetID() << std::endl;   
    std::cout << "object3 id: " << player3.GetID() << std::endl;
    std::cout << "object4 id: " << player4.GetID() << std::endl; 

    
    while(!window.isClosed()){        
        
        // needs the be called to register events like key presses
        if(SDL_PollEvent(&event)){
            window.pollEvents(event);
            // renders the objects to the screen without this wont display
            // NOTE: THE ORDER IN WHICH YOU RENDER CAN BE SEEN AS "LAYERS"
            player4.Render();
            player.Render();
            player2.Render();
            player3.Render();
            // displays whole window with all the rendering
            window.createDisplay();
        }
    }
    
    return 0;

}