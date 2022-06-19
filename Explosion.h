#pragma once

class Explosion : public MovableSprite {
protected:
    Explosion() = default;
public:
    Explosion(int width, int height, uint64_t creation_tick) {
        this->width = width;
		this->height = height;
        this->creation_tick = creation_tick;
    }

    std::string getType() override {
		return "Explosion";
	}

    std::string serialize() override {
        std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << getType() << " " << getCenterGlobal().first << " " << getCenterGlobal().second << " " << creation_tick;

		return ss.str();
    }

    uint64_t getCreationTick() {
        return creation_tick;
    }

    ~Explosion() override = default;
protected:
    uint64_t creation_tick = 0;
};

class BigExplosion : public Explosion {
public:
    BigExplosion(int x, int y, uint64_t creation_tick) {
        width = 384;
        height = 384;

        setCoordsByCenter(x, y);
        this->creation_tick = creation_tick;
    }

    std::string getType() override {
		return "BigExplosion";
	}

    ~BigExplosion() override = default;
};

class SmallExplosion : public Explosion {
public:
    SmallExplosion(int x, int y, uint64_t creation_tick) {
        width = 200;
        height = 200;

        setCoordsByCenter(x, y);
        this->creation_tick = creation_tick;
    }

    std::string getType() override {
		return "SmallExplosion";
	}

    ~SmallExplosion() override = default;
};