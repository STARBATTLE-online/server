#pragma once
#include "HeadSprite.h"

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

	virtual void SetSpeed(double new_x_speed, double new_y_speed)
	{
		x_speed = new_x_speed;
		y_speed = new_y_speed;
	}

	virtual void Move()
	{
		global_y += y_speed;
		global_x += x_speed;
		Border();
	}

	virtual std::pair<double, double> GetSpeed()
	{
		return std::make_pair(y_speed, x_speed);
	}

	virtual double GetMass()
	{
		return mass;
	}

	virtual void Border()
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

	std::string GetType() override
	{
		return "MovableSprite";
	}

	void TruncateSpeed(double limit)
	{
		double speed = sqrt(pow(GetSpeed().first, 2) + pow(GetSpeed().second, 2));

		if (speed < limit)
			return;

		double newX = GetSpeed().first / speed;
		double newY = GetSpeed().second / speed;

		SetSpeed(newX * limit, newY * limit);
	}

	virtual bool CheckCollision(MovableSprite *element)
	{
		if (element == this)
		{
			return false;
		}
		else if (Distance(element) < 0)
		{
			std::pair<double, double> result = Collide(element);
			// element->SetSpeed(result.first, result.second);
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual std::pair<double, double> Collide(MovableSprite *element)
	{
		double distanceSquared = pow(GetCenterGlobal().first - element->GetCenterGlobal().first, 2) + pow(GetCenterGlobal().second - element->GetCenterGlobal().second, 2);

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

protected:
	double y_speed = 0.;
	double x_speed = 0.;

	int mass = 1;
};