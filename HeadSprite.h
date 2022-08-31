#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

#include "Logger.hpp"
#include "GlobalVariables.h"

/**
 * @brief HeadSprite is used as a base for all objects that might be rendered.
 */
class HeadSprite {
public:	
	HeadSprite() = default;
	
	virtual ~HeadSprite() = default;

	/*
	static void createMessage(){
		Logger::log("Creating a HeadSprite");
	}

	static void deleteMessage(){
		Logger::log("Deleting a HeadSprite");
	}

	void * operator new(size_t size)
	{
		//cout << "New operator overloading " << endl;
		createMessage();
		void * p = ::operator new(size);
		return p;
	}
	
	void operator delete(void * p)
	{
		deleteMessage();
		//cout << "Delete operator overloading " << endl;
		::operator delete(p);
	}
	*/

	virtual void setCoords(double new_x, double new_y) {
		global_x = new_x;
		global_y = new_y;
	}

	virtual void setCoordsByCenter(double new_x, double new_y) {
		global_x = new_x - width / 2;
		global_y = new_y - height / 2;
	}

	std::pair<double, double> getCoordsGlobal() const {
		return std::make_pair(global_x, global_y);
	}

	std::pair<double, double> getSize() const  {
		return std::make_pair(width, height);
	}

	virtual double getRadius() {
		return (height + width) / 4.0;
	}

	std::pair<double, double> getCenterGlobal() const {
		return std::make_pair(global_x + width / 2, global_y + height / 2);
	}

	double distance(HeadSprite* object) {
		return sqrt(pow(getCenterGlobal().first - object->getCenterGlobal().first, 2)
			+ pow(getCenterGlobal().second - object->getCenterGlobal().second, 2))
			- getRadius() - object->getRadius();
	}

	double realDistance(HeadSprite* object) {
		double dist = 100500.;
		for(int i = -1; i <= 1; ++i) {
			for(int j = -1; j <= 1; ++j) {
				dist = std::min(dist, (double)sqrt(pow(getCenterGlobal().first - object->getCenterGlobal().first + i * MAP_WIDTH, 2)
					+ pow(getCenterGlobal().second - object->getCenterGlobal().second + j * MAP_HEIGHT, 2)));
			}
		}

		return dist;
	}

	virtual std::string getType() {
		return "HeadSprite";
	}

	virtual std::string serialize() {
		std::stringstream ss;
		ss << "MovableSprite" << " " << global_x << " " << global_y << " ";
		return ss.str();
	}

protected:
	double width = 0.;
	double height = 0.;
	double global_x = 0.;
	double global_y = 0.;
};
