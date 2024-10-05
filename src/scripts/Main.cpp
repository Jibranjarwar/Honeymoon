#include <SDL2/SDL.h>
#include "window.h"
#include "gamescreen.h"
#include "gameobject.h"
#include <iostream>
#include <string>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

int main(int argc, char **argv){
    
    // created a window object
    Window window("GAME ENGINE", 800, 640);
    
    // initialize and setup for ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDL2_InitForSDLRenderer(Window::window, Window::renderer);
    ImGui_ImplSDLRenderer2_Init(Window::renderer);

    // creates gameObject types 
    GameObject player(Window::renderer, "C:\\Users\\shvdi\\Pictures\\Azula.png", 200, 200, 500, 200);
    GameObject player2(Window::renderer, 100, 100, 200, 200, 55, 55, 200, 255);
    GameObject player3(Window::renderer, 100, 100, 200, 400, 55, 55, 200, 255);
    GameScreen* gameScreen = new GameScreen();
    

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
            
        // All these start the ImGUI frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Creates new ImGUI window for testing atm
        ImGui::Begin("Tester");
        ImGui::SetWindowSize(ImVec2(100,100));
        ImGui::End();

        // Wraps up all ImGUI elements and compiles everything into a ImDrawData struct
        ImGui::Render();

        // renders the objects to the screen without this wont display
        // NOTE: THE ORDER IN WHICH YOU RENDER CAN BE SEEN AS "LAYERS"
        player4.Render();
        player2.Render();
        player3.Render();

        // draws graph
        gameScreen->DrawGraph(window.window);

        // EVERYTHING UP TO PLAYER RENDERER IS NEEDED SINCE RIGHT NOW WE RENDER IN MAIN.CPP
        // AND NEED TO EDGES TO PASS
        int width, height;
        SDL_GetWindowSize(window.window, &width, &height);

        // sets colour to white for the lines
        SDL_SetRenderDrawColor(Window::renderer, 255, 255, 255, 255);

        // rounds to the next 10th e.g 453 -> 460 or e.g 337 -> 340
        int temp_offset_height = (int)(height / 3.5);
        int offset_height_rounded = 10 - (temp_offset_height % 10);
        int offset_height = temp_offset_height + offset_height_rounded;

        int offset_width = (int)(width / 1.25);
        
        player.Render(width - offset_width, 0, width, height - offset_height);

        // needs the be called to register events like key presses
        if(SDL_PollEvent(&event)){
            
            window.pollEvents(event);

            // allows for interaction with the ImGui window created
            ImGui_ImplSDL2_ProcessEvent(&event);

            // Movement function
            player3.Movement(event);
            
            // calls Zoom In and Out Function for GameScreen
            gameScreen->ZoomInAndOut(event, &player);
            
            // Checks the drag for gameObject 
            gameScreen->InitalDragState(event, &player);
        }    
        
        gameScreen->ScreenOffset();

        // Draws IMGUI to the renderer to be ready to be presented on window by getting ImDrawData struct
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), Window::renderer);
        
        // displays whole window with all the rendering
        window.createDisplay();
    }

    // Destroys ImGUI elements
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    return 0;

}