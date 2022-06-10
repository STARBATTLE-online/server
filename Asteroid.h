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
				element->SetSpeed(result.first, result.second);
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
	
private:
	virtual std::pair<double, double> CountCollision(MovableSprite* element) {
		double d = sqrt(pow(GetCenterGlobal().first - element->GetCenterGlobal().first, 2)
			+ pow(GetCenterGlobal().second - element->GetCenterGlobal().second, 2));
		if (d == 0)
		{
			return {0., 0.};
		}

		//���� ������� ��������, ����������� �����������
		double v1_x = (x_speed * (mass - element->GetMass()) + 2 * element->GetMass() * element->GetSpeed().first) / (mass + element->GetMass());
		double v2_x = (element->GetSpeed().first * (element->GetMass() - mass) + 2 * mass * x_speed) / (mass + element->GetMass());
		double v1_y = (y_speed * (mass - element->GetMass()) + 2 * element->GetMass() * element->GetSpeed().second) / (mass + element->GetMass());
		double v2_y = (element->GetSpeed().second * (element->GetMass() - mass) + 2 * mass * y_speed) / (mass + element->GetMass());
		
		SetSpeed(v1_x, v1_y);
		element->SetSpeed(v2_x, v2_y);
		return std::make_pair(v2_x, v2_y);
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
	

private:

};

