#include <SDL2/SDL.h>
#include "window.h"
#include "gamescreen.h"
#include "gameobject.h"
#include <iostream>
#include <vector>
#include <string>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "filemanager.h"
#include "serilization.h"
#include "camera.h"
#include <filesystem>
#include "json.hpp"
#include <unordered_map>


struct GameObjectUI {
    GameObject name;
    std::vector<GameObject> children; 
};


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
    //GameObject(window.renderer, "C:\Users\shvdi\Pictures\Azula.png", 100, 100, 0, 0);


    //create empty gameObjects since we assign in the if statement and couldnt access them outside without initialising them first
    GameObject player;
    GameObject player3;

    Camera gameCamera(window.renderer, 250, 200, 400, 300, 0, 0, 0, 255);

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
    
    // list to hold all GameObjects in the UI
    std::vector<GameObjectUI> gameObjectsUI;
    std::vector<GameObject> gameObjects;

    
    // for adding new GameObject and child names
    static char gameObjectName[32] = "";
    static char childObjectName[32] = "";
    GameObject defaultObject;

    gameObjects.push_back(player);
    gameObjects.push_back(player3);

    // calls serilization cpp 

    // creates new vector with gameObjects
    //std::vector<GameObject> gameObjectsCopy;

    std::unordered_map<int, GameObject> gameObjectsCopy;

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

         // Creates the top menu bar
        if (ImGui::BeginMainMenuBar()) {
            // Project Name menu
            if (ImGui::BeginMenu("Project Name")) {
                ImGui::MenuItem("New Project");   
                ImGui::MenuItem("Load Project");  
                ImGui::MenuItem("Save Project");  
                ImGui::EndMenu();
            }

            // Project menu
            if (ImGui::BeginMenu("Project")) {
                ImGui::MenuItem("Build");        
                ImGui::MenuItem("Run");          
                ImGui::MenuItem("Settings");     
                ImGui::EndMenu();
            }

            // Options menu
            if (ImGui::BeginMenu("Options")) {
                ImGui::MenuItem("Preferences");   
                ImGui::MenuItem("Help");          
                ImGui::MenuItem("About");         
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // Sidebar for managing GameObjects and ChildObjects
        ImGui::Begin("GameObject Manager", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowSize(ImVec2(250, 500)); // Adjust size as needed
        ImGui::SetWindowPos(ImVec2(0, 20));

        ImGui::Text("GameObjects");
        
        // Input for new GameObject name
        ImGui::InputText("New GameObject", gameObjectName, IM_ARRAYSIZE(gameObjectName));
        if (ImGui::Button("Add GameObject") && strlen(gameObjectName) > 0) {
            // Add a new GameObject to the list
            GameObject new_object = GameObject(window.renderer, "C:\\Users\\shvdi\\Pictures\\Azula.png", gameObjectName, 100, 100, 500, 500);
            gameObjectsUI.push_back({new_object, {}});//change struct to gameobject, be able to define gameobject with default parameters,
            strcpy(gameObjectName, ""); // Clear input field
        }

        // Display each GameObject with its children
        for (size_t i = 0; i < gameObjectsUI.size(); ++i) {
            ImGui::PushID(i); // Ensure unique ID for each item

            // Tree node for each GameObject
            if (ImGui::TreeNode(gameObjectsUI[i].name._name.c_str())) {
                
                // Input for new child object name
                ImGui::InputText("Child Name", childObjectName, IM_ARRAYSIZE(childObjectName));
                if (ImGui::Button("Add Child") && strlen(childObjectName) > 0) {
                    // Add a new child to the current GameObject
                    GameObject new_object = GameObject(window.renderer, "C:\\Users\\shvdi\\Pictures\\Azula.png", gameObjectName, 100, 100, 700, 500);
                    gameObjectsUI[i].children.push_back(new_object);
                    strcpy(childObjectName, ""); // Clear input field
                }

                // List children
                for (size_t j = 0; j < gameObjectsUI[i].children.size(); ++j) {
                    ImGui::Text("- %s", gameObjectsUI[i].children[j]._name.c_str());
                    
                    ImGui::SameLine();
                    if (ImGui::Button("Remove")) {
                        // Remove child from list
                        gameObjectsUI[i].children.erase(gameObjectsUI[i].children.begin() + j);
                        break;
                    }
                }

                // Button to remove the GameObject itself
                if (ImGui::Button("Remove GameObject")) {
                    gameObjectsUI.erase(gameObjectsUI.begin() + i);
                    ImGui::TreePop(); // Close the tree node before deleting
                    ImGui::PopID();
                    break; // Restart the loop after removal
                }

                ImGui::TreePop(); // Close the tree node
            }
            ImGui::PopID();
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
        player4.Render();
       // player2.Render();
        player3.Render();

        // draws graph
        gameScreen->DrawGraph(window.window);

        // EVERYTHING UP TO PLAYER RENDERER IS NEEDED SINCE RIGHT NOW WE RENDER IN MAIN.CPP
        // AND NEED TO EDGES TO PASS
        int width, height;
        SDL_GetWindowSize(window.window, &width, &height);

        //player2.RenderPreview(window.renderer, 200, 200);
        
        for(int i = 0; i < gameObjects.size(); i++){
            // ISSUE: A DANGLING POINTER WHEN BLUE LOCK IMAGE SHOWS BEFORE AZULA BUT WORKS FINE IF AZULA SHOWS FIRST THEN BLUE LOCK
            // FIXED: CHANGED DEFAULT GAMEOBJECT POINTER TO ASSIGN -1 SO WE DONT ERASE FROM DICTIONARY LIKE BEFORE 
            
            // Checks whether gameObject is in Camera
            if(gameCamera.Game_Camera_Objects(gameObjects[i])){
                
                // Checks if ID already exists in Dictionary OR if the value for the key is -1 we also change it
                if((gameObjectsCopy.find(gameObjects[i].GetID()) == gameObjectsCopy.end()) || gameObjectsCopy[gameObjects[i].GetID()].GetID() == -1){
                    gameObjectsCopy[gameObjects[i].GetID()] = GameObject(gameObjects[i]);
                }
            }
            // Checks if gameObject is not in Camera
            else if(!gameCamera.Game_Camera_Objects(gameObjects[i])){
                // instead of removing we now just assign default empty GameObject if we need to de-reference object
                gameObjectsCopy[gameObjects[i].GetID()] = GameObject();
            }
            gameObjects[i].Render(width - offset_width, 0, width, height - offset_height);
            
        }

        for(int i = 0; i < gameObjectsUI.size(); i++){

            gameObjectsUI[i].name.Render(width - offset_width, 0, width, height - offset_height);

        }

        gameCamera.Camera_Render(3);

        // This is a copy of the above gameObject but because its in a vector doesnt change original instance like above
        //gameObjects[0].Render(width - offset_width, 0, width, height - offset_height);

        // needs the be called to register events like key presses
        if(SDL_PollEvent(&event)){
            
            window.pollEvents(event);

            // allows for interaction with the ImGui window created
            ImGui_ImplSDL2_ProcessEvent(&event);

            // Movement function
            player3.Movement(event);
            
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
            
            //std::cout << gameObjectsCopy.size() << std::endl;
            for(int i = 0; i < gameObjectsCopy.size(); i++){
                
                //std::cout << "ID: " << gameObjectsCopy[i].GetID() << std::endl;
                // makes sure that gameObjectCopy value ID doesnt equal too -1 which is what defaultObject has as an ID also
                
                // ISSUE: WITH THE RESIZE FUNCTION IT GETS CALLED EVERY FRAME SO KEEPS EXPANDING AND BREAKS
                if(gameObjectsCopy[i] != defaultObject){
                    //std::cout << "ID: " << gameObjectsCopy[i].GetID() << std::endl;
                    gameCamera.Resize(gameObjectsCopy[i], width, height);
                    std::cout << "width after resize: " << gameObjectsCopy[i]._width << std::endl;
                    gameObjectsCopy[i].RenderPreview(previewWindow.renderer, width - offset_width, offset_height);
                }
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
    
    return 0;

}