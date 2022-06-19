#pragma once 

class Powerup : public MovableSprite {
protected:
    Powerup() = default;
public:
    Powerup(int width, int height, uint64_t creation_tick) {
        this->width = width;
		this->height = height;
        this->creation_tick = creation_tick;
    }

    std::string getType() override {
		return "Powerup";
	}

    std::string serialize() override {
        std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << getType() << " " << getCenterGlobal().first << " " << getCenterGlobal().second;

		return ss.str();
    }

    uint64_t getCreationTick() {
        return creation_tick;
    }

    ~Powerup() override = default;

    virtual void activate(Ship* ship) {}
    
protected:
    uint64_t creation_tick = 0;
};

class Shield : public Powerup {
public:
    Shield(int x, int y, uint64_t creation_tick) {
        width = 126;
        height = 126;

        setCoordsByCenter(x, y);
        this->creation_tick = creation_tick;
    }

    std::string getType() override {
		return "Shield";
	}

    ~Shield() override = default;

    /* 
     * Is supposed to modify the ship 
     */
    void activate(Ship* ship) override {
        ship->setShieldDuration(200);
    }
};

class Barrage : public Powerup {
public:
    Barrage(int x, int y, uint64_t creation_tick) {
        width = 126;
        height = 126;

        setCoordsByCenter(x, y);
        this->creation_tick = creation_tick;
    }

    std::string getType() override {
		return "Barrage";
	}

    ~Barrage() override = default;

    /* 
     * Is supposed to modify the ship 
     */
    void activate(Ship* ship) override {
        ship->setBarrageDuration(200);
    }
};