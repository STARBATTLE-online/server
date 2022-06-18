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
    static void InitRequest(std::stringstream& ss, std::stringstream& response);
    static void MouseButtonRequest(std::stringstream& ss, std::stringstream& response);
    static void MouseMoveRequest(std::stringstream& ss, std::stringstream& response);
    static void KeyRequest(std::stringstream& ss, std::stringstream& response);
    static void TickRequest(std::stringstream& ss, std::stringstream& response);
};

std::shared_ptr<MapCreator> RequestManager::m_map_creator;

void RequestManager::SetMapCreator(std::shared_ptr<MapCreator> map_creator) {
    RequestManager::m_map_creator = map_creator;
}

std::string RequestManager::ProcessRequest(std::string request)  {
    std::lock_guard<std::mutex> lock(RequestManager::m_map_creator->mt);
    std::stringstream response;
    std::stringstream ss(request);
    std::string output, commandType;

    //std::cout << "Request: " << request;

    while(ss >> commandType) {
        if(commandType == "INIT") {
            InitRequest(ss, response);
        } else if(commandType == "MB") {
            MouseButtonRequest(ss, response);
        } else if(commandType == "KB") {
            KeyRequest(ss, response);
        } else if(commandType == "MM") {
            MouseMoveRequest(ss, response);
        } else if(commandType == "TICK") {
            TickRequest(ss, response);
        } else {
            break;
        }
        response << " ";
    }
    
    std::cout << response.str() << "\n";
    return response.str();
}

void RequestManager::MouseMoveRequest(std::stringstream& ss, std::stringstream& response) {
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
                response << "OK";
                return;
            } else {
                response << "INVALID_PRIVATE_KEY";
                return;
            }
        }
    }

    response << "INVALID_PUBLIC_KEY";
}

void RequestManager::MouseButtonRequest(std::stringstream& ss, std::stringstream& response) {
    uint64_t public_key, private_key;
    std::string mb;
    ss >> mb >> public_key >> private_key;

    if(mb != "L") {
        response << "OK";
        return;
    }

    auto ships = RequestManager::m_map_creator->GetShips();

    for(auto& ship : ships) {
        if(ship->GetPublicKey() == public_key) {
            if(ship->GetPrivateKey() == private_key) {
                RequestManager::m_map_creator->Shoot(ship);
                response << "OK";
                return;
            } else {
                response << "INVALID_PRIVATE_KEY";
                return;
            }
        }
    }

    response << "INVALID_PUBLIC_KEY";
}

void RequestManager::KeyRequest(std::stringstream& ss, std::stringstream& response) {
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
                    response << "INVALID_KEY";
                    return;
                }
                
                response << "OK";
            } else {
                response << "INVALID_PRIVATE_KEY";
            }
            return;
        }
    }

    response << "INVALID_PUBLIC_KEY";
}

void RequestManager::InitRequest(std::stringstream& ss, std::stringstream& response) {
    
    auto r = RequestManager::m_map_creator->AddShip(rand() % 1000, rand() % 1000, Rotation::Top);
    auto coords = r->GetCoordsGlobal();
    response << "INIT " << std::to_string(r->GetPublicKey()) << " " << std::to_string(r->GetPrivateKey()) << " " << std::to_string((int)coords.first) << " " 
            << std::to_string((int)coords.second) << " " << std::to_string(r->GetSpriteID()) << " " << std::to_string(MAP_WIDTH) << " " << std::to_string(MAP_HEIGHT);
}

void RequestManager::TickRequest(std::stringstream& ss, std::stringstream& response) {
    response << "TICK " << RequestManager::m_map_creator->Serialize();
}