#include <SDL2/SDL.h>
#include <window.h>

int main(int argc, char **argv){
    
    Window window("GAME ENGINE", 800, 640);
    SDL_Event event;    
    
    while(!window.isClosed()){        
        window.pollEvents(event);
    }
    
    return 0;

}