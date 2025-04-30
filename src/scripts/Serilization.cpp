#include "serilization.h"
#include <iostream>
#include <fstream>

std::unordered_map<int, json> gameObjectMap;

void SaveState(std::string projectName, std::vector<GameObject> gameObjects, std::vector<Camera> cameraObjects){

    /*json data = {
        {"name", "player"},
        {"position", {{"x", 100}, {"y", 200}}},
        {"health", 75},
        {"inventory", {"sword", "rock", "THE_CHOOSEN_ONE"}}
    };*/

    json data;
    // creates empty array in json for gameObjects
    //data["gameObjects"] = json::array();

    for(const auto& gameObject : gameObjects){
        json obj = {
            {"id", gameObject.GetID()}, 
            {"size", {
                {"width", gameObject._width}, 
                {"height", gameObject._height}}
            },
            {"name", gameObject._name},
            {"position", {
                {"x", gameObject._x}, 
                {"y", gameObject._y}
            }},
            {"filename", gameObject._filename},
            {"originals", {
                {"original_x", gameObject._original_x}, 
                {"original_y", gameObject._original_y}, 
                {"original_w", gameObject._original_w}, 
                {"original_h", gameObject._original_h}
            }},
            {"screen_cord", {
                {"x", gameObject._screen_x}, 
                {"y", gameObject._screen_y}, 
                {"width", gameObject._screen_width}, 
                {"height", gameObject._screen_height}
            }},
            {"oppacity", gameObject._a},
            {"addedCollision", gameObject.addedCollision}
        };

        if(gameObject.addedCollision){
            obj["collision"] = {
                {"x", gameObject.collisionBox._x},
                {"y", gameObject.collisionBox._y},
                {"screen_x", gameObject.collisionBox._screen_x},
                {"screen_y", gameObject.collisionBox._screen_y},
                {"originals", {
                    {"x", gameObject.collisionBox._original_x}, 
                    {"y", gameObject.collisionBox._original_y}, 
                    {"width", gameObject.collisionBox._original_w}, 
                    {"height", gameObject.collisionBox._original_h}
                }},
                {"colours", {
                    {"r", gameObject.collisionBox._r},
                    {"g", gameObject.collisionBox._g},
                    {"b", gameObject.collisionBox._b},

                }},
                {"oppacity", gameObject.collisionBox._a},
            };
        }

        if(!gameObject.childrenIDs.empty()){
            obj["children"] = gameObject.childrenIDs;
        }

        if(!gameObject.scripts.empty()){
            obj["scripts"] = gameObject.scripts;
        }
        
        // Add to the JSON array
        data["gameObjects"].push_back(obj);

        // Add to the unordered_map
        gameObjectMap[gameObject.GetID()] = obj;
    }

    data["LastObjectID"] = json{{"id", GameObject::_current_id}};

    data["Camera"] = json{
        {"ChangedScreenCord", cameraObjects[0].ChangeScreenCord},
        {"x", cameraObjects[0]._x},
        {"y", cameraObjects[0]._y},
        {"width", cameraObjects[0]._width},
        {"height", cameraObjects[0]._height},
        {"screen_x", cameraObjects[0]._screen_x},
        {"screen_y", cameraObjects[0]._screen_y},
        {"originals", {
            {"x", cameraObjects[0]._original_x}, 
            {"y", cameraObjects[0]._original_y}, 
            {"width", cameraObjects[0]._original_w}, 
            {"height", cameraObjects[0]._original_h}
        }},
        {"colours", {
            {"r", cameraObjects[0]._r},
            {"g", cameraObjects[0]._g},
            {"b", cameraObjects[0]._b},

        }},
        {"oppacity", cameraObjects[0]._a},
    };

    data["InitialMatrix"] = json{
        {"size", {
            {"width", GameScreen::InitialMatrix->_width}, 
            {"height",  GameScreen::InitialMatrix->_height}}
        },
        {"name",  GameScreen::InitialMatrix->_name},
        {"position", {
            {"x",  GameScreen::InitialMatrix->_x}, 
            {"y",  GameScreen::InitialMatrix->_y}
        }},
        {"filename",  GameScreen::InitialMatrix->_filename},
        {"originals", {
            {"x",  GameScreen::InitialMatrix->_original_x}, 
            {"y",  GameScreen::InitialMatrix->_original_y}, 
            {"width",  GameScreen::InitialMatrix->_original_w}, 
            {"height",  GameScreen::InitialMatrix->_original_h}
        }}
    };
    
    /*data["gameScreen"] = {{"mouse_wheel_y", gamescreen->mouse_wheel_y},
        {"current_window_position_x", gamescreen->current_window_position_x},
        {"current_window_position_y", gamescreen->current_window_position_y},
        {"mouse_wheel_max", gamescreen->mouse_wheel_max},
        {"zoomfactor", gamescreen->zoomfactor},
        {"viewport_offset_x", gamescreen->viewport_offset_x},
        {"viewport_offset_y", gamescreen->viewport_offset_y},
        {"difference_x", gamescreen->difference_x}, {"difference_y", gamescreen->difference_y},
        {"width_difference", gamescreen->width_difference}, {"height_difference", gamescreen->height_difference}};
    */

    std::ofstream file(projectName + ".json");
    
    if(file.is_open()){
        // pretty print with 4-space indentation
        file << data.dump(4);
        file.close();
        std::cout << "Data saved successfully" << std::endl;
    }else{
        std::cout << "Failed to open file for writing" << std::endl;
    }
}

json LoadState(std::string project){
    std::ifstream file(project);
    json loaded_data;
    if(file.is_open()){
        file >> loaded_data;
        file.close();
    }else{
        std::cout << "Failed to open file for reading!" << std::endl;
    }

    return loaded_data;
}

void rename_file(std::string project, std::string newName){
    std::filesystem::path oldPath = std::filesystem::current_path() / project;

    // Ensure the file exists before renaming
    if (!std::filesystem::exists(oldPath)) {
        std::cerr << "File not found: " << oldPath << std::endl;
        return;
    }

    std::filesystem::path newPath = oldPath.parent_path() / (newName + ".json");

    try {
        std::filesystem::rename(oldPath, newPath);
        std::cout << "File successfully renamed to: " << newPath.filename() << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Rename failed: " << e.what() << std::endl;
    }
}

//TO DO: GAMEOBJECTMAP HAS ISSUE WHERE IT DOESNT EXIST PAST SCOPE
// OF APPLICATION SO WHEN WE RELOAD NEW APPLICATION GAMEOBJECTMAP IS NULL
// SO NEED TO FIX HOW WE GET ID
json findGameObject(int id){
    if(gameObjectMap.find(id) != gameObjectMap.end()){
        return gameObjectMap[id];
    }

    std::cout << gameObjectMap[0].dump(4) << std::endl;

    return gameObjectMap[0];
}