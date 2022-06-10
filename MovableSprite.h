#pragma once
#include "HeadSprite.h"

class MovableSprite : public HeadSprite {
public:
	MovableSprite() = default;

	MovableSprite(double sprite_width, double sprite_height) {
		width = sprite_width;
		height = sprite_height;
	};

	~MovableSprite() override = default;

	virtual void SetSpeed(double new_x_speed, double new_y_speed) {
		x_speed = new_x_speed;
		y_speed = new_y_speed;
	}

	virtual void Move() {
		global_y += y_speed;
		global_x += x_speed;
		Border();
	}

	virtual std::pair<double, double> GetSpeed() {
		return std::make_pair(y_speed, x_speed);
	}

	virtual double GetMass() {
		return mass;
	}

	virtual void Border() {

		if (global_x + width / 2 >= MAP_WIDTH)
		{
			global_x = -1 * (width / 2);
		}
		else if (global_x < (width / 2) * -1)
		{
			global_x = MAP_WIDTH - width / 2;
		}

		if (global_y + height / 2 >= MAP_HEIGHT)
		{
			global_y = -1 * (height / 2);
		}
		else if (global_y < (height / 2) * -1)
		{
			global_y = MAP_HEIGHT - height / 2;
		}

	}

protected:
	double y_speed = 0.;
	double x_speed = 0.;

	int mass = 1;
};