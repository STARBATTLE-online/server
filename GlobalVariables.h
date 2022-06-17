#pragma once
static const int MAP_WIDTH = 2000;
static const int MAP_HEIGHT = 2000;
static const int NUM_ASTEROIDS_BIG = 5;
static const int NUM_ASTEROIDS_SMALL = 5;
static const int NUM_AMMO = 3;
static const double ABILITY_PROBABILITY = 0.5;

int NUM_ASTEROIDS() {
	return NUM_ASTEROIDS_BIG + NUM_ASTEROIDS_SMALL;
}

// TODO: FIX THIS SHIt/
enum class FRKey {
    RIGHT, LEFT, DOWN, UP
};

enum class FRMouseButton {
    RIGHT, MIDDLE, LEFT
};
