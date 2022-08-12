#pragma once

#include <string>

static const int MAP_WIDTH = 8000;
static const int MAP_HEIGHT = 8000;
static const int NUM_ASTEROIDS_BIG = 25;
static const int NUM_ASTEROIDS_SMALL = 25;
static const int NUM_AMMO = 3;
static const double ABILITY_PROBABILITY = 0.5;
static const int BULLET_SPEED = 30;
static const int RELOAD_TIME = 15;

static const std::string gameVersion = "1.0.0";

enum class FRKey {
    RIGHT, LEFT, DOWN, UP
};

enum class FRMouseButton {
    RIGHT, MIDDLE, LEFT
};
