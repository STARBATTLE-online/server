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
				asteroids.push_back(new BigAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, (double)(rand() % 10 - 5) / 10, (double)(rand() % 10 - 5) / 10));
			}
			else
			{
				asteroids.push_back(new SmallAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, (double)(rand() % 10 - 5) / 10, (double)(rand() % 10 - 5) / 10));
			}
		}

		for(auto astroid : asteroids)
		{
			while (AllAsteroidsCheckCollision(astroid))
			{
				astroid->SetCoords(rand() % MAP_WIDTH, rand() % MAP_HEIGHT);
			}
		}


	};
	~MapCreator(){};

	bool AllAsteroidsCheckCollision(MovableSprite* object) {
		
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

	bool AllShipsCheckCollision(HeadSprite* object) {
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

	void MoveAll() {
		for (auto astroid : asteroids)
		{
			astroid->Move();
		}
		for (auto ship : ships)
		{
			ship->Move();
		}
	}

	void CheckCollisionsAll() {
		for (auto astroid : asteroids)
		{
			AllAsteroidsCheckCollision(astroid);
		}
		for (auto ship : ships)
		{
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

		return ss.str();
	}
	
	std::mutex mt;


	auto GetShips() {
		return ships;
	}

	auto GetAsteroids() {
		return asteroids;
	}
protected:
	std::vector<Asteroid*> asteroids;
	std::vector<Ship*> ships;
};
