#pragma once
#include "MovableSprite.h"
#include "World.h"
#include "Rotation.h"

//????? ????? ??? ?????, ???? ?? ???????? ???????
class Bullet : public MovableSprite {
public:
	Bullet(int x, int y) {
		width = 20;
		height = 20;
		global_x = x;
		global_y = y;
		mass = 4;
	};

	Bullet(int x, int y, double x_speed, double y_speed) {
		width = 20;
		height = 20;
		global_x = x;
		global_y = y;
		this->x_speed = x_speed;
		this->y_speed = y_speed;
	};

	std::string serialize() override {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << "Bullet " << getCenterGlobal().first << " " << getCenterGlobal().second << " " << x_speed << " " << y_speed;

		return ss.str();
	}

	~Bullet() = default;

	int lifespan = 80;
	uint64_t sender_id = 0;
};

class Ship : public MovableSprite {
public:
	Ship() = default;
	Ship(double sprite_width, double sprite_height) {
		width = sprite_width;
		height = sprite_height;

		private_key = rand();
		public_key = rand();

		sprite_id = rand() % 4 + 1;

		mass = 10;
	};

	~Ship() override = default;

	void move() override {
		global_y += y_speed;
		global_x += x_speed;
		border();
		useImpulse();
	}

	double getRadius() override {
		if(protection > 0) {
			return 140;
		} else {
			return (height + width) / 4.0;
		}
	}

	void tick() {
		if (reload_time)
			--reload_time;

		if (protection)
			--protection;

		if (hidden_protection) 
			--hidden_protection;

		if (barrage_duration) 
			--barrage_duration;
	}

	void useImpulse() {
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

	void setMovementDirection(FRKey k) {
		switch (k) {
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

	void sendMouseMoveEvent(int x, int y) {
		mouse_x = x;
		mouse_y = y;
	};

	void setBarrageDuration(int duration) {
		barrage_duration = duration;
	}

	uint64_t getDestructionScore() override {
		return 100;
	}

	uint64_t getPrivateKey() {
		return private_key;
	}

	uint64_t getPublicKey() {
		return public_key;
	}

	uint64_t getSpriteID() {
		return sprite_id;
	}

	Rotation getRotation() {
		return rotation;
	}

	std::string serialize() override {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << getType() << " " << getCenterGlobal().first << " " << getCenterGlobal().second << " " << getRotation() << " " << getSpriteID() << " " << protection << " " << hp << " " << (std::abs(x_speed) > engine_power_speed / 1.35 || std::abs(y_speed) > engine_power_speed / 1.35) << " " << getPublicKey() << " ";

		return ss.str();
	}

	void setRotation(Rotation rot) {
		rotation = rot;
	}

	std::string getType() override {
		return "Ship";
	}

	std::pair<double, double> getMouseCoords() {
		return std::make_pair(mouse_x, mouse_y);
	}

	void setReloadTime(int time) {
		reload_time = time;
	}

	int getReloadTime()
	{
		return reload_time;
	}

	double getSpeedCeiling() override {
		return 50.;
	}

	int getBarrageDuration() {
		return barrage_duration;
	}

	void takeDamage(int damage) {
		if(protection > 0 || hidden_protection > 0) return;

		hp -= damage;
	}

	int getHealth() {
		return hp;
	}

	void setShieldDuration(int duration) {
		protection = duration;
	}

	void setHiddenShieldDuration(int duration) {
		hidden_protection = duration;
	}

	int getShieldDuration() {
		return protection;
	}

protected:
	Rotation rotation;
	double impulse = 1.01;
	double control_impulse = 0.01;
	double engine_power_speed = 10;

	int reload_time = 0;

	uint64_t private_key = 0;
	uint64_t public_key = 0;

	uint64_t sprite_id = 0;
	int hp = 10; 
	int mouse_x = 0;
	int mouse_y = 0;

	int protection = 200;
	int hidden_protection = 0;
	int barrage_duration = 0;
};