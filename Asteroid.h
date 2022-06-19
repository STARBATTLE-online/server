#pragma once
#include "MovableSprite.h"

class Asteroid : public MovableSprite
{
public:
	Asteroid() = default;
	Asteroid(double sprite_width, double sprite_height, int sprite_mass)
	{
		width = sprite_width;
		height = sprite_height;
		mass = sprite_mass;
	};

	~Asteroid() override = default;

	std::string Serialize() override
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << GetType() << " " << GetCenterGlobal().first << " " << GetCenterGlobal().second;

		return ss.str();
	}
};

class SmallAsteroid : public Asteroid
{
public:
	SmallAsteroid(int x, int y, double speed_x, double speed_y)
	{
		width = 60;
		height = 50;
		mass = 6;
		SetCoordsByCenter(x, y);
		SetSpeed(speed_x, speed_y);
	};

	std::string GetType() override
	{
		return "SmallAsteroid";
	}
};

class BigAsteroid : public Asteroid
{
public:
	BigAsteroid(int x, int y, double speed_x, double speed_y)
	{
		width = 100;
		height = 88;
		mass = 10;
		SetCoordsByCenter(x, y);
		SetSpeed(speed_x, speed_y);
	};

	std::pair<SmallAsteroid, SmallAsteroid> Split()
	{
		SmallAsteroid asteroid1(static_cast<int>(global_x) + 12, static_cast<int>(global_y) - 10, 0, -1.5);
		SmallAsteroid asteroid2(static_cast<int>(global_x) + 12, static_cast<int>(global_y) + 35, 0, 1.5);
		return std::make_pair(asteroid1, asteroid2);
	}

	std::string GetType() override
	{
		return "BigAsteroid";
	}
};
