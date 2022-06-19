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

	uint64_t GetDestructionScore() override {
		return 5.;
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

	std::string GetType() override
	{
		return "BigAsteroid";
	}

	uint64_t GetDestructionScore() override {
		return 2.;
	}
};
