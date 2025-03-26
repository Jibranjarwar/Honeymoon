#include "filemanager.h"
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <commdlg.h>

std::vector<std::pair<std::string, int>> directories;
TreeNode* directory_tree;
std::vector<std::filesystem::path> selectedFiles;
std::string current_pressed_dir;
SDL_Texture* file_image = nullptr;

void OpenFileWithDefaultProgram(const std::string& filePath){
    
    // Opens file with users default program NOTE: THIS WORKS FOR WINDOWS NOT TO SURE BOUT LINUX
    ShellExecute(0, "open", filePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

std::string SelectImageFile(){
    
    // Fixes issue where when we get a Image file it changes the current working directory
    // This is so we can re-set it back to what its suppose to be
    char workingDirectory[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, workingDirectory);

    OPENFILENAMEA f = {sizeof(OPENFILENAMEA)};

    char buff[MAX_PATH] = {};

    f.lpstrTitle = "Image File Manager";
    f.lpstrFilter = "png files\0*.png\0jpg files\0*.jpg\0";

    f.nMaxFile = sizeof(buff);
    f.lpstrFile = buff;
    
    // makes sure that the file exists 
    f.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    
    std::string selectedImageFile;
    
    // opens file manager and when file is selected it saves too buff which we then set to selectedImageFile 
    if(GetOpenFileNameA(&f)){
        std::cout << "file name: " << buff << std::endl;
        selectedImageFile = buff;
    }

    // re-sets working Directory back to original
    SetCurrentDirectoryA(workingDirectory);
    return selectedImageFile;
}

void Initialize(int x, int y, int width, int height, SDL_Renderer* renderer){
    static ImVec2 lastPosition = ImVec2(-1, -1);
    ImVec2 wantedPosition(x, y);
    
    // if we resize the window we change the the position of the ImGui tab
    if (lastPosition.x != wantedPosition.x || lastPosition.y != wantedPosition.y) {
        ImGui::SetNextWindowPos(wantedPosition);
        lastPosition = wantedPosition;
    }
    ImGui::Begin("File Manager", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::SetWindowSize(ImVec2(width, height));
    
    // Sidebar width
    float sidebarWidth = width * 0.2f;
    
    // Creates Child Process for Sidebar to navigate folders
    ImGui::BeginChild("Sidebar", ImVec2(sidebarWidth, height - 35), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    //if(directories.empty()){
    
    // NOTE: Could end up having some issues since its called each loop just in case
    
    // Convert the file name to a filesystem path NOTE: HARD CODED RN BUT ITS WILL BE WHERE THE SAVED PROJECT IS STORED
    std::filesystem::path filePath("main.exe");

    // Gets the full path to the file
    std::filesystem::path absolutePath = std::filesystem::absolute(filePath).parent_path();

    //std::cout << absolutePath << std::endl;

    directories = GetAllDirectories(absolutePath.string());
    directory_tree = CreateTree(directories, absolutePath.string());
    //Print(directory_tree);
    
    /*
    INCASE RENDER TREE NODE FAILS
    for (const auto& [dirName, depth] : directories) {
        // Indent based on depth of folder
        if(depth != 0){
            ImGui::Indent(depth * 10);
        }
        
        // Display folder name
        ImGui::Text("%s/", dirName.c_str());
        
        // Unindent based on depth of folder
        if(depth != 0){
            ImGui::Unindent(depth * 10);
        }
    }*/
    
    RenderTreeNode(directory_tree);

    ImGui::EndChild();

    if(file_image == nullptr){
        file_image = LoadIconTexure("C:\\Users\\shvdi\\Documents\\4_Year_Project\\src\\scripts\\file.png", renderer);
    }

    ImGui::SameLine();  // This positions the following text on the same horizontal line as the child
    ImGui::BeginGroup();
    ImGui::Text("Directory: %s", current_pressed_dir.c_str());

    // List all files in the selected directory
    const int filesPerRow = 4;
    const float columnWidth = 100.0f;

    // Start the column layout with a fixed column width
    ImGui::Columns(filesPerRow, nullptr, false);
    
    for (const auto& file : selectedFiles) {
        ImGui::BeginGroup();
        ImGui::Image((void*)file_image, ImVec2(50, 75));

        if(ImGui::IsItemHovered()){
            // Get the top-left corner of the image
            ImVec2 min = ImGui::GetItemRectMin();  
            
            // Get the bottom-right corner of the image
            ImVec2 max = ImGui::GetItemRectMax();  
            
            // Get the window's draw list and draw the rectangle
            ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(39, 119, 245, 255), 5.0f, ImDrawFlags_RoundCornersAll, 2.0f);
        }

        // If Image is clicked then we open the file with whatever default program or a program the user chooses
        if(ImGui::IsItemClicked()){
            OpenFileWithDefaultProgram(file.string());
        }
        
        // Prevent text from overflowing into other columns
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + columnWidth);
        
        ImGui::Text("%s", file.filename().string().c_str());
        
        // Restore text wrapping after the file name
        ImGui::PopTextWrapPos();

        ImGui::EndGroup();
        
        // Move to next column
        ImGui::NextColumn();
    }
    
    // End the column layout and the group
    ImGui::Columns(1);

    ImGui::EndGroup();

    //AddFiles(directory_tree, absolutePath.string());
    ImGui::End();
}

std::vector<std::pair<std::string, int>> GetAllDirectories(const std::string& rootPath) {
    //std::cout << rootPath << std::endl;
    std::filesystem::path rootPath_filesystem = rootPath;
    
    // creates vector with string and int pair
    std::vector<std::pair<std::string, int>> directories;
    
    // add root folder as the first item
    directories.emplace_back(rootPath_filesystem.filename().string(), 0);
    
    // call recursive function
    GetDirectoriesRecursively(rootPath, directories);
    return directories;
}

void GetDirectoriesRecursively(const std::filesystem::path& rootPath, std::vector<std::pair<std::string, int>>& directories, int depth){
    
    //std::cout << rootPath << std::endl;
    try{
        for (const auto& entry : std::filesystem::directory_iterator(rootPath)) {
            
            // checks if its a directory
            if (entry.is_directory()) {
                // Save directory name with depth (used for indentation)
                std::string directoryName = entry.path().filename().string();
                
                // checks wether the directory is hidden and if so skips it
                if(directoryName.front() != '.'){
                    
                    // appends to vector
                    directories.emplace_back(entry.path().filename().string(), depth);
                    
                    // Recursively search inside subdirectories, increasing depth
                    GetDirectoriesRecursively(entry.path(), directories, depth + 1);
                }
            }
        }
    }catch(const std::filesystem::filesystem_error& e){
        std::cerr << "filesystem error: " << e.what() << std::endl;
    }
}

// function to create new node
TreeNode* GetNewNode(std::string dirName, TreeNode* parent){
    TreeNode* newNode = new TreeNode();
    newNode->dirName = dirName;
    newNode->parent = parent;
    return newNode;
}

//function to add child to specific node
void addChild(TreeNode* node, std::string dirName){
    // first create a new node
    TreeNode* newNode = GetNewNode(dirName, node);
    node->children.push_back(newNode);
}

// Create a Tree Structure
TreeNode *CreateTree(std::vector<std::pair<std::string, int>> &directories, std::string rootPath)
{

    int current_depth = 1;
    TreeNode *root = GetNewNode(directories[0].first);
    TreeNode *previousNode = root;
    TreeNode *currentNode = root;
    std::string currentPath = rootPath;
    std::string tempPath = currentPath;

    for (const auto &[dirName, depth] : std::vector(directories.begin() + 1, directories.end()))
    {

        if (current_depth < depth)
        {
            // we went down by 1

            // set the the current node before as the new root since we went down one
            previousNode = currentNode;
            current_depth = depth;
        }
        else if (current_depth > depth)
        {
            // we went back up 1

            // size_t used as an unsigned int to get the position in the string
            size_t pos = currentPath.find("\\" + previousNode->dirName);

            // if search is NOT found find returns std::string::npos so we check against it
            if (pos != std::string::npos)
            {
                // Erase from "headers" to the end of the path
                currentPath.erase(pos);
            }

            // we get the parent of the previousNode
            previousNode = previousNode->parent;
            current_depth = depth;
        }
        std::filesystem::path fsPath(currentPath);
        std::string lastPart = fsPath.filename().string();

        // edge case if we have two folders in same directory where we didnt go into a sub folder
        // causing an issue with no depth change so it didnt apply proper logic without this
        if (current_depth == depth && lastPart != previousNode->dirName)
        {
            // std::cout << "this is current path:" << currentPath << " and prev: " << previousNode->dirName << std::endl;
            // std::cout << "dir Name: " << dirName << std::endl;
            // currentNode = previousNode->parent;

            size_t pos = currentPath.find("\\" + lastPart);

            // if search is NOT found find returns std::string::npos so we check against it
            if (pos != std::string::npos)
            {
                // Erase from "headers" to the end of the path
                currentPath.erase(pos);
            }

            // std::cout << "new path: " << currentPath << std::endl;
        }

        // add child node to parent node
        addChild(previousNode, dirName);

        // set the last item in vector which is the last child as current node since we want it as our last reference
        currentNode = previousNode->children.back();

        currentPath += "\\" + currentNode->dirName;
        // std::cout << currentPath << std::endl;

        // loop too check all files within currentPath and add them too the current node (basically adding files into correct directories)
        for (const auto &entry : std::filesystem::directory_iterator(currentPath, std::filesystem::directory_options::skip_permission_denied))
        {

            // std::cout << entry.path() << std::endl;

            // checks wether file already exists within vector if so skip
            if (std::find(currentNode->files.begin(), currentNode->files.end(), entry.path().string()) == currentNode->files.end())
            {

                // ISSUE: is_regular_file() is what seems to be messing with the performance
                // FIX: I avoided this by checking if entry has an extension which means its a file but some edgecase for this is that
                //      not all files have extensions so could end up with some issues but in our case we need extensions for images, scripts etc

                // checks wether there is a file extensions which is faster than is_regular_file()
                if (entry.path().has_extension())
                {
                    // std::cout << "is file lol" << std::endl;
                    currentNode->files.push_back(entry.path().string());
                }
            }
        }

        // std::cout << "prev Node: " << previousNode->dirName << " current Node: " << currentNode->dirName << std::endl;
        // std::cout << "dir: " << dirName << " depth: " << depth << std::endl;
    }
    return root;
}

void Print(TreeNode * root){
    //std::cout << root->dirName << std::endl;
    for(const auto& child : root->children){
        std::cout << child->dirName << std::endl;
    }
}

// Recursive function to render each node and its children
void RenderTreeNode(TreeNode* root) {
    
    // Start a collapsible tree node for the current node
    bool nodeOpen = ImGui::TreeNodeEx(root->dirName.c_str());
    
    // Check if the text part of the node was clicked
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        
        // Print message when text is clicked, but not when the arrow is clicked
        std::cout << "This is " << root->dirName << std::endl;
        current_pressed_dir = root->dirName;
        
        selectedFiles.clear();
        // prints files in directory that we click
        for (const auto& file : root->files){
            std::cout << file << std::endl;
            std::filesystem::path filePath(file);
            selectedFiles.push_back(filePath);
        }
    }
    // still no clue with what to do for this yet
    else if (ImGui::IsItemClicked()){
        std::cout << "should paste arrow" << std::endl;
    }
    
    // Render children nodes if the node is expanded
    if (nodeOpen) {
        for (const auto& child : root->children) {
            RenderTreeNode(child);
        }
        // Close the tree node
        ImGui::TreePop();
    }
}

SDL_Texture* LoadIconTexure(std::string filename, SDL_Renderer* renderer){

    // Loads PNG or JPG from existing file
    SDL_Surface* tempSurface = IMG_Load(filename.c_str());
    
    // Adds PNG or JPG on too texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    // Frees the Surface from memory to prevent memory leak
    SDL_FreeSurface(tempSurface);
    
    return texture;
}


/*void AddFiles(TreeNode * root, std::string rootPath){
    
    
    ImGui::Text("shi");
}*/