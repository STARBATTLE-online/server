#pragma once
#include <iostream>
#include <memory>
#include "MapCreator.h"

// Request format:

// MAP_WIDTH, MAP_HEIGHT
// asteroid_count, ship_count
// for each asteroid:
//  ASTEROID_TYPE
//  serialize(asteroid)
// for each ship:
//  SHIP
//  serialize(ship)
//  

// Singleton
class RequestManager {
    static std::shared_ptr<MapCreator> m_map_creator;
public:
    static void SetMapCreator(std::shared_ptr<MapCreator> map_creator);

    static std::string ProcessRequest(std::string request);
    static std::string InitRequest(std::stringstream& ss);
    static std::string MouseButtonRequest(std::stringstream& ss);
    static std::string KeyRequest(std::stringstream& ss);
};

std::shared_ptr<MapCreator> RequestManager::m_map_creator;

void RequestManager::SetMapCreator(std::shared_ptr<MapCreator> map_creator) {
    RequestManager::m_map_creator = map_creator;
}

std::string RequestManager::ProcessRequest(std::string request)  {
    std::lock_guard<std::mutex> lock(RequestManager::m_map_creator->mt);
    std::stringstream ss(request);
    std::string output, commandType;

    std::cout << request << std::endl;

    while(ss >> commandType) {
        if(commandType == "INIT") {
            output += InitRequest(ss);
        } else if(commandType == "MB") {
            output += MouseButtonRequest(ss);
        } else if(commandType == "KB") {
            output += KeyRequest(ss);
        } else {
            output += "UNKNOWN_COMMAND";
            break;
        }
    }

    return output;
}

std::string RequestManager::MouseButtonRequest(std::stringstream& ss) {

}

std::string RequestManager::KeyRequest(std::stringstream& ss) {

}

std::string RequestManager::InitRequest(std::stringstream& ss) {
    auto r = RequestManager::m_map_creator->AddShip(rand() % 1000, rand() % 1000, Rotation::Top);
    auto coords = r->GetCoordsGlobal();
    return { "INIT " + std::to_string(r->GetPublicKey()) + " " + std::to_string(r->GetPrivateKey()) + " " + std::to_string(coords.first) + " " 
            + std::to_string(coords.second) + " " + std::to_string(r->GetSpriteID()) };
}