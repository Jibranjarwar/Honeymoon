#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <vector>
#include <string>
#include <filesystem>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct TreeNode* GetNewNode(std::string dirName, TreeNode* parent = nullptr);
void addChild (TreeNode* node, std::string dirName);
void Print(TreeNode* root);


struct TreeNode{
    std::string dirName;
    TreeNode* parent;
    std::vector <TreeNode*> children;
    std::vector <std::string> files;
};

void Initialize(int x, int y, int width, int height, SDL_Renderer* renderer);

//std::vector<std::string> GetDirectories(const std::string& root);
std::vector<std::pair<std::string, int>> GetAllDirectories(const std::string& rootPath);
void GetDirectoriesRecursively(const std::filesystem::path& rootPath, std::vector<std::pair<std::string, int>>& directories, int depth = 1);
TreeNode* CreateTree(std::vector<std::pair<std::string, int>>& directories, std::string rootPath);
void RenderTreeNode(TreeNode* root);
SDL_Texture* LoadIconTexure(std::string filename, SDL_Renderer* renderer);
void OpenFileWithDefaultProgram(const std::string& filePath);
std::string SelectImageFile();
//void AddFiles(TreeNode * root, std::string rootPath);