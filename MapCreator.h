#pragma once
#include "Asteroid.h"
#include "Ship.h"
#include "Rotation.h"
#include "Explosion.h"
#include "Powerups.h"

#include <ranges>

/*
 * This class is used for storing and managing all the game state.
 */
class MapCreator
{
public:
	MapCreator(){		
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
			while (AsteroidCollisions(astroid))
			{
				astroid->SetCoords(rand() % MAP_WIDTH, rand() % MAP_HEIGHT);
			}
		}


	};
	~MapCreator(){};

	void Shoot(Ship* ship) {
		Shoot(ship->GetMouseCoords().first, ship->GetMouseCoords().second, ship);
	}

	// TODO: Verify that this works
	void Shoot(double target_x, double target_y, Ship* ship) {
		if(ship->GetReloadTime() != 0) return;
		// Calculate target angle
		double target_angle = atan2(target_y - ship->getCenterGlobal().second, target_x - ship->getCenterGlobal().first);
		// Calculate speed along axises
		double x_speed = cos(target_angle) * BULLET_SPEED;
		double y_speed = sin(target_angle) * BULLET_SPEED;
		// Create bullet
		bullets.push_back(new Bullet(ship->getCenterGlobal().first, ship->getCenterGlobal().second, x_speed, y_speed));
		// Set bullet speed
		bullets.back()->SetSpeed(x_speed, y_speed);
		bullets.back()->sender_id = ship->getPublicKey();
		ship->SetReloadTime(RELOAD_TIME);
	}

	Asteroid* AsteroidCollisions(MovableSprite* object) {
		for (auto& asteroid : asteroids)
		{
			if(asteroid->CheckCollision(object)) {
				return asteroid;
			}
		}
		return nullptr;
	}

	Bullet* BulletCollisions(MovableSprite* object, uint64_t exemptKey = 0) {
		for (auto& bullet : bullets)
		{
			if(bullet->sender_id == exemptKey) continue;
			if(bullet->CheckCollision(object)) {
				return bullet;
			}
		}
		return nullptr;
	}

	Ship* ShipCollisions(MovableSprite* object) {
		for (auto& ship : ships)
		{
			if(ship->CheckCollision(object)) {
				return ship;
			}
		}
		return nullptr;
	}

	Ship* isNearPowerup(Powerup* powerup) {
		for (auto& ship : ships)
		{
			if(ship->Distance(powerup) < 0) {
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
		ships.back()->SetRotation(rot);
		ships.back()->SetCoords(x, y);

		return ships.back();
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
	void Tick() {
		for (auto astroid : asteroids)
		{
			astroid->Move();
		}
		for (auto ship : ships)
		{
			ship->Move();
			ship->Tick();
		}
		for (auto bullet : bullets)
		{
			bullet->Move();
			--bullet->lifespan;
		}

		// Remove bullets that are out of map
		std::erase_if(bullets, [](Bullet* bullet) {
			return bullet->lifespan <= 0;
		});

		// Remove explosions that have already finished
		std::erase_if(explosions, [this](Explosion* explosion) {
			return explosion->GetCreationTick() + 100 < tick_count;
		});

		std::erase_if(powerups, [this](Powerup* powerup) {
			return powerup->GetCreationTick() + 250 < tick_count;
		});

		TryGeneratelAsteroid();

		++tick_count;
	}

	void TryGeneratelAsteroid() {	
		if(asteroids.size() >= NUM_ASTEROIDS_BIG + NUM_ASTEROIDS_SMALL) return;

		asteroids.push_back(new BigAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, (double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));
	}

	/*
	 * Performs collision checks between all the objects on the game field (that should collide) and
	 * handles their resolution.
	 */
	void CheckCollisionsAll() {
		for (auto it = asteroids.rbegin(); it != asteroids.rend(); ++it)
		{
			auto r = BulletCollisions(*it);
	
			if(r) {
				auto ptr = *(--(it.base()));
				scores[r->sender_id] += (*it)->GetDestructionScore();

				explosions.push_back(new BigExplosion(r->getCenterGlobal().first, r->getCenterGlobal().second, tick_count));
				if(ptr->GetType() == "BigAsteroid") {
					asteroids.push_back(new SmallAsteroid(ptr->getCenterGlobal().first - 5 + (rand() % 5), ptr->getCenterGlobal().second - 5 + (rand() % 5), 
						(double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));

					asteroids.push_back(new SmallAsteroid(ptr->getCenterGlobal().first - 5 + (rand() % 5), ptr->getCenterGlobal().second - 5 + (rand() % 5), 
						(double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));
				}

				if(ptr->GetType() == "SmallAsteroid" && (rand() % 100 > 30)) {
					powerups.push_back(new Shield(ptr->getCenterGlobal().first, ptr->getCenterGlobal().second, tick_count));
				}

				asteroids.erase(--(it.base()));
				bullets.erase(std::remove(bullets.begin(), bullets.end(), r), bullets.end());
			}

			AsteroidCollisions(*it);
		}
		for (auto it = ships.rbegin(); it != ships.rend(); ++it)
		{
			auto r = BulletCollisions(*it, (*it)->getPublicKey());

			if(r) {
				auto ptr = *(--(it.base()));
				scores[r->sender_id] += (*it)->GetDestructionScore();
				explosions.push_back(new BigExplosion(r->getCenterGlobal().first, r->getCenterGlobal().second, tick_count));

				(*it)->TakeDamage(2);
				if((*it)->GetHealth() <= 0)
					ships.erase(--(it.base()));

				bullets.erase(std::remove(bullets.begin(), bullets.end(), r), bullets.end());
			}
			
			AsteroidCollisions(*it);
			ShipCollisions(*it);
		}

		for(auto it = powerups.rbegin(); it != powerups.rend(); ++it)
		{
			auto r = isNearPowerup(*it);

			if(r) {
				auto ptr = *(--(it.base()));

				ptr->Activate(r);

				powerups.erase(--(it.base()));
			}
		}
	}

	/*
	 * Serializes the game state into a single string that can
	 * be accepted by the game client.
	 */
	std::string Serialize() {
		std::stringstream ss;
		
		ss << tick_count << " ";

		for (auto astroid : asteroids)
		{
			ss << astroid->Serialize() << " ";
		}

		for (auto ship : ships)
		{
			ss << ship->Serialize() << " " << scores[ship->getPrivateKey()] << " ";
		}

		for (auto bullet : bullets)
		{
			ss << bullet->Serialize() << " ";
		}

		for (auto explosion : explosions)
		{
			ss << explosion->Serialize() << " ";
		}

		for (auto powerup : powerups)
		{
			ss << powerup->Serialize() << " ";
		}

		return ss.str();
	}
	
	std::mutex mt;

	/*
	 * Getters
	 */

	auto GetShips() {
		return ships;
	}

	auto GetAsteroids() {
		return asteroids;
	}

	auto GetBullets() {
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

	uint64_t tick_count = 0;
};
