#pragma once
#include "Asteroid.h"
#include "Ship.h"
#include "Rotation.h"

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
			while (AsteroidShipCollisions(astroid))
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

	bool AsteroidShipCollisions(MovableSprite* object) {
		
		for (auto astroid_check : asteroids)
		{
			if (dynamic_cast<Ship*>(object))
			{
				if (object->Distance(astroid_check) < 50)
				{
					return true;
				}
			}
			else if (dynamic_cast<Asteroid*>(object))
			{
				if (astroid_check->CheckCollision(object))
				{
					return true;
				}
			}
		}
		return false;
	}

	bool BulletAsteroidCollisions(MovableSprite* object) {
		for(auto bullet : bullets)
		{
			if(bullet->CheckCollision(object))
			{
				return true;
			}
		}

		return false;
	}

	bool BulletShipCollisions(Ship* ship) {
		bool collisions = std::any_of(bullets.begin(), bullets.end(), [&](Bullet* bullet) {
			return ship->GetPublicKey() != bullet->sender_id && bullet->CheckCollision(ship);
		});

		return false;
	}

	bool AllShipsCheckCollision(MovableSprite* object) {
		if (dynamic_cast<Ship*>(object))
		{
			for (auto enemy : ships)
			{
				if (dynamic_cast<Ship*>(object))
				{
					if (enemy->Distance(object) < 300)
					{
						return true;
					}
				}
				else
				{
					if (enemy->CheckCollision(object))
					{
						return true;
					}
				}
			}
		}
		return false;
	}


	//���� ��� "�����������"
	void ObserverSubscribes() {
		//mouse_click_event->Subscribe();
		//key_press_event->Subscribe(&main_hero);
		//mouse_move_event->Subscribe(&main_hero);
		//mouse_move_event->Subscribe(&(inter.GetReticle()));
	}

	Ship* AddShip(int x, int y, Rotation rot) {
		ships.push_back(new Ship(96, 96));
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
	}

	void CheckCollisionsAll() {
		for (auto astroid : asteroids)
		{
			AsteroidShipCollisions(astroid);
			BulletAsteroidCollisions(astroid);
		}
		for (auto ship : ships)
		{
			BulletShipCollisions(ship);
			AllShipsCheckCollision(ship);
		}
	}

	std::string Serialize() {
		std::stringstream ss;
		
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
};
