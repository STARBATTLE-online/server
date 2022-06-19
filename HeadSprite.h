#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>

#include "GlobalVariables.h"

/*
 * Base class for all objects on the map
 */
class HeadSprite {
public:	
	HeadSprite() = default;
	
	virtual ~HeadSprite() = default;

	virtual void setCoords(double new_x, double new_y) {
		global_x = new_x;
		global_y = new_y;
	}

	virtual void setCoordsByCenter(double new_x, double new_y) {
		global_x = new_x - width / 2;
		global_y = new_y - height / 2;
	}

	virtual std::pair<double, double> getCoordsGlobal() {
		return std::make_pair(global_x, global_y);
	}

	virtual std::pair<double, double> getSize() {
		return std::make_pair(width, height);
	}

	virtual double getRadius() {
		return (height + width) / 4.0;
	}

	virtual std::pair<double, double> getCenterGlobal() {
		return std::make_pair(global_x + width / 2, global_y + height / 2);
	}

	double distance(HeadSprite* object) {
		return sqrt(pow(getCenterGlobal().first - object->getCenterGlobal().first, 2)
			+ pow(getCenterGlobal().second - object->getCenterGlobal().second, 2))
			- getRadius() - object->getRadius();
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
	double width;
	double height;
	double global_x;
	double global_y;
};
