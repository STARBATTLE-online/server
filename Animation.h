#pragma once
#include "MovableSprite.h"
#include "Rotation.h"
class Animation : public MovableSprite {
public:
    Animation() = default;

    virtual bool hasEnded() {
        return false;
    }

    std::string getType() override {
        return "Animation";
    }

    virtual void tick() {}

    virtual ~Animation() = default;
};

class LeftShipAnimation : public Animation {
public:
    LeftShipAnimation() {
        x_speed = y_speed = mass = 0;
        setCoordsByCenter(4000 - 286, 4000);
    }

    bool hasEnded() override {
        return getCenterGlobal().first <= 4000 - 686;
    }

    std::string getType() override {
        return "LeftShipAnimation";
    }

    void tick() override {
        auto [x, y] = getCenterGlobal();
        setCoordsByCenter(x - 1, y);
    }

    std::string serialize() override {
        std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << "Ship" << " " << getCenterGlobal().first << " " << getCenterGlobal().second << " " << Rotation::Left << " " << x_speed << " " << y_speed  << " " << 7 << " " << 0 << " " << 10 << " " << 0 << " " << 0 << " " << 0 << " ";

		return ss.str();
    }
};

