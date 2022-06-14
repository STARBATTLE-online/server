#pragma once
#include <iostream>

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


std::string ProcessRequest(std::string request) {
    std::cout << "Request: " << request << std::endl;
    return request;
}

class RequestFactory {
    
};