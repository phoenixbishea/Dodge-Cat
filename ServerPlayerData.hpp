#ifndef ServerPlayerData_hpp
#define ServerPlayerData_hpp

#include "Vector.hpp"
#include "Quaternion.hpp"

#define PLAYERDATA_LENGTH 224

struct ServerPlayerData
{
    int playerNum;

    Vector position;
    Quaternion orientation;

    float cannonPitch;

    char buf[PLAYERDATA_LENGTH];
};


ServerPlayerData playerData[2];

#endif
