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
    static std::string MouseMoveRequest(std::stringstream& ss);
    static std::string KeyRequest(std::stringstream& ss);
    static std::string TickRequest(std::stringstream& ss);
};

std::shared_ptr<MapCreator> RequestManager::m_map_creator;

void RequestManager::SetMapCreator(std::shared_ptr<MapCreator> map_creator) {
    RequestManager::m_map_creator = map_creator;
}

std::string RequestManager::ProcessRequest(std::string request)  {
    std::lock_guard<std::mutex> lock(RequestManager::m_map_creator->mt);
    std::stringstream ss(request);
    std::string output, commandType;

    //std::cout << "Request: " << request;

    while(ss >> commandType) {
        if(commandType == "INIT") {
            output += InitRequest(ss) + " ";
        } else if(commandType == "MB") {
            output += MouseButtonRequest(ss) + " ";
        } else if(commandType == "KB") {
            output += KeyRequest(ss) + " ";
        } else if(commandType == "MM") {
            output += MouseMoveRequest(ss) + " ";
        } else if(commandType == "TICK") {
            output += TickRequest(ss) + " ";
        } else {
            output += "UNKNOWN_COMMAND";
            break;
        }
    }

    //std::cout << "Response: " << output << std::endl;
    return output;
}

std::string RequestManager::MouseMoveRequest(std::stringstream& ss) {
    uint64_t public_key, private_key;
    double mouse_x, mouse_y;
    std::string rotation;
    ss >> mouse_x >> mouse_y;
    ss >> rotation;
    ss >> public_key >> private_key;
    auto ships = RequestManager::m_map_creator->GetShips();

    for(auto& ship : ships) {
        if(ship->GetPublicKey() == public_key) {
            if(ship->GetPrivateKey() == private_key) {
                ship->SendMouseMoveEvent(mouse_x, mouse_y);
                ship->SetRotation(RotationFromString(rotation));
                return "OK";
            } else {
                return "INVALID_PRIVATE_KEY";
            }
        }
    }

    return "INVALID_PUBLIC_KEY";
}

std::string RequestManager::MouseButtonRequest(std::stringstream& ss) {
    uint64_t public_key, private_key;
    std::string mb;
    ss >> mb >> public_key >> private_key;

    if(mb != "L") {
        return "OK";
    }

    auto ships = RequestManager::m_map_creator->GetShips();

    for(auto& ship : ships) {
        if(ship->GetPublicKey() == public_key) {
            if(ship->GetPrivateKey() == private_key) {
                ship->Shoot();
                return "OK";
            } else {
                return "INVALID_PRIVATE_KEY";
            }
        }
    }

    return "INVALID_PUBLIC_KEY";
}

std::string RequestManager::KeyRequest(std::stringstream& ss) {
    uint64_t public_key, private_key;
    std::string key;
    ss >> key >> public_key >> private_key;
    auto ships = RequestManager::m_map_creator->GetShips();

    for(auto& ship : ships) {
        if(ship->GetPublicKey() == public_key) {
            if(ship->GetPrivateKey() == private_key) {
                if(key == "L") {
                    ship->MoveManual(FRKey::LEFT);
                } else if(key == "R") {
                    ship->MoveManual(FRKey::RIGHT);
                } else if(key == "U") {
                    ship->MoveManual(FRKey::UP);
                } else if(key == "D") {
                    ship->MoveManual(FRKey::DOWN);
                } else {
                    return "INVALID_KEY";
                }
                
                return "OK";
            } else {
                return "INVALID_PRIVATE_KEY";
            }
        }
    }

    return "INVALID_PUBLIC_KEY";
}

std::string RequestManager::InitRequest(std::stringstream& ss) {
    
    auto r = RequestManager::m_map_creator->AddShip(rand() % 1000, rand() % 1000, Rotation::Top);
    auto coords = r->GetCoordsGlobal();
    return "INIT " + std::to_string(r->GetPublicKey()) + " " + std::to_string(r->GetPrivateKey()) + " " + std::to_string((int)coords.first) + " " 
            + std::to_string((int)coords.second) + " " + std::to_string(r->GetSpriteID()) + " " + std::to_string(MAP_WIDTH) + " " + std::to_string(MAP_HEIGHT);
}

std::string RequestManager::TickRequest(std::stringstream& ss) {
    return "TICK " + RequestManager::m_map_creator->Serialize();
}