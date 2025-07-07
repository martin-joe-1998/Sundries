// Use ISO C++ 20!!
#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <tinyxml2/tinyxml2.h>

using json = nlohmann::json;

int main()  
{  
    std::ifstream file("testMap.json");
    json mapData;
    file >> mapData;

    int width = mapData["width"];
    int height = mapData["height"];

    // Get "data" from Layer 1
    std::vector<int> flatData = mapData["layers"][0]["data"];

    // Convert to 2d array
    std::vector<std::vector<int>> tileMap(height, std::vector<int>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            tileMap[y][x] = flatData[y * width + x];
        }
    }

    // printing for test
    for (const auto& row : tileMap) {
        for (int id : row) {
            std::cout << id << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";

    // ---------------------------------- tinyxml2 ----------------------------------
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile("OpticalTileset.tsx") != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load tileset\n";
        return 1;
    }

    std::unordered_map<int, std::string> tileIdToFile;

    tinyxml2::XMLElement* tileset = doc.FirstChildElement("tileset");
    for (tinyxml2::XMLElement* tile = tileset->FirstChildElement("tile");
        tile != nullptr;
        tile = tile->NextSiblingElement("tile")) {

        int id = tile->IntAttribute("id");
        tinyxml2::XMLElement* image = tile->FirstChildElement("image");
        const char* source = image->Attribute("source");

        tileIdToFile[id + 1] = source; // *Be careful ：Tiled's tile ID starts from 1
    }

    // Printing for test
    for (const auto& [id, file] : tileIdToFile) {
        std::cout << "Tile ID " << id << ": " << file << "\n";
    }


    return 0;
}