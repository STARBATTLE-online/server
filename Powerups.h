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

    std::string GetType() override
	{
		return "Powerup";
	}

    std::string Serialize() override {
        std::stringstream ss;
		ss << std::fixed << std::setprecision(0);
		ss << GetType() << " " << GetCenterGlobal().first << " " << GetCenterGlobal().second;

		return ss.str();
    }

    uint64_t GetCreationTick() {
        return creation_tick;
    }

    ~Powerup() override = default;
protected:
    uint64_t creation_tick = 0;
};

class Shield : public Powerup {
public:
    Shield(int x, int y, uint64_t creation_tick) {
        width = 126;
        height = 126;

        SetCoordsByCenter(x, y);
        this->creation_tick = creation_tick;
    }

    std::string GetType() override
	{
		return "Shield";
	}

    ~Shield() override = default;
};