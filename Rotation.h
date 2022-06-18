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
