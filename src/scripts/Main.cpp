#include <SDL2/SDL.h>
#include "window.h"
#include "gamescreen.h"
#include "gameobject.h"
#include "luafunctions.h"
#include <iostream>
#include <vector>
#include <string>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "filemanager.h"
#include "serilization.h"
#include "camera.h"
#include "collision.h"
#include "gameobjectui.h"
#include <filesystem>
#include "json.hpp"
#include <unordered_map>
#include <set>
#include "Sol/sol.hpp"
#include <algorithm>
//SETUP LUA CPP WRAPPER 
//DEAFULT CHILDERN NAME
//CHILD STICKS WITH PARENT,
//CHILD PROPERTIES
//gameobject 

using json = nlohmann::json;

// moved gameObjects vector to global space so LuaFunctions.cpp can access it
// global variables
std::vector<GameObject> gameObjects;
sol::state global_lua;
std::vector<GameObjectUI> gameObjectsUI;

int main(int argc, char **argv){

    //std::cout << SOL_VERSION << std::endl;
    

    GameObject* matched_gameobject;
    bool stopDrag = false;
    int prev_screen_x = 0;
    int prev_screen_y = 0;


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

    Camera gameCamera(window.renderer, 400, 400, 200, 220, 0, 0, 0, 255);
    //Collision gameObjectCollision(window.renderer, 300, 300, 500, 220, 3, 252, 32, 255);

    std::vector<Camera> cameraObjects;

    cameraObjects.push_back(gameCamera);

    GameScreen* gameScreen = new GameScreen(window.renderer);

    // creates gameObject types
    if(!(std::filesystem::exists("data.json"))){
        //std::cout << "yes it does" << std::endl;
    
        player = GameObject(window.renderer, "C:\\Users\\shvdi\\Pictures\\Azula.png", 200, 200, 500, 200);
        //GameObject player2(window.renderer, 100, 100, 50, 50, 55, 55, 200, 255);
        player3 = GameObject(window.renderer, "C:\\Users\\shvdi\\Pictures\\blue_lock.jpg", 100, 100, 200, 300);
        
    }
    else{

        json load_data = reader_tester();

        player = GameObject(window.renderer, load_data["gameObjects"][0]["filename"], load_data["gameObjects"][0]["size"]["width"], load_data["gameObjects"][0]["size"]["height"], load_data["gameObjects"][0]["position"]["x"], load_data["gameObjects"][0]["position"]["y"]);
        //GameObject player2(window.renderer, 100, 100, 50, 50, 55, 55, 200, 255);
        player3 = GameObject(window.renderer, load_data["gameObjects"][1]["filename"], load_data["gameObjects"][1]["size"]["width"], load_data["gameObjects"][1]["size"]["height"], load_data["gameObjects"][1]["position"]["x"], load_data["gameObjects"][1]["position"]["y"]);

        gameScreen->Setter(load_data);
    }
    
    // list to hold all GameObjects in the UI
    std::vector<std::pair<int, GameObject>> deletedObjects;
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
    int preview_width, preview_height;
    int previousWidth = 0, previousHeight = 0;
    int offset_width, offset_height;
    int window_x, window_y;
    
    /*
    makes sure that the id's are static and being incremented each time new object of type
    GameObject class is created
    */
    std::cout << "object id: " << player.GetID() << std::endl;
    //std::cout << "object2 id: " << player2.GetID() << std::endl;   
    std::cout << "object3 id: " << player3.GetID() << std::endl;
    std::cout << "object4 id: " << player4.GetID() << std::endl; 
    GameObject* selectedGameObject = nullptr;
    GameObject* selectedChildObject = nullptr;
    global_lua.open_libraries(sol::lib::base);
    RegisterGameObjectWithLua(global_lua);               
    RegisterGameObjectsWithLua(global_lua, gameObjects); 

    while(!window.isClosed()){
        SDL_GetWindowSize(window.window, &width, &height);
        SDL_GetWindowSize(previewWindow.window, &preview_width, &preview_height);
        

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
                if(!isPressed){
                    if(ImGui::MenuItem("Run")){
                        isPressed = !isPressed;
                    }
                }else{
                    if(ImGui::MenuItem("Stop")){
                        isPressed = !isPressed;
                        std::sort(deletedObjects.begin(), deletedObjects.end(), 
                            [](const std::pair<int, GameObject>& a, const std::pair<int, GameObject>& b) {
                                return a.first < b.first; // Sort in ascending order
                            });
                        for(int i = 0; i < deletedObjects.size(); i++){
                            std::cout << "index: " << deletedObjects[i].first << std::endl;
                            std::cout << "size: " << gameObjects.size() << std::endl;
                            gameObjects.insert(gameObjects.begin() + deletedObjects[i].first, deletedObjects[i].second);
                            std::cout << "size: " << gameObjects.size() << std::endl;
                            gameObjectsCopy[deletedObjects[i].second.GetID()] = deletedObjects[i].second;
                            //gameObjectsUI.insert(gameObjectsUI.begin() + deletedObjects)
                            
                            //ISSUE: SOMETHING WITH THE GAMEOBJECTUI WHEN WE RESTORE OBJECT THE REFERENCING MESSES UP FOR THE OBJECTS
                            
                            /*for(auto& ui : gameObjectsUI){
                                if(ui.name.GetID() == deletedObjects[i].second.GetID()){
                                    std::cout << "samesame but not samesame" << std::endl;
                                }
                            }*/
                        }

                        deletedObjects.clear();
                    }
                }
                // test button for native script
                if(ImGui::MenuItem("RunScript")){
                    try{
                        global_lua.script_file("C:\\Users\\shvdi\\Documents\\4_Year_Project\\src\\scripts\\test.lua");
                    }catch(const sol::error& e){
                        std::cerr << "Lua Error: " << e.what() << std::endl;
                    }
                    
                }         
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
        ImGui::Begin("GameObject Manager", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowSize(ImVec2(width - offset_width, height)); // Adjust size as needed
        ImGui::SetWindowPos(ImVec2(0, 18));

        // if preview is on we disable gameManager so no new items can be added or changed while the test is on
        if(isPressed){
            ImGui::BeginDisabled(true);
        }

        ImGui::Text("GameObjects");

        // If user clicks a GameObject, set it as selected
        
        // sets input text size so we can see "new gameObject" always
        ImGui::SetNextItemWidth(width - offset_width - 115);
        // Input for new GameObject name

        ImGui::InputText("New GameObject", gameObjectName, IM_ARRAYSIZE(gameObjectName));
        //no same names, index if same ADD

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
            GameObject new_object = GameObject(window.renderer, "C:\\Users\\shvdi\\Pictures\\gyro_zepelli.jpg", name, 300, 300, 400, 400);
            gameObjectsUI.push_back({new_object, {}});
            gameObjects.push_back(new_object);
            // add last reference in gameObjects since new_object gets changed because of the vector needing to relocate memory for new space
            // ISSUE: SINCE THE WHOLE VECTOR GETS REALLOCATED THE MEMORY GETS MESSED UP WITH LUA AND CREATES CRASH
            //AddGameObjectToLua(lua, gameObjects.back());
            RegisterGameObjectsWithLua(global_lua, gameObjects); 

            // Clear the input field
            strcpy(gameObjectName, "");
        }
        for (size_t i = 0; i < gameObjectsUI.size(); ++i)
        {
            ImGui::PushID(i); // Unique ID for each GameObject

            // Check if this GameObject is selected
            bool isSelected = (selectedGameObject == &gameObjectsUI[i].name);
            if (ImGui::Selectable((gameObjectsUI[i].name._name + "##" + std::to_string(i)).c_str(), selectedGameObject == &gameObjectsUI[i].name)) {
                selectedGameObject = &gameObjectsUI[i].name; // Set selected parent
                selectedChildObject = nullptr; // Ensure child is not selected
                std::cout << "Selected GameObject: " << selectedGameObject->_name << std::endl;
            }

                // Tree node for each GameObject
                if (ImGui::TreeNode("##TreeNode", gameObjectsUI[i].name._name.c_str()))
                {

                    // sets input text size so we can see "Child Name" always
                    ImGui::SetNextItemWidth(width - offset_width - 110);

                    // Input for new child object name
                    ImGui::InputText("Child Name", childObjectName, IM_ARRAYSIZE(childObjectName));
                    if (ImGui::Button("Add Child"))
                    {
                        std::string childName = childObjectName;

                        // Assign a default name if no name is provided
                        if (childName.empty())
                        {
                            int childIndex = 1;
                            std::set<int> usedChildIndices; // Track used indices per parent

                            // Collect used indices for this parent
                            for (const auto &child : gameObjectsUI[i].children)
                            {
                                if (child._name.rfind("Child", 0) == 0) // If name starts with "Child"
                                {
                                    std::string numPart = child._name.substr(5);
                                    if (!numPart.empty() && std::all_of(numPart.begin(), numPart.end(), ::isdigit))
                                    {
                                        usedChildIndices.insert(std::stoi(numPart));
                                    }
                                }
                            }

                            // Find the next available index
                            while (usedChildIndices.count(childIndex))
                            {
                                childIndex++;
                            }

                            childName = "Child" + std::to_string(childIndex);
                        }

                        // Create and add the new child object
                        GameObject new_object = GameObject(window.renderer, "C:\\Users\\shvdi\\Pictures\\shadow.png", childName, 300, 300, 400, 100);
                        gameObjectsUI[i].children.push_back(new_object);
                        gameObjects.push_back(new_object);

                        // find gameObject in gameObject vector so we can add to children ID vector
                        auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
                                                   [&](const GameObject &obj)
                                                   { return obj.GetID() == gameObjectsUI[i].name.GetID(); });
                        
                        // add children IDs too gameObject so we can find them on easier later
                        it->childrenIDs.push_back(new_object.GetID());
                        
                        // add last reference in gameObjects since new_object gets changed because of the vector needing to relocate memory for new space
                        // ISSUE: SINCE THE WHOLE VECTOR GETS REALLOCATED THE MEMORY GETS MESSED UP WITH LUA AND CREATES CRASH
                        //AddGameObjectToLua(lua, gameObjects.back());
                        RegisterGameObjectsWithLua(global_lua, gameObjects); 

                        strcpy(childObjectName, ""); // Clear input field

                        // Reset selection to avoid crash
                        selectedChildObject = nullptr;
                    }

                    // List children
                    for (size_t j = 0; j < gameObjectsUI[i].children.size(); ++j)
                    {
                        ImGui::PushID(j); // Unique ID for each child
                        // Allow clicking to select a child
                        if (ImGui::Selectable((gameObjectsUI[i].children[j]._name + "##" + std::to_string(j)).c_str(), selectedChildObject == &gameObjectsUI[i].children[j])) {
                            selectedChildObject = &gameObjectsUI[i].children[j]; // Set selected child
                            selectedGameObject = nullptr; // Ensure parent is not selected
                            std::cout << "Selected Child Object: " << selectedChildObject->_name << std::endl;
                        }

                        ImGui::Text("- %s", gameObjectsUI[i].children[j]._name.c_str());

                        ImGui::SameLine();
                        
                        if (ImGui::Button("Remove"))
                        {
                            auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
                                                   [&](const GameObject &obj)
                                                   { return obj.GetID() == gameObjectsUI[i].children[j].GetID(); });
                            
                            if (it != gameObjects.end())
                            {
                                gameObjects.erase(it);              // Remove the GameObject from the vector
                                gameObjectsCopy.erase(it->GetID()); // Remove the GameObject from the map
                                it->childrenIDs.erase(std::remove(it->childrenIDs.begin(), it->childrenIDs.end(), it->GetID()), it->childrenIDs.end()); // Rempves the GameObject id from children vector
                            }

                            // remake Lua Table after deleting gameObjects
                            RegisterGameObjectsWithLua(global_lua, gameObjects);

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
                    // Remove from gameObjects vector by matching the ID using a lambda function
                    // where find_if returns a pointer to gameObject
                    auto it = std::find_if(gameObjects.begin(), gameObjects.end(), 
                        [&](const GameObject& obj) { return obj.GetID() == gameObjectsUI[i].name.GetID(); });

                    if (it != gameObjects.end())
                    {
                        gameObjectsCopy.erase(it->GetID()); // Remove the GameObject from the map
                        gameObjects.erase(it); // Remove the GameObject from the vector
                        
                    }
                    
                    // loop through children if not empty so we delete everything related to a gameObject since they
                    // are its children
                    if(gameObjectsUI[i].children.size() > 0){
                        for(int j = 0; j < gameObjectsUI[i].children.size(); j++){
                            auto it = std::find_if(gameObjects.begin(), gameObjects.end(), 
                                [&](const GameObject& obj) { return obj.GetID() == gameObjectsUI[i].children[j].GetID(); });

                            if (it != gameObjects.end())
                            {
                                gameObjectsCopy.erase(it->GetID()); // Remove the GameObject from the map
                                gameObjects.erase(it); // Remove the GameObject from the vector
                                it->childrenIDs.clear();                             
                            }
                        }

                        // clears children vector so its back to being empty
                        gameObjectsUI[i].children.clear();
                    }
                    // remake Lua Table after deleting gameObjects
                    RegisterGameObjectsWithLua(global_lua, gameObjects);
                    
                    gameObjectsUI.erase(gameObjectsUI.begin() + i);
                    ImGui::TreePop(); // Close the tree node before deleting
                    ImGui::PopID();   // Remove the current GameObject ID
                    break;
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
        
        // end if statement for preview disable gameManager since it needs and end for where we stop the Disabled in ImGui
        if(isPressed){
            ImGui::EndDisabled();
        }
        
        ImGui::End();

        if (selectedGameObject != nullptr || selectedChildObject != nullptr)
        {
            bool exists = false;
            GameObject *currentObject = selectedGameObject != nullptr ? selectedGameObject : selectedChildObject;
            // Check if the selected GameObject or Child Object still exists
            for (const auto &obj : gameObjectsUI)
            {
                if (&obj.name == currentObject)
                {
                    exists = true;
                    break;
                }
                for (const auto &child : obj.children)
                {
                    if (&child == currentObject)
                    {
                        exists = true;
                        break;
                    }
                }
                if (exists)
                    break;
            }

            // If the selected GameObject or Child Object has been removed, reset the pointer
            if (!exists)
            {
                selectedGameObject = nullptr;
                selectedChildObject = nullptr;
            }
            else
            {
                // Begin the right-hand properties menu for the selected object
                ImGui::Begin("Object Properties", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
                ImGui::SetWindowSize(ImVec2(250, height - offset_height - 18));
                ImGui::SetWindowPos(ImVec2(width - 250, 18));

                // Display selected object name
                ImGui::Text("Selected Object: %s", currentObject->_name.c_str());
                if (!ImGui::IsWindowFocused())
                {
                    stopDrag = false; // Stop dragging if the window is unselected
                }

                // Store the current name before modifying
                static char renameBuffer[128];
                strncpy(renameBuffer, currentObject->_name.c_str(), sizeof(renameBuffer));

                if (ImGui::InputText("Name", renameBuffer, IM_ARRAYSIZE(renameBuffer)))
                {
                    // Check if the input is not empty before updating
                    if (strlen(renameBuffer) > 0)
                    {
                        currentObject->_name = renameBuffer; // Update name only if input is not empty
                    }
                }
                // Continuously update X/Y fields during dragging
                int new_x = currentObject->getX();
                int new_y = currentObject->getY();

                // Find the matched GameObject in the gameObjects vector
                GameObject *matched_gameobject = nullptr;
                for (auto &obj : gameObjects)
                {
                    if (obj.GetID() == currentObject->GetID())
                    {
                        matched_gameobject = &obj;
                        break;
                    }
                }

                if (matched_gameobject)
                {
                    int prev_screen_x = matched_gameobject->_screen_x;
                    int prev_screen_y = matched_gameobject->_screen_y;
                    int prev_screen_width = matched_gameobject->_screen_width;
                    int prev_screen_height = matched_gameobject->_screen_height;

                    ImGui::Text("Position");
                    
                    // Add drag controls for X and Y
                    if (ImGui::DragInt("X Position", &matched_gameobject->_screen_x, 1.0f, -1000, 1000))
                    {
                        stopDrag = true;
                        int diff_x = (prev_screen_x - matched_gameobject->_screen_x) * -1;
                        matched_gameobject->UpdatePosX(diff_x);

                        // If the selected object is a parent, update its children's positions
                        if (selectedGameObject != nullptr)
                        {
                            for (size_t i = 0; i < gameObjectsUI.size(); ++i)
                            {
                                if (gameObjectsUI[i].name.GetID() == matched_gameobject->GetID())
                                {
                                    // Update children positions
                                    for (auto &child : gameObjectsUI[i].children)
                                    {
                                        child.UpdatePosX(diff_x);

                                        // Also update the copy in the gameObjects vector
                                        for (auto &obj : gameObjects)
                                        {
                                            if (obj.GetID() == child.GetID())
                                            {
                                                obj._screen_x += diff_x;
                                                obj.UpdatePosX(diff_x);
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }

                    if (ImGui::DragInt("Y Position", &matched_gameobject->_screen_y, 1.0f, -1000, 1000))
                    {
                        stopDrag = true;
                        int diff_y = (prev_screen_y - matched_gameobject->_screen_y);
                        matched_gameobject->UpdatePosY(diff_y);

                        // If the selected object is a parent, update its children's positions
                        if (selectedGameObject != nullptr)
                        {
                            for (size_t i = 0; i < gameObjectsUI.size(); ++i)
                            {
                                if (gameObjectsUI[i].name.GetID() == matched_gameobject->GetID())
                                {
                                    // Update children positions
                                    for (auto &child : gameObjectsUI[i].children)
                                    {
                                        child.UpdatePosY(diff_y);

                                        // Also update the copy in the gameObjects vector
                                        for (auto &obj : gameObjects)
                                        {
                                            if (obj.GetID() == child.GetID())
                                            {
                                                obj.UpdatePosY(diff_y);
                                                obj._screen_y -= diff_y;
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }

                    ImGui::Text("Size");
                    if (ImGui::DragInt("Width", &matched_gameobject->_screen_width, 1.0f, -1000, 1000))
                    {
                        stopDrag = true;
                        int diff_width = (prev_screen_width - matched_gameobject->_screen_width) * -1;
                        matched_gameobject->UpdateWidth(diff_width);

                        // If the selected object is a parent, update its children's positions
                        if (selectedGameObject != nullptr)
                        {
                            for (size_t i = 0; i < gameObjectsUI.size(); ++i)
                            {
                                if (gameObjectsUI[i].name.GetID() == matched_gameobject->GetID())
                                {
                                    // Update children positions
                                    for (auto &child : gameObjectsUI[i].children)
                                    {
                                        child.UpdateWidth(diff_width);
                                        //child._screen_width -= diff_width;

                                        // Also update the copy in the gameObjects vector
                                        for (auto &obj : gameObjects)
                                        {
                                            if (obj.GetID() == child.GetID())
                                            {
                                                obj.UpdateWidth(diff_width);
                                                obj._screen_width += diff_width;
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }

                    if (ImGui::DragInt("Height", &matched_gameobject->_screen_height, 1.0f, -1000, 1000))
                    {
                        stopDrag = true;
                        int diff_height = (prev_screen_height - matched_gameobject->_screen_height) * -1;
                        matched_gameobject->UpdateHeight(diff_height);

                        // If the selected object is a parent, update its children's positions
                        if (selectedGameObject != nullptr)
                        {
                            for (size_t i = 0; i < gameObjectsUI.size(); ++i)
                            {
                                if (gameObjectsUI[i].name.GetID() == matched_gameobject->GetID())
                                {
                                    // Update children positions
                                    for (auto &child : gameObjectsUI[i].children)
                                    {
                                        child.UpdateHeight(diff_height);
                                        //child._screen_height += diff_height;

                                        // Also update the copy in the gameObjects vector
                                        for (auto &obj : gameObjects)
                                        {
                                            if (obj.GetID() == child.GetID())
                                            {
                                                obj._screen_height += diff_height;
                                                obj.UpdateHeight(diff_height);
                                                
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }

                // If the selected object is a parent, display its children
                if (selectedGameObject != nullptr)
                {
                    ImGui::Separator();
                    ImGui::Text("Children:");

                    for (size_t i = 0; i < gameObjectsUI.size(); ++i)
                    {
                        if (gameObjectsUI[i].name.GetID() == selectedGameObject->GetID())
                        {
                            for (size_t j = 0; j < gameObjectsUI[i].children.size(); ++j)
                            {
                                ImGui::PushID(j); // Ensure unique ID for each child

                                // Make the child name clickable
                                if (ImGui::Selectable(gameObjectsUI[i].children[j]._name.c_str(), selectedChildObject == &gameObjectsUI[i].children[j]))
                                {
                                    selectedChildObject = &gameObjectsUI[i].children[j]; // Set selected child
                                    selectedGameObject = nullptr;                        // Deselect parent
                                }

                                ImGui::PopID(); // Remove the current child ID
                            }
                            break;
                        }
                    }
                }

                ImGui::Text("Texture:");
                ImTextureID obj_image = (ImTextureID)matched_gameobject->_objTexture;
                ImGui::Image(obj_image, ImVec2(200, 200));
                if(ImGui::Button("Select Image")){
                    std::string newTexture = SelectImageFile();
                    
                    // this changes all instances that contain the old texture to the new texture including the previewGameObjects in gameObjectsCopy
                    matched_gameobject->_objTexture = matched_gameobject->Texture(newTexture, window.renderer);
                    matched_gameobject->_previewTexture = matched_gameobject->Texture(newTexture, previewWindow.renderer);
                    
                    // iterates through gameObjectsCopy using a lambda function to find key-value pair for the gameObject we are looking for
                    auto it = std::find_if(gameObjectsCopy.begin(), gameObjectsCopy.end(),
                                                       [&](const auto &pair)
                                                       { return pair.second.GetID() == matched_gameobject->GetID();});
                    
                    if(it != gameObjectsCopy.end()){
                        it->second._objTexture = matched_gameobject->Texture(newTexture, window.renderer);
                        it->second._previewTexture = matched_gameobject->Texture(newTexture, previewWindow.renderer);
                        std::cout << "new Texture: " << newTexture << std::endl;
                    }
                }

                if(!matched_gameobject->addedCollision){
                    if(ImGui::Button("Add Collision")){
                        matched_gameobject->AddCollision(window.renderer);
                        matched_gameobject->addedCollision = true;
                    }
                }else{
                    ImGui::Text("Collision Attributes:");
                    int collision_x = matched_gameobject->collisionBox.getX();
                    int collision_y = matched_gameobject->collisionBox.getY();
                    prev_screen_x = matched_gameobject->collisionBox._screen_x;
                    prev_screen_y = matched_gameobject->collisionBox._screen_y;
                    
                    ImGui::Text("Position");
                    if (ImGui::DragInt("X Position##", &matched_gameobject->collisionBox._screen_x, 1.0f))
                    {
                        int col_diff_x = (prev_screen_x - matched_gameobject->collisionBox._screen_x) * -1;
                        stopDrag = true;
                        matched_gameobject->collisionBox.UpdatePosX(col_diff_x);
                    }

                    if (ImGui::DragInt("Y Position##", &matched_gameobject->collisionBox._screen_y, 1.0f))
                    {
                        int col_diff_y = (prev_screen_y - matched_gameobject->collisionBox._screen_y);
                        stopDrag = true;
                        matched_gameobject->collisionBox.UpdatePosY(col_diff_y);
                    }

                    ImGui::Text("Size");
                    if (ImGui::DragInt("Width##", &matched_gameobject->collisionBox._width, 1.0f))
                    {
                        stopDrag = true;
                    }

                    if (ImGui::DragInt("Height##", &matched_gameobject->collisionBox._height, 1.0f))
                    {
                        stopDrag = true;
                    }
                }


                // Add a Close Button
                if (ImGui::Button("Close"))
                {
                    selectedGameObject = nullptr;  // Deselect the GameObject and close the popup
                    selectedChildObject = nullptr; // Deselect the Child Object and close the popup
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
            if(cameraObjects[0].Game_Camera_Objects(gameObjects[i])){
                
                // Checks if ID already exists in Dictionary OR if the value for the key is -1 we also change it
                if((gameObjectsCopy.find(gameObjects[i].GetID()) == gameObjectsCopy.end()) || gameObjectsCopy[gameObjects[i].GetID()].GetID() == -1){
                    gameObjectsCopy[gameObjects[i].GetID()] = GameObject(gameObjects[i]);
                }
            }
            // Checks if gameObject is not in Camera
            else if(!cameraObjects[0].Game_Camera_Objects(gameObjects[i])){
                // instead of removing we now just assign default empty GameObject if we need to de-reference object
                gameObjectsCopy[gameObjects[i].GetID()] = GameObject();
            }
            gameObjects[i].Render(width - offset_width, 0, width, height - offset_height);

            if(gameObjects[i].addedCollision){
                gameObjects[i].RenderCollisionBox(3, width - offset_width, 0, width, height - offset_height);
                //GameObject* collidedObj = gameObjects[i].collisionBox.Collision_Check(gameObjects[i], gameObjects);
                /*if(collidedObj != nullptr){
                    std::cout << collidedObj->GetID() << std::endl;
                    gameObjects[i].collisionBox.Del(collidedObj, gameObjects, gameObjectsUI, gameObjectsCopy);
                }*/
            }
        }
        cameraObjects[0].Camera_Render(3, width - offset_width, 0, width, height - offset_height);
        //gameObjectCollision.Collision_Render(3, width - offset_width, 0, width, height - offset_height);

        //This is a copy of the above gameObject but because its in a vector doesnt change original instance like above
        //gameObjects[0].Render(width - offset_width, 0, width, height - offset_height);

        // needs the be called to register events like key presses
        if (SDL_PollEvent(&event))
        {
            // NOTE: COULD HAVE ISSUES DEPENDING ON NULLPTR SINCE NO CHECKS
            // sets the global event variable for GameObject class
            GameObject::setEvent(&event);

            window.pollEvents(event);

            // allows for interaction with the ImGui window created
            ImGui_ImplSDL2_ProcessEvent(&event);

            // Movement function
            //player3.Movement(event);
            if (event.type == SDL_KEYDOWN)
            {

                // Check if a GameObject is selected
                GameObject *selectedObject = nullptr;
                if (selectedGameObject != nullptr)
                {
                    selectedObject = selectedGameObject; // Use the selected parent GameObject
                }
                else if (selectedChildObject != nullptr)
                {
                    selectedObject = selectedChildObject; // Use the selected child GameObject
                }
            }
            /*player3.Movement(event);
            
            if(gameObjects.size() > 2){
                gameObjects[2].Movement(event);
            }*/

            // prevents gameObjects from spawning in wrong areas once preview is on and user switches back to editor
            if(!isPressed && !stopDrag){
            
                // calls Zoom In and Out Function for GameScreen
                gameScreen->ZoomInAndOut(event, gameObjects, cameraObjects);
            
                // Checks the drag for gameObject
                gameScreen->InitalDragState(event, gameObjects, cameraObjects);
            }
            
        }    
        
        gameScreen->ScreenOffset();

        if(global_lua["gameLoop"].valid()){
            sol::function gameLoop = global_lua["gameLoop"];
            gameLoop();  // Call the Lua game loop
        }else {
            // debug for Lua Code
            //std::cerr << "Error: 'gameLoop' function not found in Lua!" << std::endl;
        }

        // Draws IMGUI to the renderer to be ready to be presented on window by getting ImDrawData struct
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), window.renderer);
        
        // displays whole window with all the rendering
        window.createDisplay();

        if(isPressed){
            SDL_ShowWindow(previewWindow.window);
            selectedGameObject = nullptr;
            
            //std::cout << gameObjectsCopy.size() << std::endl;
            for(int i = 0; i < gameObjectsCopy.size(); i++){
                
                //std::cout << "ID: " << gameObjectsCopy[i].GetID() << std::endl;
                // makes sure that gameObjectCopy value ID doesnt equal too -1 which is what defaultObject has as an ID also
                
                // ISSUE: WITH THE RESIZE FUNCTION IT GETS CALLED EVERY FRAME SO KEEPS EXPANDING AND BREAKS
                if(gameObjectsCopy[i] != defaultObject){
                    //std::cout << "ID: " << gameObjectsCopy[i].GetID() << std::endl;
                    auto it = std::find_if(gameObjects.begin(), gameObjects.end(), 
                    [&](const GameObject& obj) { return obj.GetID() == gameObjectsCopy[i].GetID(); });

                    cameraObjects[0].Resize(gameObjectsCopy[i], *it, preview_width, preview_height);
                    std::cout << "width after resize: " << gameObjectsCopy[i]._width << std::endl;
                    gameObjectsCopy[i].RenderPreview(previewWindow.renderer, width - offset_width, offset_height);
                    std::cout << gameObjectsCopy[i].GetID();
                    
                    if(gameObjects[2].addedCollision){
                        //gameObjects[2].collisionBox.On_Collision(gameObjects[2], gameObjects);
                        GameObject* collidedObject = gameObjects[2].collisionBox.Collision_Check(gameObjects[2], gameObjects);
                        
                        if(collidedObject != nullptr){
                            // finds object in gameObject vector
                            auto collided_it = std::find(gameObjects.begin(), gameObjects.end(), *collidedObject);
                            
                            // uses distance formula from algorithm library to find index of gameObeject
                            int index = std::distance(gameObjects.begin(), collided_it);
                            
                            // push deletedObjects too vector
                            deletedObjects.push_back(std::make_pair(index, *collidedObject));
                            
                            // Del function to delete on collision
                            // ISSUE IF IT TRIES TO DELETE CHILD OBJECT OF ITSELF IT CRASHES
                            gameObjects[2].collisionBox.Del(collidedObject, gameObjects, gameObjectsCopy);
                        }
                        /*GameObject* collidedObj = it->collisionBox.Collision_Check(*it, gameObjects);
                        if(collidedObj != nullptr){
                            std::cout << collidedObj->GetID() << std::endl;
                            int index = std::distance(gameObjects.begin(), it);
                            deletedObjects.push_back(std::make_pair(index, *it));
                            it->collisionBox.Del(collidedObj, gameObjects, gameObjectsCopy);
                        
                        }*/
                    }
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
    IMG_Quit();
    SDL_Quit();
    return 0;

}