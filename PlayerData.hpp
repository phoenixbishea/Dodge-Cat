#ifndef PlayerData_hpp
#define PlayerData_hpp

#include "Vector.hpp"
#include "Quaternion.hpp"

struct PlayerData
{
	Vector velocity;
    Quaternion orientation;

    Vector sightPosition = Vector(0, 0, -200);
    Vector cameraPosition = Vector(0, 300, 500);

    float cannonPitch;
    Quaternion cannonOrientation;
};

#endif
