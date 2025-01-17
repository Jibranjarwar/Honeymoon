#include "serilization.h"
#include <iostream>
#include <fstream>

std::unordered_map<int, json> gameObjectMap;

void tester(std::vector<GameObject> gameObjects, GameScreen* gamescreen){

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
        json obj = {{"id", gameObject.GetID()}, {"size", {
            {"width", gameObject._width}, 
            {"height", gameObject._height}}},
            {"id", gameObject.GetID()},
            {"position", {{"x", gameObject._x}, {"y", gameObject._y}}},
            {"filename", gameObject._filename},
            {"originals", {{"original_x", gameObject._original_x}, {"original_y", gameObject._original_y}, {"original_w", gameObject._original_w}, {"original_h", gameObject._original_h}}}
            };
        
        // Add to the JSON array
        data["gameObjects"].push_back(obj);

        // Add to the unordered_map
        gameObjectMap[gameObject.GetID()] = obj;
    }

    data["gameScreen"] = {{"mouse_wheel_y", gamescreen->mouse_wheel_y},
        {"current_window_position_x", gamescreen->current_window_position_x},
        {"current_window_position_y", gamescreen->current_window_position_y},
        {"mouse_wheel_max", gamescreen->mouse_wheel_max},
        {"zoomfactor", gamescreen->zoomfactor},
        {"viewport_offset_x", gamescreen->viewport_offset_x},
        {"viewport_offset_y", gamescreen->viewport_offset_y},
        {"difference_x", gamescreen->difference_x}, {"difference_y", gamescreen->difference_y},
        {"width_difference", gamescreen->width_difference}, {"height_difference", gamescreen->height_difference}};

    std::ofstream file("data.json");
    
    if(file.is_open()){
        // pretty print with 4-space indentation
        file << data.dump(4);
        file.close();
        std::cout << "Data saved successfully" << std::endl;
    }else{
        std::cout << "Failed to open file for writing" << std::endl;
    }
}

json reader_tester(){
    std::ifstream file("data.json");
    json loaded_data;
    if(file.is_open()){
        file >> loaded_data;
        file.close();

        std::cout << "Name: " << loaded_data["gameObjects"][0]["name"] << std::endl;
        std::cout << "Position" << "(" << loaded_data["gameObjects"][0]["position"]["x"] << ", " << loaded_data["gameObjects"][0]["position"]["y"] << ")" << std::endl;
    }else{
        std::cout << "Failed to open file for reading!" << std::endl;
    }

    return loaded_data;
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