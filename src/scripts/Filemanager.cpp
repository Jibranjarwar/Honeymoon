#include "filemanager.h"
#include <iostream>

std::vector<std::pair<std::string, int>> directories;
TreeNode* directory_tree;


void Initialize(int x, int y, int width, int height){
    static ImVec2 lastPosition = ImVec2(-1, -1);
    ImVec2 wantedPosition(x, y);
    
    // if we resize the window we change the the position of the ImGui tab
    if (lastPosition.x != wantedPosition.x || lastPosition.y != wantedPosition.y) {
        ImGui::SetNextWindowPos(wantedPosition);
        lastPosition = wantedPosition;
    }
    ImGui::Begin("File Manager", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
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

    //AddFiles(directory_tree, absolutePath.string());
    ImGui::End();
}

std::vector<std::pair<std::string, int>> GetAllDirectories(const std::string& rootPath) {
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

//Create a Tree Structure 
TreeNode* CreateTree(std::vector<std::pair<std::string, int>>& directories, std::string rootPath){
    
    int current_depth = 1;
    TreeNode* root = GetNewNode(directories[0].first);
    TreeNode* previousNode = root;
    TreeNode* currentNode = root;
    std::string currentPath = rootPath;
    std::string tempPath = currentPath;
    
    for (const auto& [dirName, depth] : std::vector(directories.begin() + 1, directories.end())){
        
        if(current_depth < depth){
            // we went down by 1

            // set the the current node before as the new root since we went down one
            previousNode = currentNode;
            current_depth = depth;
        }
        else if(current_depth > depth){
            // we went back up 1
            
            // size_t used as an unsigned int to get the position in the string
            size_t pos = currentPath.find("\\" + previousNode->dirName);
            
            // if search is NOT found find returns std::string::npos so we check against it
            if (pos != std::string::npos) {
                // Erase from "headers" to the end of the path
                currentPath.erase(pos);
            }

            // we get the parent of the previousNode
            previousNode = previousNode->parent;
            current_depth = depth;
        }
        
        // add child node to parent node
        addChild(previousNode, dirName);

        // set the last item in vector which is the last child as current node since we want it as our last reference
        currentNode = previousNode->children.back();
        
        currentPath += "\\" + currentNode->dirName;
        //std::cout << currentPath << std::endl;
        
        // loop too check all files within currentPath and add them too the current node (basically adding files into correct directories)
        // NOTE BUG: THIS STUMPS PERFORMANCE NEED TO HAVE SOME KINDA CONDITION WHERE IT ONLY EXECUTES WHEN A NEW FILE GETS ADDED 
        for(const auto& entry : std::filesystem::directory_iterator(currentPath)){
            
            //std::cout << entry.path() << std::endl;

            if(entry.is_regular_file()){
                //std::cout << "is file lol" << std::endl;
                currentNode->files.push_back(entry.path().string());
            }
        }
        
        //std::cout << "prev Node: " << previousNode->dirName << " current Node: " << currentNode->dirName << std::endl;
        //std::cout << "dir: " << dirName << " depth: " << depth << std::endl;
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
        
        // prints files in directory that we click
        for (const auto& file : root->files){
            std::cout << file << std::endl;
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

/*void AddFiles(TreeNode * root, std::string rootPath){
    
    
    ImGui::Text("shi");
}*/