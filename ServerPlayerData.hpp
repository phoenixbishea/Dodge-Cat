#ifndef ServerPlayerData_hpp
#define ServerPlayerData_hpp

#include "Vector.hpp"

struct ServerPlayerData
{
    int playerNum;

    Vector position;
    float orientation;

    float cannonPitch;
};

ServerPlayerData playerData[2];

#endif
