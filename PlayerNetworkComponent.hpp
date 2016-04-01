#ifndef PlayerNetworkComponent_hpp
#define PlayerNetworkComponent_hpp

#include <OgreSceneManager.h>

#include "PlayerData.hpp"
#include "ServerPlayerData.hpp"

class PlayerNetworkComponent
{
public:
	PlayerNetworkComponent()
	{
	}

	~PlayerNetworkComponent()
	{
	}
	
	void update(PlayerData& obj, Ogre::SceneNode* cannonNode)
	{
      // Set player position and orientation
      obj.position = playerData[0].position;
      obj.orientation = playerData[0].orientation;

      // Set cannon pitch amount for the graphics module to update
      float newPitch = playerData[0].cannonPitch;
      float oldPitch = cannonNode->getOrientation().getPitch().valueRadians();
      obj.cannonPitch = oldPitch - newPitch;
	}
};

#endif
