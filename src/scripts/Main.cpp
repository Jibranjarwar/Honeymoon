#include <SDL2/SDL.h>
#include "window.h"
#include "gamescreen.h"
#include "gameobject.h"
#include <iostream>
#include <string>
#include <vector> 
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "filemanager.h"
#include "serilization.h"
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;

int main(int argc, char **argv){
    
    // created a window object
    Window window("GAME ENGINE", 800, 640);
    Window previewWindow("PREVIEW", 800, 640);
    SDL_HideWindow(previewWindow.window);
    
    // initialize and setup for ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDL2_InitForSDLRenderer(window.window, window.renderer);
    ImGui_ImplSDLRenderer2_Init(window.renderer);

    GameObject player;
    GameObject player3;

    GameScreen* gameScreen = new GameScreen(window.renderer);

    // creates gameObject types
    if(!(std::filesystem::exists("data.json"))){
        //std::cout << "yes it does" << std::endl;
    
        player = GameObject(window.renderer, "C:\\Users\\shvdi\\Pictures\\Azula.png", 200, 200, 500, 200);
        //GameObject player2(window.renderer, 100, 100, 50, 50, 55, 55, 200, 255);
        player3 = GameObject(window.renderer, "C:\\Users\\shvdi\\Pictures\\blue_lock.jpg", 100, 100, 200, 400);
        
    }
    else{

        json load_data = reader_tester();

        player = GameObject(window.renderer, load_data["gameObjects"][0]["filename"], load_data["gameObjects"][0]["size"]["width"], load_data["gameObjects"][0]["size"]["height"], load_data["gameObjects"][0]["position"]["x"], load_data["gameObjects"][0]["position"]["y"]);
        //GameObject player2(window.renderer, 100, 100, 50, 50, 55, 55, 200, 255);
        player3 = GameObject(window.renderer, load_data["gameObjects"][1]["filename"], load_data["gameObjects"][1]["size"]["width"], load_data["gameObjects"][1]["size"]["height"], load_data["gameObjects"][1]["position"]["x"], load_data["gameObjects"][1]["position"]["y"]);

        gameScreen->Setter(load_data);
    }
    
    std::vector<GameObject> gameObjects;

    gameObjects.push_back(player);
    gameObjects.push_back(player3);

    // calls serilization cpp 

    // creates new vector with gameObjects
    std::vector<GameObject> gameObjectsCopy = gameObjects;
    // creates game Object with Texture or in game development terms "sprite"
    GameObject player4(window.renderer, "C:\\Users\\shvdi\\Pictures\\gyro_zepelli.jpg", 500, 500, 100, 100);

    //tester(gameObjects);
    //reader_tester();
    

    // creates a SDL event pointer
    SDL_Event event;

    bool isPressed = false;
    bool save = false;
    int width, height;
    int previousWidth = 0, previousHeight = 0;
    int offset_width, offset_height;
    
    /*
    makes sure that the id's are static and being incremented each time new object of type
    GameObject class is created
    */
    std::cout << "object id: " << player.GetID() << std::endl;
    //std::cout << "object2 id: " << player2.GetID() << std::endl;   
    std::cout << "object3 id: " << player3.GetID() << std::endl;
    std::cout << "object4 id: " << player4.GetID() << std::endl; 

    
    while(!window.isClosed()){        
        
        SDL_GetWindowSize(window.window, &width, &height);

        // sets colour to white for the lines
        //SDL_SetRenderDrawColor(window.renderer, 255, 255, 255, 255);

        // rounds to the next 10th e.g 453 -> 460 or e.g 337 -> 340
        if(previousWidth != width && previousHeight != height){
            int temp_offset_height = (int)(height / 3.5);
            int offset_height_rounded = 10 - (temp_offset_height % 10);
            offset_height = temp_offset_height + offset_height_rounded;

            offset_width = (int)(width / 1.25);
        }

        // All these start the ImGUI frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Creates new ImGUI window for testing atm
        ImGui::Begin("Controller");
        ImGui::SetWindowSize(ImVec2(200,100));
        if(!isPressed){
            if(ImGui::Button("Preview")){
                isPressed = !isPressed;
                std::cout << isPressed << std::endl;
            }
        }else{
            if(ImGui::Button("Stop")){
                isPressed = !isPressed;
                std::cout << isPressed << std::endl;
            }
        }

        if(!save){
            if(ImGui::Button("Save")){
                tester(gameObjects, gameScreen);
            }
        }
        ImGui::End();

        // calls file manager cpp
        Initialize(width - offset_width, height - offset_height, offset_width, offset_height, window.renderer);

        // Wraps up all ImGUI elements and compiles everything into a ImDrawData struct
        ImGui::Render();

        // renders the objects to the screen without this wont display
        // NOTE: THE ORDER IN WHICH YOU RENDER CAN BE SEEN AS "LAYERS"
        //player4.Render();
        //player2.Render();
        // using the vector to render this object currently
        //player3.Render();

        // draws graph
        gameScreen->DrawGraph(window.window);

        // EVERYTHING UP TO PLAYER RENDERER IS NEEDED SINCE RIGHT NOW WE RENDER IN MAIN.CPP
        // AND NEED TO EDGES TO PASS
        
        //player.Render(width - offset_width, 0, width, height - offset_height);

        //player2.RenderPreview(window.renderer, 200, 200);
        
        for(int i = 0; i < gameObjects.size(); i++){
            gameObjects[i].Render(width - offset_width, 0, width, height - offset_height);
            
        }

        // This is a copy of the above gameObject but because its in a vector doesnt change original instance like above
        //gameObjects[0].Render(width - offset_width, 0, width, height - offset_height);

        // needs the be called to register events like key presses
        if(SDL_PollEvent(&event)){
            
            window.pollEvents(event);

            // allows for interaction with the ImGui window created
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_KEYDOWN) {

                // Movement function
                //player3.Movement(event);
                //player2.Movement(event);
                gameObjects[0].Movement(event);
                gameObjectsCopy[0].Movement(event);
            }
            
            // calls Zoom In and Out Function for GameScreen
            gameScreen->ZoomInAndOut(event, gameObjects);
            
            // Checks the drag for gameObject 
            gameScreen->InitalDragState(event, gameObjects);
        }    
        
        gameScreen->ScreenOffset();

        // Draws IMGUI to the renderer to be ready to be presented on window by getting ImDrawData struct
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), window.renderer);
        
        // displays whole window with all the rendering
        window.createDisplay();

        if(isPressed){
            SDL_ShowWindow(previewWindow.window);
            
            for(int i = 0; i < gameObjectsCopy.size(); i++){
                gameObjectsCopy[i].RenderPreview(previewWindow.renderer, width - offset_width, offset_height);
            }
            //gameObjects[1].RenderPreview(previewWindow.renderer, width - offset_width, offset_height);
            previewWindow.createDisplay();
        }else{
            SDL_HideWindow(previewWindow.window);
        }
    }

    // Destroys ImGUI elements
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    IMG_Quit();
    SDL_Quit();
    return 0;

}
