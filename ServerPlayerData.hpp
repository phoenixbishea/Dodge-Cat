#ifndef ServerPlayerData_hpp
#define ServerPlayerData_hpp

#include "Vector.hpp"
#define PLAYERDATA_LENGTH 224

struct ServerPlayerData
{
    int playerNum;

    Vector position;
    float orientation;

    float cannonPitch;

    char buf[PLAYERDATA_LENGTH];
};


ServerPlayerData playerData[2];

#endif
