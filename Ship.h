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

	std::string getType() override {
		return "Bullet";
	}

	std::string serialize() override {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << "Bullet " << getCenterGlobal().first << " " << getCenterGlobal().second << " " << x_speed << " " << y_speed;

		return ss.str();
	}

	~Bullet() = default;

	virtual int getDamage() {
		return 2;
	}

	int lifespan = 100;
	uint64_t sender_id = 0;
};

class AIBullet : public Bullet {
public:
	AIBullet(int x, int y, double x_speed, double y_speed) : Bullet(x, y, x_speed, y_speed) {}
	int getDamage() override {
		return 4;
	}

	~AIBullet() = default;
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

		mass = 30;
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

	virtual std::vector<Bullet*> shoot(double target_x, double target_y) {
		if(getReloadTime() != 0) return {};
		// Calculate target angle
		double target_angle = atan2(target_y - getCenterGlobal().second, target_x - getCenterGlobal().first);
		// Calculate speed along axises
		double x_speed = cos(target_angle) * BULLET_SPEED;
		double y_speed = sin(target_angle) * BULLET_SPEED;
		// Create bullet
		auto bullet = getBullet(getCenterGlobal().first, getCenterGlobal().second, x_speed, y_speed);
		// Set bullet speed
		bullet->setSpeed(x_speed, y_speed);
		bullet->sender_id = getPublicKey();
		setReloadTime(getReloadCooldown());

		return { bullet };
	}

	void sendMouseMoveEvent(int x, int y) {
		mouse_x = x;
		mouse_y = y;
	};

	virtual Bullet* getBullet(double a, double b, double c, double d) {
		return new Bullet(a, b, c, d);
	}

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
		ss << "Ship" << " " << 0 << " "  << getCenterGlobal().first << " " << getCenterGlobal().second << " " << getRotation() << " " << x_speed << " " << y_speed  << " " << getSpriteID() << " " << protection << " " << std::max(hp * 10 / getMaxHP(), 1) << " " << (std::abs(x_speed) > engine_power_speed / 1.35 || std::abs(y_speed) > engine_power_speed / 1.35) << " " << getPublicKey() << " ";

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

	void setRotationFromSpeed() {
		if( std::abs(x_speed) > std::abs(y_speed) ) {
			if(x_speed > 0) {
				rotation = Rotation::Right;
			} else {
				rotation = Rotation::Left;
			}
		} else {
			if(y_speed > 0) {
				rotation = Rotation::Bottom;
			} else {
				rotation = Rotation::Top;
			}
		}
	}

	int getReloadTime()
	{
		return reload_time;
	}

	virtual int getReloadCooldown()
	{
		return 25;
	}

	double getSpeedCeiling() override {
		return 50.;
	}

	int getBarrageDuration() {
		return barrage_duration;
	}

	virtual void takeDamage(int damage) {
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

	virtual void heal(uint64_t amount) {
		hp = std::min(hp + amount, (uint64_t)getMaxHP());
	}
	
	virtual void ability() {
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

	virtual int getMaxHP() {
		return 10;
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
	int hp = getMaxHP(); 
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

		sprite_id = 7;

		mass = 30;
	};


	std::string getType() override {
		return "AIShip";
	}

	int getReloadCooldown() override {
		return 50;
	}

	Bullet* getBullet(double a, double b, double c, double d) override {
		return new AIBullet(a, b, c, d);
	}

	std::string serialize() override {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << "Ship" << " " << 0 << " " << getCenterGlobal().first << " " << getCenterGlobal().second << " " << getRotation() << " " << x_speed << " " << y_speed  << " " << getSpriteID() << " " << protection << " " << std::max(hp * 10 / getMaxHP(), 1) << " " << 1 << " " << getPublicKey() << " ";

		return ss.str();
	}


	~AIShip() override = default;
};

class MassShooter : public Ship {
public:
	MassShooter() = default;
	MassShooter(double sprite_width, double sprite_height) {
		width = sprite_width;
		height = sprite_height;

		private_key = rand();
		public_key = rand();

		sprite_id = 2;

		mass = 30;
	};


	std::string getType() override {
		return "MassShooter";
	}

	std::vector<Bullet*> shoot(double target_x, double target_y) override {
		if(getReloadTime() != 0) return {};
		// Calculate target angle
		double target_angle = atan2(target_y - getCenterGlobal().second, target_x - getCenterGlobal().first);
		// Create bullet
		auto bullet = getBullet(getCenterGlobal().first, getCenterGlobal().second, cos(target_angle) * BULLET_SPEED, sin(target_angle) * BULLET_SPEED);
		auto bullet_left = getBullet(getCenterGlobal().first, getCenterGlobal().second, cos(target_angle + 0.25) * BULLET_SPEED, sin(target_angle + 0.25) * BULLET_SPEED);
		auto bullet_right = getBullet(getCenterGlobal().first, getCenterGlobal().second, cos(target_angle - 0.25) * BULLET_SPEED, sin(target_angle - 0.25) * BULLET_SPEED);

		// Set bullet speed
		bullet->sender_id = getPublicKey();

		bullet_left->sender_id = getPublicKey();

		bullet_right->sender_id = getPublicKey();

		setReloadTime(getReloadCooldown());

		return { bullet_left, bullet, bullet_right };
	}


	Bullet* getBullet(double a, double b, double c, double d) override {
		return new Bullet(a, b, c, d);
	}

	~MassShooter() override = default;
};

class FastBoy : public Ship {
public:
	FastBoy() = default;
	FastBoy(double sprite_width, double sprite_height) {
		width = sprite_width;
		height = sprite_height;

		private_key = rand();
		public_key = rand();

		sprite_id = 3;

		engine_power_speed = 15;

		mass = 30;
	};


	std::string getType() override {
		return "FastBoy";
	}

	Bullet* getBullet(double a, double b, double c, double d) override {
		return new Bullet(a, b, c, d);
	}

	int getReloadCooldown() override {
		return 15;
	}

	~FastBoy() override = default;
};

class FatGuy : public Ship {
public:
	FatGuy() = default;
	FatGuy(double sprite_width, double sprite_height) {
		width = sprite_width;
		height = sprite_height;

		private_key = rand();
		public_key = rand();

		sprite_id = 5;

		mass = 100;

		hp = 20;
	};

	std::string getType() override {
		return "FatGuy";
	}

	Bullet* getBullet(double a, double b, double c, double d) override {
		return new Bullet(a, b, c, d);
	}

	int getMaxHP() override {
		return 20;
	}

	void ability() override {
		if(boost_cooldown) {
			return;
		}
		boost_cooldown = 400;

		setShieldDuration(200);
	}

	~FatGuy() override = default;
};


class HomingMissile : public Bullet {
private:
	MovableSprite* target = nullptr;
public:
	HomingMissile(int x, int y, double x_speed, double y_speed) : Bullet(x, y, x_speed, y_speed) {}
	~HomingMissile() override = default;

	int getMaxAcceleration() {
		return 10;
	}

	void setTarget(MovableSprite* spr) {
		target = spr;
	}

	MovableSprite* getTarget() {
		return target;
	}
};

class Deadeye : public Ship {
public:
	Deadeye() = default;
	Deadeye(double sprite_width, double sprite_height) {
		width = sprite_width;
		height = sprite_height;

		private_key = rand();
		public_key = rand();

		sprite_id = 4;

		mass = 30;

		hp = 10;
	};

	std::string getType() override {
		return "Deadeye";
	}

	Bullet* getBullet(double a, double b, double c, double d) override {
		return new HomingMissile(a, b, c, d);
	}

	~Deadeye() override = default;
};

class EnemySpawner : public Ship {
	int status = 0;
public:
	EnemySpawner() = default;
	EnemySpawner(double sprite_width, double sprite_height) {
		width = sprite_width;
		height = sprite_height;

		private_key = rand();
		public_key = rand();

		sprite_id = 5;

		mass = 10;

		hp = 100;
	};

	std::string getType() override {
		return "EnemySpawner";
	}

	Bullet* getBullet(double a, double b, double c, double d) override {
		return new Bullet(a, b, c, d);
	}

	int getMaxHP() override {
		return 100;
	}

	std::string serialize() override {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << "EnemySpawner" << " " << 0 << " "  << getCenterGlobal().first << " " << getCenterGlobal().second << " " << std::max(hp * 10 / getMaxHP(), 1) << " " << status << " ";

		return ss.str();
	}

	void setStatus(int a) {
		status = a;
	}

	int getStatus() {
		return status;
	}

	void heal(uint64_t amount) override {
		hp = std::min(hp + amount, (uint64_t)getMaxHP());
		
		updateState();
	}

	void takeDamage(int damage) override {
		if(protection > 0 || hidden_protection > 0) return;

		hp -= damage;

		updateState();
	}

	void updateState() {
		if(hp < getMaxHP() * 0.75) {
			setStatus(2);
		} else {
			setStatus(1);
		}
	}

	void setSpeed(double new_x_speed, double new_y_speed) override {
		x_speed = 0;
		y_speed = 0;
	}

	void setCoordsByCenter(double new_x, double new_y) override {
		global_x = 4000 - width / 2;
		global_y = 4000 - height / 2;
	}


	~EnemySpawner() override = default;
};