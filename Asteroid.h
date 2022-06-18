#pragma once
#include "MovableSprite.h"

class Asteroid : public MovableSprite 
{
public:
	Asteroid() = default;
	Asteroid(double sprite_width, double sprite_height, int sprite_mass) {
		width = sprite_width;
		height = sprite_height;
		mass = sprite_mass;
	};

	~Asteroid() override = default;

	virtual bool CheckCollision(MovableSprite* element) {
		if (element == this)
		{
			return false;
		}
		else if (Distance(element) < 0)
		{
			if (dynamic_cast<Asteroid*>(element))
			{
				std::pair<double, double> result = CountCollision(element);
				//element->SetSpeed(result.first, result.second);
				return true;
			}
			else 
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	std::string Serialize() override {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << GetType() << " " << GetCenterGlobal().first << " " << GetCenterGlobal().second;

		return ss.str();
	}
	
private:
	virtual std::pair<double, double> CountCollision(MovableSprite* element) {
		double distanceSquared = pow(GetCenterGlobal().first - element->GetCenterGlobal().first, 2)
			+ pow(GetCenterGlobal().second - element->GetCenterGlobal().second, 2);

		double distance = sqrt(distanceSquared);

		double overlap = (distance - (GetRadius() + element->GetRadius())) / 2.;

		double moveX = (overlap * (GetCenterGlobal().first - element->GetCenterGlobal().first)) / distance;
		double moveY = (overlap * (GetCenterGlobal().second - element->GetCenterGlobal().second)) / distance;

		SetCoordsByCenter(GetCenterGlobal().first - moveX, GetCenterGlobal().second - moveY);
		element->SetCoordsByCenter(element->GetCenterGlobal().first + moveX, element->GetCenterGlobal().second + moveY);

		// Find normal vector
		double normalX = -(GetCenterGlobal().first - element->GetCenterGlobal().first) / distance;
		double normalY = -(GetCenterGlobal().second - element->GetCenterGlobal().second) / distance;

		// Find tangent vector
		double tangentX = -normalY;
		double tangentY = normalX;

		double dotProductTangent1 = tangentX * GetSpeed().first + tangentY * GetSpeed().second;
		double dotProductTangent2 = tangentX * element->GetSpeed().first + tangentY * element->GetSpeed().second;
		
		double dotProductNormal1 = normalX * GetSpeed().first + normalY * GetSpeed().second;
		double dotProductNormal2 = normalX * element->GetSpeed().first + normalY * element->GetSpeed().second;

		double m1 = (dotProductNormal1 * (GetMass() - element->GetMass()) + 2 * element->GetMass() * dotProductNormal2) / (GetMass() + element->GetMass());
		double m2 = (dotProductNormal2 * (element->GetMass() - GetMass()) + 2 * GetMass() * dotProductNormal1) / (GetMass() + element->GetMass());

		SetSpeed(tangentX * dotProductTangent1 + normalX * m1, tangentY * dotProductTangent1 + normalY * m1);
		element->SetSpeed(tangentX * dotProductTangent2 + normalX * m2, tangentY * dotProductTangent2 + normalY * m2);

		TruncateSpeed(3);
		element->TruncateSpeed(3);

		return {0, 0};
	}	
};

class SmallAsteroid : public Asteroid
{
public:
    SmallAsteroid(int x, int y, double speed_x, double speed_y) {
        width = 44;
        height = 36;
        mass = 6;
        SetCoordsByCenter(x, y);
        SetSpeed(speed_x, speed_y);
    };

	std::string GetType() override {
		return "SmallAsteroid";
	}

private:

};

class BigAsteroid : public Asteroid
{
public:
	BigAsteroid(int x, int y, double speed_x, double speed_y) {
		width = 68;
		height = 60;
		mass = 10;
		SetCoordsByCenter(x, y);
		SetSpeed(speed_x, speed_y);
	};
	
	std::pair<SmallAsteroid, SmallAsteroid> Split() {
		SmallAsteroid asteroid1(static_cast<int>(global_x) + 12, static_cast<int>(global_y) - 10, 0, -1.5);
		SmallAsteroid asteroid2(static_cast<int>(global_x) + 12, static_cast<int>(global_y) + 35, 0, 1.5);
		return std::make_pair(asteroid1, asteroid2);		
	}
	
	std::string GetType() override {
		return "BigAsteroid";
	}

private:

};

