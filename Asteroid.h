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
		else if (GetRadius() + element->GetRadius()
			>= sqrt(pow(GetCenterGlobal().first - element->GetCenterGlobal().first, 2)
			+ pow(GetCenterGlobal().second - element->GetCenterGlobal().second, 2)))
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
		double mass_delta = mass - element->GetMass();
		double new_speed_x = (GetSpeed().first * mass_delta + 2 * element->GetMass() * element->GetSpeed().first) / (mass + element->GetMass());
		double new_speed_y = (GetSpeed().second * mass_delta + 2 * element->GetMass() * element->GetSpeed().second) / (mass + element->GetMass());

		double new_speed_x_element = (-element->GetSpeed().first * mass_delta + 2 * mass * GetSpeed().first) / (mass + element->GetMass());
		double new_speed_y_element = (-element->GetSpeed().second * mass_delta + 2 * mass * GetSpeed().second) / (mass + element->GetMass());

		SetSpeed(new_speed_x, new_speed_y);
		element->SetSpeed(new_speed_x_element, new_speed_y_element);

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

