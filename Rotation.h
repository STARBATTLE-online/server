//
// Created by fail on 6/11/22.
//

#ifndef STARWARS_SERVER_ROTATION_H
#define STARWARS_SERVER_ROTATION_H

enum Rotation
{
    Top,
    Left,
    Bottom,
    Right
};

Rotation RotationFromString(std::string s) {
    if(s == "T") {
        return Top;
    } else if(s == "L") {
        return Left;
    } else if(s == "D") {
        return Bottom;
    } else if(s == "R") {
        return Right;
    } else {
        return Top;
    }
}

std::ostream& operator<<(std::ostream& os, Rotation r) {
    switch (r) {
        case Rotation::Top:
            os << "T";
            break;
        case Rotation::Left:
            os << "L";
            break;
        case Rotation::Bottom:
            os << "B";
            break;
        case Rotation::Right:
            os << "R";
            break;
    }
    return os;
}

#endif //STARWARS_SERVER_ROTATION_H
