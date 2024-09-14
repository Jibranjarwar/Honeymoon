#include <SDL2/SDL.h>
#include <window.h>
#include <gameobject.h>
#include <iostream>
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
            player.Render();
            player2.Render();
            player3.Render();
            
            // allows for interaction with the ImGui window created
            ImGui_ImplSDL2_ProcessEvent(&event);
            
            // Draws IMGUI to the renderer to be ready to be presented on window by getting ImDrawData struct
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), Window::renderer);

            // displays whole window with all the rendering
            window.createDisplay();
        }
    }

    // Destroys ImGUI elements
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    return 0;

}