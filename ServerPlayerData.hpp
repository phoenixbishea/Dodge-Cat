#ifndef ServerPlayerData_hpp
#define ServerPlayerData_hpp

#include <OgreMath.h>
#include <btBulletDynamicsCommon.h>

#include "Vector.hpp"

struct ServerPlayerData
{
    int playerNum;

	Vector position;
    btScalar orientation;

    Ogre::Real cannonPitch;
};

#endif
