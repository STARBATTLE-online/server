#pragma once

#include "Asteroid.h"
#include "Ship.h"
#include "Rotation.h"
#include "Explosion.h"
#include "Powerups.h"

#include <ranges>

/**
 * @brief This class is used for storing and managing all the game state.
 */
class World
{
public:
	World(){	
		asteroids.reserve(64);
		ships.reserve(64);
		bullets.reserve(64);

		for (int i = 0; i < NUM_ASTEROIDS_BIG + NUM_ASTEROIDS_SMALL; i++)
		{
			if (i < NUM_ASTEROIDS_BIG)
			{
				asteroids.push_back(new BigAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, (double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));
			}
			else
			{
				asteroids.push_back(new SmallAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, (double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));
			}
		}

		for(auto astroid : asteroids)
		{
			while (asteroidCollisions(astroid))
			{
				astroid->setCoords(rand() % MAP_WIDTH, rand() % MAP_HEIGHT);
			}
		}


	};
	~World() {
		for(auto el : ships) delete el;
		for(auto el : asteroids) delete el;
		for(auto el : powerups) delete el;
		for(auto el : explosions) delete el;
		for(auto el : bullets) delete el;
	};

	World(const World& other) {}

	void shoot(Ship* ship) {
		shoot(ship->getMouseCoords().first, ship->getMouseCoords().second, ship);
	}

	void rightClick(Ship* ship) {
		rightClick(ship->getMouseCoords().first, ship->getMouseCoords().second, ship);
	}

	void rightClick(double target_x, double target_y, Ship* ship) {
		ship->boost();
	}

	// TODO: Verify that this works
	void shoot(double target_x, double target_y, Ship* ship) {
		if(ship->getReloadTime() != 0) return;
		// Calculate target angle
		double target_angle = atan2(target_y - ship->getCenterGlobal().second, target_x - ship->getCenterGlobal().first);
		// Calculate speed along axises
		double x_speed = cos(target_angle) * BULLET_SPEED;
		double y_speed = sin(target_angle) * BULLET_SPEED;
		// Create bullet
		bullets.push_back(new Bullet(ship->getCenterGlobal().first, ship->getCenterGlobal().second, x_speed, y_speed));
		// Set bullet speed
		bullets.back()->setSpeed(x_speed, y_speed);
		bullets.back()->sender_id = ship->getPublicKey();
		ship->setReloadTime(RELOAD_TIME);
	}

	Asteroid* asteroidCollisions(MovableSprite* object) {
		for (auto& asteroid : asteroids)
		{
			if(asteroid->collisionDetector(object)) {
				return asteroid;
			}
		}
		return nullptr;
	}

	Bullet* bulletCollisions(MovableSprite* object, uint64_t exemptKey = 0) {
		for (auto& bullet : bullets)
		{
			if(bullet->sender_id == exemptKey) continue;
			if(bullet->collisionDetector(object)) {
				return bullet;
			}
		}
		return nullptr;
	}

	Ship* shipCollisions(MovableSprite* object) {
		for (auto& ship : ships)
		{
			if(ship->collisionDetector(object)) {
				return ship;
			}
		}
		return nullptr;
	}

	Ship* getCloseToPowerup(Powerup* powerup) {
		for (auto& ship : ships)
		{
			if(ship->distance(powerup) < 0) {
				return ship;
			}
		}
		return nullptr;
	}

	/*
	 * Adds a ship with the specified position and rotation to the game field.
	 */
	Ship* AddShip(int x, int y, Rotation rot) {
		ships.push_back(new Ship(192, 192));
		ships.back()->setRotation(rot);
		ships.back()->setCoords(x, y);

		public_id_to_ship[ships.back()->getPublicKey()] = ships.back();
		ship_to_public_id[ships.back()] = ships.back()->getPublicKey();
		return ships.back();
	}

	Ship* generateEnemy(int x, int y) {
		ships.push_back(new Ship(192, 192));
		ships.back()->setRotation(Rotation::Left);
		ships.back()->setCoords(x, y);

		public_id_to_ship[ships.back()->getPublicKey()] = ships.back();
		ship_to_public_id[ships.back()] = ships.back()->getPublicKey();
		return ships.back();
	}

	Ship* getShipByPublicID(uint64_t id) {
		return public_id_to_ship[id];
	}

	uint64_t getPublicIDByShip(Ship* ship) {
		return ship_to_public_id[ship];
	}
	
    /*
	 * Adds an asteroid with the specified position and rotation to the game field.
	 */
	void AddAsteroid(int x, int y, Rotation rot) {
		asteroids.push_back(new BigAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, rand() % 10 - 5, rand() % 10 - 5));
	}

	/*
	 * Advances the game state by 1 frame.
	 */
	void tick() {
		std::vector<MovableSprite*> erased;

		for (auto asteroid : asteroids)
		{
			asteroid->move();
		}
		for (auto ship : ships)
		{
			ship->move();
			ship->tick();

			if(ship->getBarrageDuration() > 0) {
					constexpr const int mod = 10;
					auto r = (200 - ship->getBarrageDuration()) % (8 * mod);
					
					auto position = ship->getCenterGlobal();
					const int barrage_speed_multiplier = 5;
					switch(r) {
						case 1:
							bullets.push_back(new Bullet(position.first, position.second, 0, -5 * barrage_speed_multiplier));
							bullets.back()->sender_id = ship->getPublicKey();
							break;
						case 1 + mod:
							bullets.push_back(new Bullet(position.first, position.second, 2.2 * barrage_speed_multiplier, -2.2 * barrage_speed_multiplier));
							bullets.back()->sender_id = ship->getPublicKey();
							break;
						case 1 + mod * 2:
							bullets.push_back(new Bullet(position.first, position.second, 5 * barrage_speed_multiplier, 0));
							bullets.back()->sender_id = ship->getPublicKey();
							break;
						case 1 + mod * 3:
							bullets.push_back(new Bullet(position.first, position.second, 2.2 * barrage_speed_multiplier, 2.2 * barrage_speed_multiplier));
							bullets.back()->sender_id = ship->getPublicKey();
							break;
						case 1 + mod * 4:
							bullets.push_back(new Bullet(position.first, position.second, 0, 5 * barrage_speed_multiplier));
							bullets.back()->sender_id = ship->getPublicKey();
							break;
						case 1 + mod * 5:
							bullets.push_back(new Bullet(position.first, position.second, -2.2 * barrage_speed_multiplier, 2.2 * barrage_speed_multiplier));
							bullets.back()->sender_id = ship->getPublicKey();
							break;
						case 1 + mod * 6:
							bullets.push_back(new Bullet(position.first, position.second, -5 * barrage_speed_multiplier, 0));
							bullets.back()->sender_id = ship->getPublicKey();
							break;
						case 1 + mod * 7:
							bullets.push_back(new Bullet(position.first, position.second, -2.2 * barrage_speed_multiplier, -2.2 * barrage_speed_multiplier));
							bullets.back()->sender_id = ship->getPublicKey();
							break;
						default:
							break;
					}
			}
		}
		for (auto bullet : bullets)
		{
			bullet->move();
			--bullet->lifespan;
		}

		std::erase_if(bullets, [&erased] (Bullet* bullet) {
			if(bullet->lifespan <= 0) {
				erased.push_back(bullet);
				return true;
			}

			return false;
		});

		// Remove explosions that have already finished
		std::erase_if(explosions, [this, &erased](Explosion* explosion) {
			if(explosion->getCreationTick() + 100 < tick_count) {
				erased.push_back(explosion);
				return true;
			}

			return false;
		});

		std::erase_if(powerups, [this, &erased](Powerup* powerup) {
			if(powerup->getCreationTick() + 250 < tick_count) {
				erased.push_back(powerup);
				return true;
			}

			return false;
		});

		for(const auto& el : erased) {
			if(dynamic_cast<Ship*>(el)) {
				auto r = dynamic_cast<Ship*>(el);
				auto id = ship_to_public_id[r]; 
				ship_to_public_id[r] = 0;
				public_id_to_ship[id] = nullptr;
			}
			
			delete el;
		}

		tryGenerateAsteroid();

		++tick_count;
	}

	void tryGenerateAsteroid() {	
		if(asteroids.size() >= NUM_ASTEROIDS_BIG + NUM_ASTEROIDS_SMALL) return;

		asteroids.push_back(new BigAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, (double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));
	}

	/*
	 * Performs collision checks between all the objects on the game field (that should collide) and
	 * handles their resolution.
	 */
	void checkAllCollisions() {
		std::vector<MovableSprite*> erased;

		for (auto it = asteroids.rbegin(); it != asteroids.rend(); ++it)
		{
			auto r = bulletCollisions(*it);
	
			if(r) {
				auto ptr = *(--(it.base()));
				scores[r->sender_id] += (*it)->getDestructionScore();

				explosions.push_back(new BigExplosion(r->getCenterGlobal().first, r->getCenterGlobal().second, tick_count));
				if(ptr->getType() == "BigAsteroid") {
					asteroids.push_back(new SmallAsteroid(ptr->getCenterGlobal().first - 5 + (rand() % 5), ptr->getCenterGlobal().second - 5 + (rand() % 5), 
						(double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));

					asteroids.push_back(new SmallAsteroid(ptr->getCenterGlobal().first - 5 + (rand() % 5), ptr->getCenterGlobal().second - 5 + (rand() % 5), 
						(double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));
				}

				if(ptr->getType() == "SmallAsteroid") {
					if(rand() % 100 > 90) {
						powerups.push_back(new Shield(ptr->getCenterGlobal().first, ptr->getCenterGlobal().second, tick_count));
					} else if(rand() % 100 > 88) {
						powerups.push_back(new Barrage(ptr->getCenterGlobal().first, ptr->getCenterGlobal().second, tick_count));
					} else if(rand() % 100 > 81) {
						powerups.push_back(new Heal(ptr->getCenterGlobal().first, ptr->getCenterGlobal().second, tick_count));
					}
				}
				erased.push_back(ptr);
				erased.push_back(r);

				asteroids.erase(--(it.base()));
				bullets.erase(std::remove(bullets.begin(), bullets.end(), r), bullets.end());
				break;
			}

			asteroidCollisions(*it);
		}
		for (auto it = ships.rbegin(); it != ships.rend(); ++it)
		{
			auto r1 = bulletCollisions(*it, (*it)->getPublicKey());

			if(r1) {
				auto ptr = *(--(it.base()));
				scores[r1->sender_id] += (*it)->getDestructionScore();
				explosions.push_back(new BigExplosion(r1->getCenterGlobal().first, r1->getCenterGlobal().second, tick_count));

				(*it)->takeDamage(2);
				if((*it)->getHealth() <= 0) {
					erased.push_back(ptr);
					ships.erase(--(it.base()));
				}

				erased.push_back(r1);
				bullets.erase(std::remove(bullets.begin(), bullets.end(), r1), bullets.end());
				break;
			}
			
			auto r2 = asteroidCollisions(*it);

			if(r2) {
				auto ptr = *(--(it.base()));

				(*it)->takeDamage(((r2->getType() == "BigAsteroid") ? 3 : 2));
				(*it)->setHiddenShieldDuration(3);
				if((*it)->getHealth() <= 0) {
					explosions.push_back(new BigExplosion(r2->getCenterGlobal().first, r2->getCenterGlobal().second, tick_count));
					erased.push_back(ptr);
					ships.erase(--(it.base()));
				}

				break;
			}

			auto r3 = shipCollisions(*it);

			if(r3) {
				auto ptr = *(--(it.base()));

				(*it)->takeDamage(3);
				r3->takeDamage(3);
				(*it)->setHiddenShieldDuration(5);
				r3->setHiddenShieldDuration(5);

				if(r3->getHealth() <= 0) {
					explosions.push_back(new BigExplosion(r3->getCenterGlobal().first, r3->getCenterGlobal().second, tick_count));
					erased.push_back(r3);
					ships.erase(std::remove(ships.begin(), ships.end(), r3), ships.end());
				} else if((*it)->getHealth() <= 0) {
					explosions.push_back(new BigExplosion((*it)->getCenterGlobal().first, (*it)->getCenterGlobal().second, tick_count));
					erased.push_back(ptr);
					ships.erase(--(it.base()));
				}

				break;
			}
		}

		for(auto it = powerups.rbegin(); it != powerups.rend(); ++it)
		{
			auto r = getCloseToPowerup(*it);

			if(r) {
				auto ptr = *(--(it.base()));

				ptr->activate(r);
				erased.push_back(ptr);
				powerups.erase(--(it.base()));
			}
		}

		for(auto it = ships.rbegin(); it != ships.rend(); ++it) {
			auto ptr = *(--(it.base()));
			if(ptr->getTicksSinceLastSeen() >= 500) {
				explosions.push_back(new BigExplosion(ptr->getCenterGlobal().first, ptr->getCenterGlobal().second, tick_count));
				erased.push_back(ptr);

				ships.erase(--(it.base()));
			}
		}

		for(auto& el : erased) delete el;
	}

	/*
	 * Serializes the game state into a single string that can
	 * be accepted by the game client.
	 */
	std::string serialize() {
		std::stringstream ss;
		
		ss << tick_count << " ";

		for (auto astroid : asteroids)
		{
			ss << astroid->serialize() << " ";
		}

		for (auto ship : ships)
		{
			ss << ship->serialize() << " " << scores[ship->getPublicKey()] << " ";
		}

		for (auto bullet : bullets)
		{
			ss << bullet->serialize() << " ";
		}

		for (auto explosion : explosions)
		{
			ss << explosion->serialize() << " ";
		}

		for (auto powerup : powerups)
		{
			ss << powerup->serialize() << " ";
		}

		return ss.str();
	}
	
	std::mutex mt;

	/*
	 * Getters
	 */

	auto& getShips() {
		return ships;
	}

	auto& GetAsteroids() {
		return asteroids;
	}

	auto& GetBullets() {
		return bullets;
	}

protected:
	/* We use vectors of pointers instead of values because we want to use inheritance */
	std::vector<Asteroid*> asteroids;
	std::vector<Ship*> ships;
	std::vector<Bullet*> bullets;
	std::vector<Explosion*> explosions;
	std::vector<Powerup*> powerups;

	std::unordered_map<uint64_t, int> scores;
	std::map<uint64_t, Ship*> public_id_to_ship;
	std::map<Ship*, uint64_t> ship_to_public_id;
	uint64_t tick_count = 0;
};
