#pragma once
static const int MAP_WIDTH = 3000;
static const int MAP_HEIGHT = 2000;
static const int NUM_ASTEROIDS_BIG = 50;
static const int NUM_ASTEROIDS_SMALL = 0;
static const int NUM_AMMO = 3;
static const double ABILITY_PROBABILITY = 0.5;

int NumberOfAsteroids() {
	return NUM_ASTEROIDS_BIG + NUM_ASTEROIDS_SMALL;
}

// TODO: FIX THIS SHIt/
enum class FRKey {
    RIGHT, LEFT, DOWN, UP
};

enum class FRMouseButton {
    RIGHT, MIDDLE, LEFT
};
