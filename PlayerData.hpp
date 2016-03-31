#ifndef PlayerData_hpp
#define PlayerData_hpp

#include "Vector.hpp"
#include "Quaternion.hpp"

struct PlayerData
{
	Vector position;
	Vector velocity;
    Quaternion orientation;

    // Used for updating player nodes
    Vector sightPosition;// = Vector(0, 0, -200);
    Vector cameraPosition;// = Vector(0, 300, 500);

    // Used getting derived positions
    Vector derivedSight;// = Vector(0, 0, -200);
    Vector derivedCamera;

    float cannonPitch;
    Quaternion cannonOrientation;

    PlayerData()
	    : position(0.0f, 0.0f, 0.0f),
	    velocity(0.0f, 0.0f, 0.0f),
	    orientation(0.0f, 0.0f, 0.0f, 1.0f),
	    sightPosition(0.0f, 0.0f, -200.0f),
	    cameraPosition(0.0f, 300.0f, 500.0f),
	    derivedSight(0.0f, 0.0f, -200.0f),
	    derivedCamera(0.0f, 300.0f, 500.0f),
	    cannonPitch(0.0f),
	    cannonOrientation(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }
};

#endif
