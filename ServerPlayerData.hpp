#ifndef ServerPlayerData_hpp
#define ServerPlayerData_hpp

#include "Vector.hpp"
#include "Quaternion.hpp"

#define PLAYERDATA_LENGTH 40
#define PLAYERLOSE_LENGTH 20
#define PLAYERWIN_LENGTH  15

struct ServerPlayerData
{
    int playerNum;

    Vector position;
    Quaternion orientation;

    float cannonPitch;

    char buf[PLAYERDATA_LENGTH];
    int dataLength;
};

/**
 * This array is used for the both the client and server side
 *
 * On the server side, the index used is in accordance with
 * each player's player number.
 *    Player 1's data is stored in index 0
 *    Player 2's data is stored in index 1
 *
 * On the client side, only index 0 is used, and it is used
 * to store the data about the other player that is received
 * from the server
 */
ServerPlayerData playerData[2];

#endif
