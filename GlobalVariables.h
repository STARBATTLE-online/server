#pragma once
inline int MAP_WIDTH = 1000;
inline int MAP_HEIGHT = 1000;
inline int NUM_ASTEROIDS_BIG = 5;
inline int NUM_ASTEROIDS_SMALL = 5;
inline int NUM_AMMO = 3;
inline double ABILITY_PROBABILITY = 0.5;

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
