#include <iostream>
#include <SDL2/SDL.h>
#include "window.h"
#include "gameobject.h"
#include "gamescreen.h"
#include "camera.h"
#include "serilization.h"
#include "gameobjectui.h"
#include "lua_statefunctions.h"
#include "Sol/sol.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "Sol/sol.hpp"
#include <cassert>

std::vector<GameObject> gameObjects;
sol::state global_lua_state;
std::vector<GameObjectUI> gameObjectsUI;
std::vector<GameObject> deleteObjects;
std::unordered_map<int, GameObject> gameObjectsCopy;

void PrintTestResult(const std::string& testName, bool passed) {
    if (passed) {
        std::cout << "PASS: " << testName << std::endl;
    } else {
        std::cout << "FAIL: " << testName << std::endl;
    }
}

bool TestGameObjects() {
    bool allPassed = true;

    std::cout << "\n--- Testing GameObject Class ---" << std::endl;

    // Setup temporary SDL window and renderer
    SDL_Window* testWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, SDL_WINDOW_HIDDEN);
    SDL_Renderer* testRenderer = SDL_CreateRenderer(testWindow, -1, SDL_RENDERER_ACCELERATED);

    // Test 1: Basic creation with default constructor
    try {
        GameObject testObj;
        PrintTestResult("GameObject Default Constructor", testObj.GetID() == -1);
    } catch (...) {
        PrintTestResult("GameObject Default Constructor", false);
        allPassed = false;
    }

    // Test 2: Unique ID generation
    try {
        GameObject obj1(testRenderer, 100, 100, 0, 0, 255, 255, 255, 255);
        GameObject obj2(testRenderer, 100, 100, 0, 0, 255, 255, 255, 255);
        bool uniqueIDs = (obj1.GetID() != obj2.GetID());
        PrintTestResult("GameObject Unique IDs", uniqueIDs);
        allPassed = allPassed && uniqueIDs;
    } catch (...) {
        PrintTestResult("GameObject Unique IDs", false);
        allPassed = false;
    }

    // Cleanup
    SDL_DestroyRenderer(testRenderer);
    SDL_DestroyWindow(testWindow);

    return allPassed;
}

bool TestCameraObject(){
    bool allPassed = true;

    std::cout << "\n--- Testing CameraObject ---" << std::endl;

    // Setup temporary SDL window and renderer
    SDL_Window* testWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 900, 900, SDL_WINDOW_HIDDEN);
    SDL_Renderer* testRenderer = SDL_CreateRenderer(testWindow, -1, SDL_RENDERER_ACCELERATED);
    GameObject testObject1(testRenderer, 100, 100, 150, 550, 255, 255, 255, 255);
    GameObject testObject2(testRenderer, 100, 100, 50, 200, 255, 255, 255, 255);
    
    try{
        Camera TestCamera(testRenderer, 200, 200, 100, 500, 0, 0, 0, 255);
        bool cameraInitializedCorrectly = 
        TestCamera._width == 200 && 
        TestCamera._height == 200 &&
        TestCamera._x == 100 &&
        TestCamera._y == 500; 

        PrintTestResult("Camera Initialization", cameraInitializedCorrectly);
        
        allPassed = allPassed && cameraInitializedCorrectly;
        
        TestCamera.Camera_Render(5, 0, 0, 900, 900);
        testObject1.Render(0, 0, 900, 900);
        testObject2.Render(0, 0, 900, 900);

        bool cameraIntersection = TestCamera.Game_Camera_Objects(testObject1);
        bool noCameraIntersection = TestCamera.Game_Camera_Objects(testObject2);

        PrintTestResult("Camera Interaction with Object Inside", cameraIntersection);
        PrintTestResult("Camera Interaction with Object Outside", !noCameraIntersection);

        allPassed = allPassed && cameraIntersection && !noCameraIntersection;

        int _old_x = TestCamera._x;
        int _old_y = TestCamera._y;

        TestCamera.UpdatePosX(100);
        TestCamera.UpdatePosY(100);

        bool cameraUpdateX = _old_x + 100 == TestCamera._x;
        bool cameraUpdateY = _old_y + 100 == TestCamera._y;

        PrintTestResult("Camera Updating Position X", cameraUpdateX);
        PrintTestResult("Camera Updating Poistion Y", cameraUpdateY);

        allPassed = allPassed && cameraUpdateX && cameraUpdateY;

    }catch(...){
        PrintTestResult("Camera Initialisation", false);
        allPassed = false;
    }

    // Cleanup
    SDL_DestroyRenderer(testRenderer);
    SDL_DestroyWindow(testWindow);

    return allPassed;

}

// Test serialization functionality
bool TestSerialization() {
    bool allPassed = true;

    std::cout << "\n--- Testing Serialization ---" << std::endl;

    // Initializing fake values needed for test
    SDL_Window* testWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 900, 900, SDL_WINDOW_HIDDEN);
    SDL_Renderer* testRenderer = SDL_CreateRenderer(testWindow, -1, SDL_RENDERER_ACCELERATED);
    
    std::string testName = "TestProject";
    std::string newTestName = "TestNewProject";
    std::vector<GameObject> fakeGameObjects;
    std::vector<Camera> fakeCameraObjects;
    fakeGameObjects.push_back(GameObject(testRenderer, "test.png", "test1", 100, 100, 0, 50));
    fakeGameObjects.push_back(GameObject(testRenderer, "test2.png", "test2", 100, 100, 200, 200));
    fakeCameraObjects.push_back(Camera(testRenderer, 200, 200, 100, 500, 0, 0, 0, 255));

    GameScreen* gameScreen = new GameScreen(testRenderer);
    GameScreen::InitialMatrix = new GameObject(testRenderer, "default.png", "matrix4778192235010291", 100, 100, 400, 100);
    
    try {
        /*std::ofstream testFile("test_data.json");
        testFile << "{\"test\": true}";
        testFile.close();
        
        bool fileExists = std::ifstream("test_data.json").good();
        PrintTestResult("File Creation", fileExists);
        allPassed = allPassed && fileExists;*/
        
        // Setup temporary SDL window and renderer

        tester(testName, fakeGameObjects, fakeCameraObjects, gameScreen);

        bool fileExists = std::ifstream(testName + ".json").good();
        PrintTestResult("File Creation", fileExists);
        allPassed = allPassed && fileExists;

        if (fileExists) {
            json testFile = reader_tester(testName + ".json");
        
            bool hasGameObjects = testFile.contains("gameObjects");
            bool hasCameras = testFile.contains("Camera");
            bool hasInitialMatrix = testFile.contains("InitialMatrix");
        
            PrintTestResult("Contains GameObjects", hasGameObjects);
            PrintTestResult("Contains CameraObjects", hasCameras);
            PrintTestResult("Contains InitalMatrix", hasInitialMatrix);
        
            allPassed = allPassed && hasGameObjects && hasCameras && hasInitialMatrix;
        }
        rename_file((testName + ".json"), newTestName);

        bool originalGone = !std::ifstream(testName + ".json").good();
        bool renamedExists = std::ifstream(newTestName + ".json").good();

        PrintTestResult("Original file removed after rename", originalGone);
        PrintTestResult("Renamed file exists", renamedExists);
        
        // Clean up
        std::remove((testName + ".json").c_str());
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        PrintTestResult("Serilization Test", false);
        allPassed = false;
    }
    
    SDL_DestroyRenderer(testRenderer);
    SDL_DestroyWindow(testWindow);
    return allPassed;
}

bool TestCollision(){
    bool allPassed = true;
    
    std::cout << "\n--- Testing Collision ---" << std::endl;

    // Initializing fake values needed for test
    SDL_Window* testWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 900, 900, SDL_WINDOW_HIDDEN);
    SDL_Renderer* testRenderer = SDL_CreateRenderer(testWindow, -1, SDL_RENDERER_ACCELERATED);
    
    std::vector<GameObject> fakeGameObjects;
    std::unordered_map<int, GameObject> fakeGameObjectMap;
    fakeGameObjects.push_back(GameObject(testRenderer, "test.png", "test1", 100, 100, 0, 50));
    fakeGameObjects.push_back(GameObject(testRenderer, "test2.png", "test2", 100, 100, 200, 200));
    fakeGameObjects.push_back(GameObject(testRenderer, "test3.png", "test3", 100, 100, 50, 50));
    fakeGameObjectMap[fakeGameObjects[0].GetID()] =  fakeGameObjects[0];
    fakeGameObjectMap[fakeGameObjects[1].GetID()] =  fakeGameObjects[1];
    fakeGameObjectMap[fakeGameObjects[2].GetID()] = fakeGameObjects[2];

    fakeGameObjects[0].AddCollision(testRenderer);
    fakeGameObjects[1].AddCollision(testRenderer);
    fakeGameObjects[2].AddCollision(testRenderer);

    fakeGameObjects[0].addedCollision = true;
    fakeGameObjects[1].addedCollision = true;
    fakeGameObjects[2].addedCollision = true;

    fakeGameObjects[0].RenderCollisionBox(3, 0, 0, 900, 900);
    fakeGameObjects[1].RenderCollisionBox(3, 0, 0, 900, 900);
    fakeGameObjects[2].RenderCollisionBox(3, 0, 0, 900, 900);

    try{
        Collision collider(testRenderer, 100, 300, 150, 90, 255, 100, 50, 255);
        bool collisionInitializedCorrectly = 
        collider._width == 100 && 
        collider._height == 300 &&
        collider._x == 150 &&
        collider._y == 90 &&
        collider._r == 255 &&
        collider._g == 100 &&
        collider._b == 50 &&
        collider._a == 255;

        PrintTestResult("Collision Initialization", collisionInitializedCorrectly);

        allPassed = allPassed && collisionInitializedCorrectly;

        collider.Collision_Render(3, 0, 0, 900, 900);

        int _old_x = collider._x;
        int _old_y = collider._y;

        collider.UpdatePosX(100);
        collider.UpdatePosY(100);

        bool collisionUpdateX = _old_x + 100 == collider._x;
        bool collisionUpdateY = _old_y + 100 == collider._y;

        PrintTestResult("Camera Updating Position X", collisionUpdateX);
        PrintTestResult("Camera Updating Poistion Y", collisionUpdateY);

        allPassed = allPassed && collisionUpdateX && collisionUpdateY;

        bool collisionCheck = fakeGameObjects[0].collisionBox.Collision_Check_Bool(fakeGameObjects[0], fakeGameObjects); 

        PrintTestResult("Bool Collision Check", collisionCheck);
        
        GameObject* obj = fakeGameObjects[0].collisionBox.Collision_Check(fakeGameObjects[0], fakeGameObjects);
        PrintTestResult("Collision return check", obj->GetID() == fakeGameObjects[2].GetID());

        allPassed = allPassed && collisionCheck && (obj->GetID() == fakeGameObjects[2].GetID());

        GameObject* ptr = &fakeGameObjects[1];

        collider.Del(ptr, fakeGameObjects, fakeGameObjectMap);

        bool deletedObject = std::find_if(fakeGameObjects.begin(), fakeGameObjects.end(),
                        [](const GameObject& obj) { return obj.GetID() == 2; }) == fakeGameObjects.end();
        bool deletedObjectMap = fakeGameObjectMap.find(2) == fakeGameObjectMap.end();

        PrintTestResult("Collision Deleted Object vector", deletedObject);
        PrintTestResult("Collision Deleted Object Map", deletedObjectMap);

        allPassed = allPassed && deletedObject && deletedObjectMap;
    }catch(...){
        allPassed = false;
    }

    SDL_DestroyRenderer(testRenderer);
    SDL_DestroyWindow(testWindow);
    return allPassed;
}

bool Testlua_stateFunctions(){
    bool allPassed = true;

    std::cout << "\n--- Testing lua_stateFunctions ---" << std::endl;

    SDL_Window* testWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 900, 900, SDL_WINDOW_HIDDEN);
    SDL_Renderer* testRenderer = SDL_CreateRenderer(testWindow, -1, SDL_RENDERER_ACCELERATED);
    std::vector<GameObject> fakeGameObjects;
    gameObjects.push_back(GameObject(testRenderer, "test.png", "test1", 100, 100, 0, 50));
    gameObjects.push_back(GameObject(testRenderer, "test2.png", "test2", 100, 100, 200, 200));
    gameObjects[0].childrenIDs.push_back(gameObjects[1].GetID());
    GameScreen* gameScreen = new GameScreen(testRenderer);
    GameScreen::InitialMatrix = new GameObject(testRenderer, "default.png", "matrix4778192235010291", 100, 100, 400, 100);

    try{
        sol::state lua_state_state;
        lua_state_state.open_libraries(sol::lib::base, sol::lib::table, sol::lib::math);
        RegisterGameObjectWithlua_state(lua_state_state);
        RegisterGameObjectsWithlua_state(lua_state_state, gameObjects);

        sol::load_result script1 = lua_state_state.load(R"(
            obj = gameObjects["test1"]
            obj.x = 100
            obj.y = 200
            obj.width = 50
            obj.height = 50
            return obj.x, obj.y, obj.width, obj.height
        )");

        if (!script1.valid()) {
            sol::error err = script1;
            std::cerr << "lua load error: " << err.what() << std::endl;
            allPassed = false;
            PrintTestResult("Loading Script", false);
            std::cout << "Skipping further tests on lua Functions.cpp" << std::endl;
            return allPassed;
        }
        
        sol::protected_function_result result = script1();
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "lua execution error: " << err.what() << std::endl;
            allPassed = false;
            PrintTestResult("Executing Script", false);
            std::cout << "Skipping further tests on lua Functions.cpp" << std::endl;
            return allPassed;
        }

        std::tuple<int, int, int, int> values = result;
        auto [xValue, yValue, widthV, heightV] = values;

        bool ValueChanges = gameObjects[0]._x == (xValue + GameScreen::InitialMatrix->_x) && gameObjects[0]._y == -(yValue + (GameScreen::InitialMatrix->_y * -1)) && gameObjects[0]._height == heightV && gameObjects[0]._width == widthV;
        
        PrintTestResult("Checking if script Updated values for gameObject", ValueChanges);
        allPassed = allPassed && ValueChanges;

        sol::load_result script2 = lua_state_state.load(R"(
            obj = gameObjects["test1"].children["test2"]
            return obj.name, obj:GetID()
        )");

        if (!script2.valid()) {
            sol::error err = script2;
            std::cerr << "lua load error: " << err.what() << std::endl;
            allPassed = false;
            PrintTestResult("Loading Script", false);
            std::cout << "Skipping further tests on lua Functions.cpp" << std::endl;
            return allPassed;
        }
        
        sol::protected_function_result result2 = script2();
        if (!result2.valid()) {
            sol::error err = result2;
            std::cerr << "lua execution error: " << err.what() << std::endl;
            allPassed = false;
            PrintTestResult("Executing Script", false);
            std::cout << "Skipping further tests on lua Functions.cpp" << std::endl;
            return allPassed;
        }

        std::tuple<std::string, int> values2 = result2;
        auto [obj_name, obj_id] = values2;

        bool ChildrenCheck = gameObjects[1]._name == obj_name && gameObjects[1].GetID() == obj_id; 
        
        PrintTestResult("Checking if script Works with Children GameObjects", ChildrenCheck);
        allPassed = allPassed && ChildrenCheck;

        assert(lua_state["A_key"] == SDLK_a);
        assert(lua_state["D_key"] == SDLK_d);
        assert(lua_state["W_key"] == SDLK_w);
        assert(lua_state["S_key"] == SDLK_s);
        assert(lua_state["Q_key"] == SDLK_q);
        assert(lua_state["E_key"] == SDLK_e);
        assert(lua_state["F_key"] == SDLK_f);
        assert(lua_state["C_key"] == SDLK_c);
        assert(lua_state["LEFT_key"] == SDLK_LEFT);
        assert(lua_state["RIGHT_key"] == SDLK_RIGHT);
        assert(lua_state["UP_key"] == SDLK_UP);
        assert(lua_state["DOWN_key"] == SDLK_DOWN);
        assert(lua_state["SPACE_key"] == SDLK_SPACE);

        assert(lua["KeycodeToScancode"](SDLK_a) == SDL_GetScancodeFromKey(SDLK_a));
        assert(lua["KeycodeToScancode"](SDLK_d) == SDL_GetScancodeFromKey(SDLK_d));
        assert(lua["KeycodeToScancode"](SDLK_w) == SDL_GetScancodeFromKey(SDLK_w));
        assert(lua["KeycodeToScancode"](SDLK_s) == SDL_GetScancodeFromKey(SDLK_s));
        assert(lua["KeycodeToScancode"](SDLK_q) == SDL_GetScancodeFromKey(SDLK_q));
        assert(lua["KeycodeToScancode"](SDLK_e) == SDL_GetScancodeFromKey(SDLK_e));
        assert(lua["KeycodeToScancode"](SDLK_f) == SDL_GetScancodeFromKey(SDLK_f));
        assert(lua["KeycodeToScancode"](SDLK_c) == SDL_GetScancodeFromKey(SDLK_c));
        assert(lua["KeycodeToScancode"](SDLK_LEFT) == SDL_GetScancodeFromKey(SDLK_LEFT));
        assert(lua["KeycodeToScancode"](SDLK_RIGHT) == SDL_GetScancodeFromKey(SDLK_RIGHT));
        assert(lua["KeycodeToScancode"](SDLK_UP) == SDL_GetScancodeFromKey(SDLK_UP));
        assert(lua["KeycodeToScancode"](SDLK_DOWN) == SDL_GetScancodeFromKey(SDLK_DOWN));
        assert(lua["KeycodeToScancode"](SDLK_SPACE) == SDL_GetScancodeFromKey(SDLK_SPACE));

        PrintTestResult("KeyBinding checks", true);

        sol::load_result script3 = lua_state_state.load(R"(
            toDelete = {}
            obj = gameObjects["test1"]
            obj_test_copy = gameObjects["test2"]
            test_delete_copy_obj = obj:Copy()
            copy_obj = obj_test_copy:Copy()
            table.insert(toDelete, test_delete_copy_obj)
            DeleteGameObjects(toDelete)
            return copy_obj
        )");

        if (!script3.valid()) {
            sol::error err = script3;
            std::cerr << "lua load error: " << err.what() << std::endl;
            allPassed = false;
            PrintTestResult("Loading Script", false);
            std::cout << "Skipping further tests on lua Functions.cpp" << std::endl;
            return allPassed;
        }

        sol::protected_function_result result3 = script3();
        if (!result3.valid()) {
            sol::error err = result3;
            std::cerr << "lua execution error: " << err.what() << std::endl;
            allPassed = false;
            PrintTestResult("Executing Script", false);
            std::cout << "Skipping further tests on lua Functions.cpp" << std::endl;
            return allPassed;
        }

        if (result3.valid()) {
            sol::object copy_obj_lua = result3;
            GameObject* copy_obj = copy_obj_lua.as<GameObject*>();

            assert(gameObjects.size() == 3);
            assert(gameObjects[2]._name == "test2_copy");
            PrintTestResult("Checking if Copy Function worked", true);

            assert(gameObjectsCopy.count(copy_obj->GetID()) == 0);
            PrintTestResult("Checking if DeleteGameObject Function worked", true);
        }else{
            sol::error err = result;
            std::cerr << "Lua script execution failed: " << err.what() << std::endl;
            allPassed = false;
            PrintTestResult("Lua script execution", false);
        }

    }catch(...){
        allPassed = false;
    }

    SDL_DestroyRenderer(testRenderer);
    SDL_DestroyWindow(testWindow);
    return allPassed;
}

// Main test function
int main(int argc, char** argv) {
    std::cout << "==== GAME ENGINE BASIC TESTS ====" << std::endl;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    bool gameObjectsPassed = TestGameObjects();
    bool CameraPassed = TestCameraObject();
    bool serializationPassed = TestSerialization();
    bool CollisionPassed = TestCollision();
    bool lua_stateFunctionsPassed = Testlua_stateFunctions();
    bool allTestsPassed = gameObjectsPassed && CameraPassed && serializationPassed && CollisionPassed && lua_stateFunctionsPassed;
    
    // Print summary
    std::cout << "\n==== TEST SUMMARY ====" << std::endl;
    std::cout << "GameObject Tests: " << (gameObjectsPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Serialization Tests: " << (serializationPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Camera Tests: " << (CameraPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Collision Tests: " << (CollisionPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "lua_stateFunctions Tests: " << (lua_stateFunctionsPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Overall: " << (allTestsPassed ? "PASSED" : "FAILED") << std::endl;
    
    // Clean up
    SDL_Quit();
    
    return allTestsPassed ? 0 : 1;
}