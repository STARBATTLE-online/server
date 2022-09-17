#pragma once
#include <iostream>
#include <memory>
#include "World.h"

#include "Database.h"

/**
 * @brief RequestManager is used to create responses; singleton
 */
class RequestManager {
    static std::shared_ptr<World> m_map_creator;
public:
    static void setWorld(std::shared_ptr<World> map_creator);

    static std::string processRequest(const std::string& request);
    static void initRequest(std::stringstream& ss, std::stringstream& response);
    static void mouseButtonRequest(std::stringstream& ss, std::stringstream& response);
    static void mouseMoveRequest(std::stringstream& ss, std::stringstream& response);
    static void keyRequest(std::stringstream& ss, std::stringstream& response);
    static void tickRequest(std::stringstream& ss, std::stringstream& response);
    static void closeRequest(std::stringstream& ss, std::stringstream& response);


    static void registerRequest(std::stringstream& ss, std::stringstream& response);
    static void loginRequest(std::stringstream& ss, std::stringstream& response);
};

std::shared_ptr<World> RequestManager::m_map_creator;

void RequestManager::setWorld(std::shared_ptr<World> map_creator) {
    RequestManager::m_map_creator = map_creator;
}

std::string RequestManager::processRequest(const std::string& request)  {
    std::lock_guard<std::mutex> lock(RequestManager::m_map_creator->mt);
    std::stringstream response;
    std::stringstream ss(request);
    std::string output, commandType;

    while(ss >> commandType) {
        if(commandType == "INIT") {
            initRequest(ss, response);
        } else if(commandType == "MB") {
            mouseButtonRequest(ss, response);
        } else if(commandType == "KB") {
            keyRequest(ss, response);
        } else if(commandType == "MM") {
            mouseMoveRequest(ss, response);
        } else if(commandType == "TICK") {
            tickRequest(ss, response);
        } else if(commandType == "CLOSE") {
            closeRequest(ss, response);
        } else {
            break;
        }
        response << " ";
    }

    return response.str();
}

void RequestManager::mouseMoveRequest(std::stringstream& ss, std::stringstream& response) {
    uint64_t public_key, private_key;
    double mouse_x, mouse_y;
    std::string rotation;
    ss >> mouse_x >> mouse_y;
    ss >> rotation;
    ss >> public_key >> private_key;
    auto ships = RequestManager::m_map_creator->getShips();

    for(auto& ship : ships) {
        if(ship->getPublicKey() == public_key) {
            if(ship->getPrivateKey() == private_key) {
                ship->sendMouseMoveEvent(mouse_x, mouse_y);
                ship->setRotation(RotationFromString(rotation));
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

void RequestManager::closeRequest(std::stringstream& ss, std::stringstream& response) {
    uint64_t public_key, private_key;
    ss >> public_key >> private_key;
    for(auto it = RequestManager::m_map_creator->getShips().begin(); it != RequestManager::m_map_creator->getShips().end(); ++it) {
        auto ship = *it;
        if(ship->getPublicKey() == public_key) {
            if(ship->getPrivateKey() == private_key) {
                RequestManager::m_map_creator->getShips().erase(it);
                delete ship;
                return;
            }
        }
    }
}

void RequestManager::mouseButtonRequest(std::stringstream& ss, std::stringstream& response) {
    uint64_t public_key, private_key;
    std::string mb;
    ss >> mb >> public_key >> private_key;

    auto ships = RequestManager::m_map_creator->getShips();

    if(mb == "L") {
        for(auto& ship : ships) {
            if(ship->getPublicKey() == public_key) {
                if(ship->getPrivateKey() == private_key) {
                    RequestManager::m_map_creator->shoot(ship);
                    response << "OK";
                    return;
                } else {
                    response << "INVALID_PRIVATE_KEY";
                    return;
                }
            }
        }
    } else {
        for(auto& ship : ships) {
            if(ship->getPublicKey() == public_key) {
                if(ship->getPrivateKey() == private_key) {
                    RequestManager::m_map_creator->rightClick(ship);
                    response << "OK";
                    return;
                } else {
                    response << "INVALID_PRIVATE_KEY";
                    return;
                }
            }
        }
    }

    response << "INVALID_PUBLIC_KEY";
}

void RequestManager::keyRequest(std::stringstream& ss, std::stringstream& response) {
    uint64_t public_key, private_key;
    std::string key;
    ss >> key >> public_key >> private_key;
    auto ships = RequestManager::m_map_creator->getShips();

    for(auto& ship : ships) {
        if(ship->getPublicKey() == public_key) {
            if(ship->getPrivateKey() == private_key) {
                if(key == "L") {
                    ship->setMovementDirection(FRKey::LEFT);
                } else if(key == "R") {
                    ship->setMovementDirection(FRKey::RIGHT);
                } else if(key == "U") {
                    ship->setMovementDirection(FRKey::UP);
                } else if(key == "D") {
                    ship->setMovementDirection(FRKey::DOWN);
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

void RequestManager::initRequest(std::stringstream& ss, std::stringstream& response) {
    
    auto r = RequestManager::m_map_creator->addShip(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, Rotation::Top);
    auto coords = r->getCoordsGlobal();
    response << "INIT " << gameVersion << " " << std::to_string(r->getPublicKey()) << " " << std::to_string(r->getPrivateKey()) << " " << std::to_string((int)coords.first) << " " 
            << std::to_string((int)coords.second) << " " << std::to_string(r->getSpriteID()) << " " << std::to_string(MAP_WIDTH) << " " << std::to_string(MAP_HEIGHT);
}

void RequestManager::tickRequest(std::stringstream& ss, std::stringstream& response) {
    uint64_t public_key;
    ss >> public_key;
    auto r = RequestManager::m_map_creator->getShipByPublicID(public_key);
    if(r) r->resetTicksSinceLastSeen();
    response << "TICK " << RequestManager::m_map_creator->serialize();
}

void RequestManager::registerRequest(std::stringstream& ss, std::stringstream& response) {
    
}

void RequestManager::loginRequest(std::stringstream& ss, std::stringstream& response) {
    
}