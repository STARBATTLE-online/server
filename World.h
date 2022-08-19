#pragma once

#include "Asteroid.h"
#include "Ship.h"
#include "Rotation.h"
#include "Explosion.h"
#include "Powerups.h"
#include "Animation.h"
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

		ships.push_back(new EnemySpawner(876, 876));
		ships.back()->setCoords(0, 0);

		public_id_to_ship[ships.back()->getPublicKey()] = ships.back();
		ship_to_public_id[ships.back()] = ships.back()->getPublicKey();
	};

	~World() {
		for(auto el : ships) delete el;
		for(auto el : asteroids) delete el;
		for(auto el : powerups) delete el;
		for(auto el : explosions) delete el;
		for(auto el : bullets) delete el;
		for(auto el : animations) delete el;
	};

	World(const World& other) {}

	void shoot(Ship* ship) {
		shoot(ship->getMouseCoords().first, ship->getMouseCoords().second, ship);
	}

	void rightClick(Ship* ship) {
		rightClick(ship->getMouseCoords().first, ship->getMouseCoords().second, ship);
	}

	void rightClick(double target_x, double target_y, Ship* ship) {
		ship->ability();
	}

	// TODO: Verify that this works
	void shoot(double target_x, double target_y, Ship* ship) {
		auto r = ship->shoot(target_x, target_y);

		bullets.insert(bullets.end(), r.begin(), r.end());
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
	Ship* addShip(int x, int y, Rotation rot) {
		auto r = rand() % 6;
		switch(r) {
			case 0:
			case 1:
			case 2:
				ships.push_back(new MassShooter(192, 192));
				break;
			case 3:
				ships.push_back(new Deadeye(192, 192));
				break;
			case 4:
				ships.push_back(new FatGuy(192, 192));
				break;
			case 5:
				ships.push_back(new FastBoy(192, 192));
				break;
		}

		ships.back()->setRotation(rot);
		ships.back()->setCoords(x, y);

		public_id_to_ship[ships.back()->getPublicKey()] = ships.back();
		ship_to_public_id[ships.back()] = ships.back()->getPublicKey();
		return ships.back();
	}

	Ship* generateEnemy(int x, int y) {
		ships.push_back(new AIShip(192, 192));
		ships.back()->setRotation(Rotation::Left);
		ships.back()->setCoordsByCenter(x, y);

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
			if(!dynamic_cast<HomingMissile*>(bullet)) --bullet->lifespan;
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
			if(dynamic_cast<AIShip*>((*it)) || dynamic_cast<EnemySpawner*>((*it))) 
				(*it)->resetTicksSinceLastSeen();

			auto r = dynamic_cast<EnemySpawner*>(*it);
			if(r) {
				r->setCoordsByCenter(MAP_WIDTH / 2, MAP_HEIGHT / 2); 

				if(tick_count % 1024 > 512) {
					
					r->setStatus(2);
				} else {
					r->setStatus(1);
				}
			}

			auto r1 = bulletCollisions(*it, (*it)->getPublicKey());

			if(r1) {
				auto ptr = *(--(it.base()));
				scores[r1->sender_id] += (*it)->getDestructionScore();
				if((*it)->getType() == "EnemySpawner") {
					explosions.push_back(new BigExplosion(r1->getCenterGlobal().first, r1->getCenterGlobal().second, tick_count));
				} else {
					explosions.push_back(new BigExplosion(r1->getCenterGlobal().first, r1->getCenterGlobal().second, tick_count));
				}

				(*it)->takeDamage(r1->getDamage());
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

		for(auto it = animations.rbegin(); it != animations.rend(); ++it) {
			auto el = *it;
			if(el->hasEnded()) {
				if(el->getType() == "LeftShipAnimation") {
					auto [x, y] = el->getCenterGlobal();
					generateEnemy(x, y);
					erased.push_back(el);
					animations.erase(--(it.base()));
				}
			}
		}

		for(auto& el : erased) {
			if(!dynamic_cast<Bullet*>(el)) {
				for(auto& bullet : bullets) {
					auto bul = dynamic_cast<HomingMissile*>(bullet);
					if(!bul) continue;

					if(bul->getTarget() == el) {
						bul->setTarget(nullptr);
					}
				}
			}
			delete el;
		}

		if(tick_count % 900 == 0 && ships.size() <= 5) {
			animations.push_back(new LeftShipAnimation());
		}

		for(auto& el : animations) {
			el->tick();
		}

		for(auto& el : ships) {
			if(el->getType() != "AIShip") continue;
			for(auto& el2 : ships) {
				if(dynamic_cast<AIShip*>(el2) || dynamic_cast<EnemySpawner*>(el2)) continue;
				auto [mouse_x, mouse_y] = el2->getCenterGlobal();
                el->sendMouseMoveEvent(mouse_x, mouse_y);
                el->setRotation(RotationFromString("B"));
				//shoot(el);
				// Get vector between two ships
				auto [x, y] = el->getCenterGlobal();
				double diff_x = mouse_x - x, diff_y = mouse_y - y;
				// Normalize new speed.
				double len = std::sqrt(diff_x * diff_x + diff_y * diff_y);
				len = std::max(0.1, len);
				diff_x /= len;
				diff_y /= len;

				el->setRotationFromSpeed();

				el->setSpeed(diff_x * 5, diff_y * 5);
			}
		}

		for(auto& bullet : bullets) {
			auto el = dynamic_cast<HomingMissile*>(bullet);
			if(el) {
				//Find target
				MovableSprite* bestTarget = nullptr;
				
				if(!el->getTarget()) {
					double dist = 100000;
					for(const auto& ship : ships) {
						if(ship->getPublicKey() == el->sender_id) continue;
						double tmp = el->realDistance(ship);
						if(tmp < dist) {
							bestTarget = ship;
							dist = tmp;
						}
					}

					for(const auto& asteroid : asteroids) {
						double tmp = el->realDistance(asteroid) * 1.5;
						if(tmp < dist) {
							bestTarget = asteroid;
							dist = tmp;
						}
					}

					el->setTarget(bestTarget);
				}

				// No ships
				if(!el->getTarget()) break;

				//Calculate and accelerate bullet accordingly
				auto [x, y] = el->getCenterGlobal();
				auto [vx, vy] = el->getSpeed();
				auto [target_x, target_y] = el->getTarget()->getCenterGlobal();

				double acc_x = target_x - x, acc_y = target_y - y;
				double len = std::sqrt(acc_x * acc_x + acc_y * acc_y) + 0.001;

				acc_x /= len / 2;
				acc_y /= len / 2;

				el->setSpeed(vx + acc_x, vy + acc_y);

				el->truncateSpeed(15);
			}
		}
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
			ss << ship->serialize() << " ";
			
			if(ship->getType() != "EnemySpawner") {
				ss << scores[ship->getPublicKey()] << " ";
			}
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

		for (auto animation : animations)
		{
			ss << animation->serialize() << " ";
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
	std::vector<Animation*> animations;

	std::unordered_map<uint64_t, int> scores;
	std::map<uint64_t, Ship*> public_id_to_ship;
	std::map<Ship*, uint64_t> ship_to_public_id;
	uint64_t tick_count = 0;
};
