#pragma once
#include <iostream>
#include <vector>
#include <cmath>

#include "GlobalVariables.h"

class HeadSprite {
public:	
	HeadSprite() {};
	
	virtual ~HeadSprite(){};

	virtual void SetCoords(double new_x, double new_y) {
		global_x = new_x;
		global_y = new_y;
	}

	virtual void SetCoordsByCenter(double new_x, double new_y) {
		global_x = new_x - width / 2;
		global_y = new_y - height / 2;
	}

	virtual std::pair<double, double> GetCoordsGlobal() {
		return std::make_pair(global_x, global_y);
	}

	virtual std::pair<double, double> GetSize() {
		return std::make_pair(width, height);
	}

	virtual double GetRadius() {
		return (height + width) / 4.0;
	}

	virtual std::pair<double, double> GetCenterGlobal() {
		return std::make_pair(global_x + width / 2, global_y + height / 2);
	}

	virtual  void SendMouseMoveEvent(...) {};

	virtual  void SendMouseClickEvent(...) {};
	
	virtual  void SendKeyPressEvent(...) {};

	double Distance(HeadSprite* object) {
		return sqrt(pow(GetCenterGlobal().first - object->GetCenterGlobal().first, 2)
			+ pow(GetCenterGlobal().second - object->GetCenterGlobal().second, 2))
			- GetRadius() - object->GetRadius();
	}
	
protected:
	double width;
	double height;
	double global_x;
	double global_y;
};
