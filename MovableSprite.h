#pragma once
#include "HeadSprite.h"

/**
 * @brief MovableSprite is used for all the objects that might move or need to resolve collisions.
 */
class MovableSprite : public HeadSprite
{
public:
	MovableSprite() = default;

	MovableSprite(double sprite_width, double sprite_height)
	{
		width = sprite_width;
		height = sprite_height;
	};

	~MovableSprite() override = default;

	virtual void setSpeed(double new_x_speed, double new_y_speed)
	{
		x_speed = new_x_speed;
		y_speed = new_y_speed;
	}

	virtual void move()
	{
		global_y += y_speed;
		global_x += x_speed;
		border();
	}

	virtual std::pair<double, double> getSpeed()
	{
		return std::make_pair(y_speed, x_speed);
	}

	virtual double getMass()
	{
		return mass;
	}

	virtual void border()
	{

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

	std::string getType() override
	{
		return "MovableSprite";
	}

	void truncateSpeed(double limit)
	{
		double speed = sqrt(pow(getSpeed().first, 2) + pow(getSpeed().second, 2));

		if (speed < limit)
			return;

		double newX = getSpeed().first / speed;
		double newY = getSpeed().second / speed;

		setSpeed(newX * limit, newY * limit);
	}

	void minimalSpeedLimit(double limit)
	{
		double speed = sqrt(pow(getSpeed().first, 2) + pow(getSpeed().second, 2));
		
		if (speed > limit)
			return;

		double newX = getSpeed().first / speed;
		double newY = getSpeed().second / speed;

		setSpeed(newX * limit, newY * limit);
	}

	virtual bool collisionDetector(MovableSprite *element)
	{
		if(element == this) return false;
		auto original = element->getCenterGlobal();
		element->setCoordsByCenter(element->getCenterGlobal().first - MAP_WIDTH, element->getCenterGlobal().second - MAP_HEIGHT);
		for(int i = -1; i <= 1; ++i) {
			for(int j = -1; j <= 1; ++j) {
				auto r = distance(element);
				if(std::abs(r) <= 1e-2) return true;
				if (r < 0)
				{
					if (dynamic_cast<MovableSprite *>(element))
					{
						std::pair<double, double> result = collide(element);

						element->setCoordsByCenter(original.first, original.second);
						return true;
					}
					else
					{
						element->setCoordsByCenter(original.first, original.second);
						return true;
					}
				}

				element->setCoordsByCenter(element->getCenterGlobal().first, element->getCenterGlobal().second + MAP_HEIGHT);
			}
			element->setCoordsByCenter(element->getCenterGlobal().first + MAP_WIDTH, element->getCenterGlobal().second - 2 * MAP_HEIGHT);
		}
		element->setCoordsByCenter(original.first, original.second);
		return false;
	}

	virtual std::pair<double, double> collide(MovableSprite *element)
	{
		double distanceSquared = pow(getCenterGlobal().first - element->getCenterGlobal().first, 2) + pow(getCenterGlobal().second - element->getCenterGlobal().second, 2);

		double distance = sqrt(distanceSquared);

		double overlap = (distance - (getRadius() + element->getRadius())) / 2.;

		double moveX = (overlap * (getCenterGlobal().first - element->getCenterGlobal().first)) / distance * 2;
		double moveY = (overlap * (getCenterGlobal().second - element->getCenterGlobal().second)) / distance * 2;

		setCoordsByCenter(getCenterGlobal().first - moveX, getCenterGlobal().second - moveY);
		element->setCoordsByCenter(element->getCenterGlobal().first + moveX, element->getCenterGlobal().second + moveY);

		// Find normal vector
		double normalX = -(getCenterGlobal().first - element->getCenterGlobal().first) / distance;
		double normalY = -(getCenterGlobal().second - element->getCenterGlobal().second) / distance;

		// Find tangent vector
		double tangentX = -normalY;
		double tangentY = normalX;

		double dotProductTangent1 = tangentX * getSpeed().first + tangentY * getSpeed().second;
		double dotProductTangent2 = tangentX * element->getSpeed().first + tangentY * element->getSpeed().second;

		double dotProductNormal1 = normalX * getSpeed().first + normalY * getSpeed().second;
		double dotProductNormal2 = normalX * element->getSpeed().first + normalY * element->getSpeed().second;

		double m1 = (dotProductNormal1 * (getMass() - element->getMass()) + 2 * element->getMass() * dotProductNormal2) / (getMass() + element->getMass());
		double m2 = (dotProductNormal2 * (element->getMass() - getMass()) + 2 * getMass() * dotProductNormal1) / (getMass() + element->getMass());

		setSpeed(tangentX * dotProductTangent1 + normalX * m1, tangentY * dotProductTangent1 + normalY * m1);
		element->setSpeed(tangentX * dotProductTangent2 + normalX * m2, tangentY * dotProductTangent2 + normalY * m2);

		truncateSpeed(getSpeedCeiling());
		element->truncateSpeed(element->getSpeedCeiling());

		minimalSpeedLimit(1);
		element->minimalSpeedLimit(1);

		return {0, 0};
	}

	virtual double getSpeedCeiling() {
		return 3.;
	}

	virtual uint64_t getDestructionScore() {
		return 1;
	}
protected:
	double y_speed = 0.;
	double x_speed = 0.;

	int mass = 1;
};