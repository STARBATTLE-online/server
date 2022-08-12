#pragma once
#include "MovableSprite.h"
#include "World.h"
#include "Rotation.h"
#include <algorithm>
/**
 * @brief Bullet class that stores all the neccessary logic about bullet, its position, sender, etc.
 */
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

	int lifespan = 100;
	uint64_t sender_id = 0;
};

/**
 * @brief Ship class that stores all the neccessary logic about ship, its state.
 */
class Ship : public MovableSprite {
public:
	Ship() = default;
	Ship(double sprite_width, double sprite_height) {
		width = sprite_width;
		height = sprite_height;

		private_key = rand();
		public_key = rand();

		sprite_id = rand() % 6 + 1;

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
			return (height + width) / 4.0 - 20;
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

		if (boost_cooldown)
			--boost_cooldown;

		ticks_since_last_seen++;
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
		if(boost_cooldown > 320) return;
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
		return 25;
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

	void setHealth(int new_health) {
		hp = new_health;
	}

	void setShieldDuration(int duration) {
		protection = duration;
	}

	void setHiddenShieldDuration(int duration) {
		hidden_protection = duration;
	}

	int getShieldDuration() const {
		return protection;
	}

	int getTicksSinceLastSeen() const {
		return ticks_since_last_seen;
	}

	void resetTicksSinceLastSeen() {
		ticks_since_last_seen = 0;
	}

	void heal(uint64_t amount) {
		hp = std::min(hp + amount, (uint64_t)10);
	}
	
	void boost() {
		auto spd = getSpeed();
		if(boost_cooldown) {
			return;
		}
		boost_cooldown = 400;

		switch(getRotation()) {
		case Rotation::Top:
			setSpeed(spd.first, spd.second - 25.);
			break;
		case Rotation::Left:
			setSpeed(spd.first - 25., spd.second);
			break;
		case Rotation::Bottom:
			setSpeed(spd.first, spd.second + 25.);
			break;
		case Rotation::Right:
			setSpeed(spd.first + 25., spd.second);
			break;
		default:
			break;
		}
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

	int ticks_since_last_seen = 0;
	int boost_cooldown = 0;
};

class AIShip : public Ship {
public:
	AIShip() = default;
	AIShip(double sprite_width, double sprite_height) {
		width = sprite_width;
		height = sprite_height;

		private_key = rand();
		public_key = rand();

		sprite_id = rand() % 6 + 1;

		mass = 10;
	};


	std::string getType() override {
		return "AIShip";
	}

	~AIShip() override = default;
};