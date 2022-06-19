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

	std::string serialize() override
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << getType() << " " << getCenterGlobal().first << " " << getCenterGlobal().second;

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
		setCoordsByCenter(x, y);
		setSpeed(speed_x, speed_y);
	};

	std::string getType() override
	{
		return "SmallAsteroid";
	}

	uint64_t getDestructionScore() override {
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
		setCoordsByCenter(x, y);
		setSpeed(speed_x, speed_y);
	};

	std::string getType() override
	{
		return "BigAsteroid";
	}

	uint64_t getDestructionScore() override {
		return 2.;
	}
};
