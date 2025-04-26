#include <iostream>
#include <SDL2/SDL.h>
#include "window.h"
#include "gameobject.h"
#include "gamescreen.h"
#include "camera.h"
#include "serilization.h"
#include "gameobjectui.h"
#include "Sol/sol.hpp"
#include <fstream>
#include <vector>
#include <string>

std::vector<GameObject> gameObjects;
sol::state global_lua;
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
    
    try{
        Camera TestCamera(testRenderer, 200, 200, 100, 500, 0, 0, 0, 255);
        TestCamera.Camera_Render(5, 100, 300, 500, 600);
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
    
    try {
        /*std::ofstream testFile("test_data.json");
        testFile << "{\"test\": true}";
        testFile.close();
        
        bool fileExists = std::ifstream("test_data.json").good();
        PrintTestResult("File Creation", fileExists);
        allPassed = allPassed && fileExists;*/
        
        // Setup temporary SDL window and renderer
        SDL_Window* testWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, SDL_WINDOW_HIDDEN);
        SDL_Renderer* testRenderer = SDL_CreateRenderer(testWindow, -1, SDL_RENDERER_ACCELERATED);
        
        std::string testName = "TestProject";
        std::vector<GameObject> fakeGameObjects;
        std::vector<Camera> fakeCameraObjects;
        fakeGameObjects.push_back(GameObject(testRenderer, "test.png", 100, 100, 0, 50));
        fakeGameObjects.push_back(GameObject(testRenderer, "test2.png", 100, 100, 200, 200));
        fakeCameraObjects.push_back(Camera(testRenderer, 200, 200, 100, 500, 0, 0, 0, 255));

        GameScreen* gameScreen = new GameScreen(testRenderer);
        GameScreen::InitialMatrix = new GameObject(testRenderer, "default.png", "matrix4778192235010291", 100, 100, 400, 100);

        tester(testName, fakeGameObjects, fakeCameraObjects, gameScreen);

        bool fileExists = std::ifstream(testName + ".json").good();
        PrintTestResult("File Creation", fileExists);
        allPassed = allPassed && fileExists;
        
        // Clean up
        std::remove((testName + ".json").c_str());
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        PrintTestResult("File Creation", false);
        allPassed = false;
    }
    
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
    bool allTestsPassed = gameObjectsPassed && CameraPassed;
    
    // Print summary
    std::cout << "\n==== TEST SUMMARY ====" << std::endl;
    std::cout << "GameObject Tests: " << (gameObjectsPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Serialization Tests: " << (serializationPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Camera Tests: " << (CameraPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Overall: " << (allTestsPassed ? "PASSED" : "FAILED") << std::endl;
    
    // Clean up
    SDL_Quit();
    
    return allTestsPassed ? 0 : 1;
}