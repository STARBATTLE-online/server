#pragma once
#include "Asteroid.h"
#include "Ship.h"
#include "Rotation.h"
#include "Explosion.h"
#include "Powerups.h"

#include <ranges>

class MapCreator //�������� ����� ��������� �� �����
{
public:
	MapCreator(){
		ObserverSubscribes();
		
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
		double target_angle = atan2(target_y - ship->GetCenterGlobal().second, target_x - ship->GetCenterGlobal().first);
		// Calculate speed along axises
		double x_speed = cos(target_angle) * BULLET_SPEED;
		double y_speed = sin(target_angle) * BULLET_SPEED;
		// Create bullet
		bullets.push_back(new Bullet(ship->GetCenterGlobal().first, ship->GetCenterGlobal().second, x_speed, y_speed));
		// Set bullet speed
		bullets.back()->SetSpeed(x_speed, y_speed);
		bullets.back()->sender_id = ship->GetPublicKey();
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

	//���� ��� "�����������"
	void ObserverSubscribes() {
		//mouse_click_event->Subscribe();
		//key_press_event->Subscribe(&main_hero);
		//mouse_move_event->Subscribe(&main_hero);
		//mouse_move_event->Subscribe(&(inter.GetReticle()));
	}

	Ship* AddShip(int x, int y, Rotation rot) {
		ships.push_back(new Ship(192, 192));
		ships.back()->SetRotation(rot);
		ships.back()->SetCoords(x, y);

		return ships.back();
	}

	void DeleteShip(int x, int y, Rotation rot) {
		//����� ����� ���������� � ������
	}

	void MapResize() {
		//����� ����������� ����� ������ ��� �������������. ����� ����������� - �����������.
	}
	
	void AddAsteroid(int x, int y, Rotation rot) {
		//����� ���� ������������� - ��������� ���������. ���� �������� �������� �� ��, ���� �������� �� �������� ���� ��� ������� ��� ������ ����������.
		//AllAsteroidsCheckCollision � AllShipsCheckCollision ������ �������, �� �� �������, ��� ������ ���� ��������?
		asteroids.push_back(new BigAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, rand() % 10 - 5, rand() % 10 - 5));
	}

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

		++tick_count;
	}

	void CheckCollisionsAll() {
for (auto it = asteroids.rbegin(); it != asteroids.rend(); ++it)
		{
			auto r = BulletCollisions(*it);
	
			if(r) {
				auto ptr = *(--(it.base()));

				explosions.push_back(new BigExplosion(r->GetCenterGlobal().first, r->GetCenterGlobal().second, tick_count));
				if(ptr->GetType() == "BigAsteroid") {
					asteroids.push_back(new SmallAsteroid(ptr->GetCenterGlobal().first - 5 + (rand() % 5), ptr->GetCenterGlobal().second - 5 + (rand() % 5), 
						(double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));

					asteroids.push_back(new SmallAsteroid(ptr->GetCenterGlobal().first - 5 + (rand() % 5), ptr->GetCenterGlobal().second - 5 + (rand() % 5), 
						(double)(rand() % 10 - 5) / 5, (double)(rand() % 10 - 5) / 5));
				}

				if(ptr->GetType() == "SmallAsteroid" && (rand() % 100 > 30)) {
					powerups.push_back(new Shield(ptr->GetCenterGlobal().first, ptr->GetCenterGlobal().second, tick_count));
				}

				asteroids.erase(--(it.base()));
				bullets.erase(std::remove(bullets.begin(), bullets.end(), r), bullets.end());
			}

			AsteroidCollisions(*it);
		}
		for (auto it = ships.rbegin(); it != ships.rend(); ++it)
		{
			auto r = BulletCollisions(*it, (*it)->GetPublicKey());

			if(r) {
				auto ptr = *(--(it.base()));

				explosions.push_back(new BigExplosion(r->GetCenterGlobal().first, r->GetCenterGlobal().second, tick_count));

				ships.erase(--(it.base()));
				bullets.erase(std::remove(bullets.begin(), bullets.end(), r), bullets.end());
			}
			
			AsteroidCollisions(*it);
			ShipCollisions(*it);
		}
	}

	std::string Serialize() {
		std::stringstream ss;
		
		ss << tick_count << " ";

		for (auto astroid : asteroids)
		{
			ss << astroid->Serialize() << " ";
		}

		for (auto ship : ships)
		{
			ss << ship->Serialize() << " ";
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
	std::vector<Asteroid*> asteroids;
	std::vector<Ship*> ships;
	std::vector<Bullet*> bullets;
	std::vector<Explosion*> explosions;
	std::vector<Powerup*> powerups;

	uint64_t tick_count = 0;
};
