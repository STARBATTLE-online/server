#pragma once
#include "MovableSprite.h"
#include "SuperPower.h"
#include "MapCreator.h"
#include "Rotation.h"

//????? ????? ??? ?????, ???? ?? ???????? ???????
class Bullet : public MovableSprite
{
public:
	Bullet(int x, int y)
	{
		width = 13;
		height = 13;
		global_x = x;
		global_y = y;
		mass = 1;
	};

	Bullet(int x, int y, double x_speed, double y_speed)
	{
		width = 13;
		height = 13;
		global_x = x;
		global_y = y;
		this->x_speed = x_speed;
		this->y_speed = y_speed;
	};

	std::string Serialize() override
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << "Bullet " << global_x << " " << global_y << " " << x_speed << " " << y_speed;

		return ss.str();
	}

	~Bullet(){};

	int lifespan = 80;
	uint64_t sender_id = 0;
};

class Ship : public MovableSprite
{
public:
	Ship(){};
	Ship(double sprite_width, double sprite_height)
	{
		width = sprite_width;
		height = sprite_height;

		private_key = rand();
		public_key = rand();

		sprite_id = 3; // chosen by fair dice roll. guarenteed to be random

		mass = 5;
	};
	~Ship() override = default;

	void Move() override
	{
		global_y += y_speed;
		global_x += x_speed;
		Border();
		UseImpulse();
		if (reload_time)
			reload_time--;
		// THIS SEGFEAULTS DONT UNCOMMENT THIS LINE
		/*
			if (dynamic_cast<Shield*>(power))
			{
				power->SetCoordsByCenter(GetCenterGlobal().first, GetCenterGlobal().second);
			}
		*/
	}



	void PowersHandler()
	{
		if (dynamic_cast<Shield *>(power))
		{
			power->SetCoords(GetCenterGlobal().first - power->GetSize().first / 2, GetCenterGlobal().second - power->GetSize().second / 2);
		}
		else if (dynamic_cast<Rocket *>(power))
		{
			//
		}
	}

	void UseImpulse()
	{
		x_speed /= impulse;
		y_speed /= impulse;
		if (abs(x_speed) < control_impulse)
		{
			x_speed = 0;
		}
		if (abs(y_speed) < control_impulse)
		{
			y_speed = 0;
		}
	}

	void MoveManual(FRKey k)
	{
		switch (k)
		{
		case FRKey::RIGHT:
			x_speed = engine_power_speed;
			break;
		case FRKey::LEFT:
			x_speed = -1 * engine_power_speed;
			break;
		case FRKey::DOWN:
			y_speed = engine_power_speed;
			break;
		case FRKey::UP:
			y_speed = -1 * engine_power_speed;
			break;
		default:
			break;
		}
	}

	void SetEnginePowerSpeed(double new_speed)
	{
		engine_power_speed = new_speed;
	}

	//??? ?? "???????????". ???? ?? ???? - ??????
	void SendMouseMoveEvent(int x, int y)
	{
		mouse_x = x;
		mouse_y = y;
	};

	void SendKeyPressEvent(FRKey k)
	{
		MoveManual(k);
	};

	uint64_t GetPrivateKey()
	{
		return private_key;
	}

	uint64_t GetPublicKey()
	{
		return public_key;
	}

	uint64_t GetSpriteID()
	{
		return sprite_id;
	}

	Rotation GetRotation()
	{
		return rotation;
	}

	std::string Serialize() override
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << GetType() << " " << GetCenterGlobal().first << " " << GetCenterGlobal().second << " " << GetRotation() << " " << GetSpriteID() << " " << (std::abs(x_speed) > engine_power_speed / 1.35 || std::abs(y_speed) > engine_power_speed / 1.35) << " " << GetPublicKey() << " ";

		return ss.str();
	}

	void SetRotation(Rotation rot)
	{
		rotation = rot;
	}

	std::string GetType() override
	{
		return "Ship";
	}

	std::pair<double, double> GetMouseCoords()
	{
		return std::make_pair(mouse_x, mouse_y);
	}

	void SetReloadTime(int time)
	{
		reload_time = time;
	}

	int GetReloadTime()
	{
		return reload_time;
	}

protected:
	HeadSprite *power; //?
	Rotation rotation;
	double impulse = 1.01;
	double control_impulse = 0.01;
	double engine_power_speed = 4;

	int reload_time = 0;

	uint64_t private_key = 0;
	uint64_t public_key = 0;

	uint64_t sprite_id = 0;

	int mouse_x = 0;
	int mouse_y = 0;
};