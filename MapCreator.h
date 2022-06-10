#pragma once
#include "Asteroid.h"
#include "Ship.h"
#include "Rotation.h"

class MapCreator //заведует всеми объектами на карте
{
public:
	MapCreator(){
		ObserverSubscribes();
		
		for (int i = 0; i < NUM_ASTEROIDS(); i++)
		{
			if (i < NUM_ASTEROIDS() / 2)
			{
				asteroids.push_back(new BigAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, rand() % 10 - 5, rand() % 10 - 5));
			}
			else
			{
				asteroids.push_back(new SmallAsteroid(rand() % MAP_WIDTH, rand() % MAP_HEIGHT, rand() % 10 - 5, rand() % 10 - 5));
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


	//Тоже для "Наблюдателя"
	void ObserverSubscribes() {
		//mouse_click_event->Subscribe();
		//key_press_event->Subscribe(&main_hero);
		//mouse_move_event->Subscribe(&main_hero);
		//mouse_move_event->Subscribe(&(inter.GetReticle()));
	}

	void AddShip(int x, int y, Rotation rot) {
		//Новый игрок
	}

	void DeleteShip(int x, int y, Rotation rot) {
		//Новый игрок задолбался и свалил
	}

	void MapResize() {
		//когда добавляются новые игроки она увеличивается. Когда отключаются - уменьшается.
	}
	
	void AddAsteroid(int x, int y, Rotation rot) {
		//Когда ката увеличивается - добавляем астероиды. Надо добавить проверку на то, чтоб астероид не появился прям над пораблём или другим астероидом.
		//AllAsteroidsCheckCollision и AllShipsCheckCollision должны подойти, но ты проверь, они вообще норм работают?
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
	
protected:
	std::vector<Asteroid*> asteroids;
	std::vector<Ship*> ships;	
};
