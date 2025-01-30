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
#include <set>

//gameobject 
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
    std::set<int> usedGameObjectIndices;


    
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
    GameObject* selectedGameObject = nullptr;



    
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

          // If user clicks a GameObject, set it as selected
        
        // Input for new GameObject name
        static int gameObjectCounter = 1;

        ImGui::InputText("New GameObject", gameObjectName, IM_ARRAYSIZE(gameObjectName));

        if (ImGui::Button("Add GameObject"))
        {
            std::string name = gameObjectName;

            // Assign a default name if the field is empty
            if (name.empty())
            {
                int newIndex = 1;
                while (usedGameObjectIndices.count(newIndex))
                {
                    newIndex++;
                }
                usedGameObjectIndices.insert(newIndex);
                name = "GameObject" + std::to_string(newIndex);
            }
            else
            {
                // Ensure no duplicate indices for names starting with "GameObject"
                if (name.rfind("GameObject", 0) == 0)
                {
                    std::string numPart = name.substr(10); // Extract the number
                    if (!numPart.empty() && std::all_of(numPart.begin(), numPart.end(), ::isdigit))
                    {
                        int index = std::stoi(numPart);

                        // Add the index to the set if not already present
                        if (!usedGameObjectIndices.count(index))
                        {
                            usedGameObjectIndices.insert(index);
                        }
                    }
                }
            }

            // Create the new GameObject
            GameObject new_object = GameObject(window.renderer, "C:\\Users\\jjlov\\Downloads\\aasdsad.png", name, 300, 300, 400, 400);
            gameObjectsUI.push_back({new_object, {}});

            // Clear the input field
            strcpy(gameObjectName, "");
        }
        for (size_t i = 0; i < gameObjectsUI.size(); ++i)
        {
            ImGui::PushID(i); // Unique ID for each GameObject

            // Check if this GameObject is selected
            bool isSelected = (selectedGameObject == &gameObjectsUI[i].name);

            // Render the selectable item
            if (ImGui::Selectable((gameObjectsUI[i].name._name + "##" + std::to_string(i)).c_str(), isSelected))
            {
                // Set selectedGameObject on click
                selectedGameObject = &gameObjectsUI[i].name;
            }

            // Tree node for each GameObject
            if (ImGui::TreeNode("##TreeNode", gameObjectsUI[i].name._name.c_str()))
            {

                // Input for new child object name
                ImGui::InputText("Child Name", childObjectName, IM_ARRAYSIZE(childObjectName));
                if (ImGui::Button("Add Child") && strlen(childObjectName) > 0)
                {
                    // Add a new child to the current GameObject
                    GameObject new_object = GameObject(window.renderer, "C:\\Users\\jjlov\\Pictures\\Screenshots\\A.png", childObjectName, 300, 300, 400, 100);
                    gameObjectsUI[i].children.push_back(new_object);
                    strcpy(childObjectName, ""); // Clear input field
                }

                // List children
                for (size_t j = 0; j < gameObjectsUI[i].children.size(); ++j)
                {
                    ImGui::PushID(j); // Unique ID for each child
                    ImGui::Text("- %s", gameObjectsUI[i].children[j]._name.c_str());

                    ImGui::SameLine();
                    if (ImGui::Button("Remove"))
                    {
                        // Remove child from list
                        gameObjectsUI[i].children.erase(gameObjectsUI[i].children.begin() + j);
                        ImGui::PopID(); // Remove the current child ID before breaking
                        break;
                    }

                    ImGui::PopID(); // Remove the current child ID
                }

                // Button to remove the GameObject itself
                if (ImGui::Button("Remove GameObject"))
                {
                    std::string removedName = gameObjectsUI[i].name._name;
                    if (removedName.rfind("GameObject", 0) == 0)
                    {
                        std::string numPart = removedName.substr(10);
                        if (!numPart.empty() && std::all_of(numPart.begin(), numPart.end(), ::isdigit))
                        {
                            usedGameObjectIndices.erase(std::stoi(numPart));
                        }
                    }
                    gameObjectsUI.erase(gameObjectsUI.begin() + i);
                    ImGui::TreePop(); // Close the tree node before deleting
                    ImGui::PopID();   // Remove the current GameObject ID
                    break;            // Restart the loop after removal
                }

                ImGui::TreePop(); // Close the tree node
            }

            ImGui::PopID(); // Remove the current GameObject ID
        }

        if(!save){
            if(ImGui::Button("Save")){
                tester(gameObjects, gameScreen);
            }
        }
        ImGui::End();

        if (selectedGameObject != nullptr)
        {
            // Ensure the selected GameObject still exists
            bool exists = false;
            for (const auto &obj : gameObjectsUI)
            {
                if (&obj.name == selectedGameObject)
                {
                    exists = true;
                    break;
                }
            }

            // If the selected GameObject has been removed, reset the pointer
            if (!exists)
            {
                selectedGameObject = nullptr;
            }
            else
            {
                // Begin the right-hand properties menu
                ImGui::Begin("GameObject Properties", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

                // Set size and position for right-side alignment
                ImGui::SetWindowSize(ImVec2(250, height - 20));
                ImGui::SetWindowPos(ImVec2(width - 250, 20));

                // Display selected GameObject name
                ImGui::Text("Selected GameObject: %s", selectedGameObject->_name.c_str());

                // Rename GameObject (updates in real-time)
                static char renameBuffer[128];
                strncpy(renameBuffer, selectedGameObject->_name.c_str(), sizeof(renameBuffer));

                if (ImGui::InputText("Name", renameBuffer, IM_ARRAYSIZE(renameBuffer)))
                {
                    selectedGameObject->_name = renameBuffer; // Update the name dynamically
                }

                if (ImGui::IsItemDeactivatedAfterEdit() || ImGui::Button("Update Name"))
                {
                    // Step 1: Handle when the name is cleared
                    if (selectedGameObject->_name.empty())
                    {
                        std::string previousName = selectedGameObject->_name;

                        // Check if the previous name was in "GameObjectX" format
                        if (previousName.rfind("GameObject", 0) == 0)
                        {
                            std::string numPart = previousName.substr(10); // Extract number
                            if (!numPart.empty() && std::all_of(numPart.begin(), numPart.end(), ::isdigit))
                            {
                                int oldIndex = std::stoi(numPart);

                                // Reuse the old index
                                if (!usedGameObjectIndices.count(oldIndex))
                                {
                                    usedGameObjectIndices.insert(oldIndex);
                                    selectedGameObject->_name = "GameObject" + std::to_string(oldIndex);
                                    continue; // No need to assign a new index
                                }
                            }
                        }

                        // Assign a new index if no valid old index exists
                        int newIndex = 1;
                        while (usedGameObjectIndices.count(newIndex))
                        {
                            newIndex++;
                        }
                        usedGameObjectIndices.insert(newIndex);
                        selectedGameObject->_name = "GameObject" + std::to_string(newIndex);
                    }
                    else
                    {
                        // Step 2: Handle non-empty names starting with "GameObject"
                        if (selectedGameObject->_name.rfind("GameObject", 0) == 0)
                        {
                            std::string numPart = selectedGameObject->_name.substr(10); // Extract number
                            if (!numPart.empty() && std::all_of(numPart.begin(), numPart.end(), ::isdigit))
                            {
                                int index = std::stoi(numPart);

                                // Ensure the index is added to the set
                                usedGameObjectIndices.insert(index);
                            }
                        }
                    }
                }

               /* if (ImGui::Button("Update Name"))
                {
                    // Step 1: Remove old index if the current name is a default GameObject name
                    if (selectedGameObject->_name.rfind("GameObject", 0) == 0) // Starts with "GameObject"
                    {
                        std::string numPart = selectedGameObject->_name.substr(10); // Extract number part
                        if (!numPart.empty() && std::all_of(numPart.begin(), numPart.end(), ::isdigit))
                        {
                            int oldIndex = std::stoi(numPart);
                            usedGameObjectIndices.erase(oldIndex); // Free the index
                        }
                    }

                    // Step 2: If the name is empty, find the lowest available index
                    if (selectedGameObject->_name.empty())
                    {
                        int newIndex = 1;
                        while (usedGameObjectIndices.count(newIndex))
                        {
                            newIndex++; // Find the first free number
                        }

                        // Assign new name and mark index as used
                        selectedGameObject->_name = "GameObject" + std::to_string(newIndex);
                        usedGameObjectIndices.insert(newIndex);
                    }
                }*/
                // Display and edit children
                ImGui::Text("Children:");
                for (size_t i = 0; i < gameObjectsUI.size(); ++i)
                {
                    if (&gameObjectsUI[i].name == selectedGameObject)
                    {
                        for (size_t j = 0; j < gameObjectsUI[i].children.size(); ++j)
                        {
                            ImGui::PushID(j); // Ensure unique ID

                            // Editable input for child's name
                            static char childRenameBuffer[128];
                            strncpy(childRenameBuffer, gameObjectsUI[i].children[j]._name.c_str(), sizeof(childRenameBuffer));
                            if (ImGui::InputText(("Child " + std::to_string(j)).c_str(), childRenameBuffer, IM_ARRAYSIZE(childRenameBuffer)))
                            {
                                gameObjectsUI[i].children[j]._name = childRenameBuffer; // Update in real time
                            }

                            ImGui::PopID();
                        }
                        break;
                    }
                }

                ImGui::Text("Parent Position");
                static int newX = selectedGameObject->getX();
                static int newY = selectedGameObject->getY();

                // Input fields for modifying X and Y coordinates
                ImGui::InputInt("X Position", &newX);
                ImGui::InputInt("Y Position", &newY);

                if (ImGui::Button("Update Position"))
                {
                    // Calculate position difference
                    int deltaX = newX - selectedGameObject->getX();
                    int deltaY = newY - selectedGameObject->getY();

                    // Apply difference to selected GameObject
                    selectedGameObject->_x += deltaX;
                    selectedGameObject->_y += deltaY;

                    // Apply the same difference to child objects
                    for (auto &obj : gameObjectsUI)
                    {
                        if (&obj.name == selectedGameObject)
                        {
                            for (auto &child : obj.children)
                            {
                                child._x += deltaX;
                                child._y += deltaY;
                            }
                        }
                    }
                }
                // Add a Close Button
                if (ImGui::Button("Close"))
                {
                    selectedGameObject = nullptr; // Deselect the GameObject and close the popup
                }

                ImGui::End();
            }
        }

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
            if(gameObjectsUI[i].children.size() > 0){
                for(int j = 0; j < gameObjectsUI[i].children.size(); j++){
                    gameObjectsUI[i].children[j].Render(width - offset_width, 0, width, height - offset_height);
                }
            }

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