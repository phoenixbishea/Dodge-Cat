#ifndef PlayerNetworkComponent_hpp
#define PlayerNetworkComponent_hpp

#include <OgreSceneManager.h>

#include "PlayerData.hpp"

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
		// update player obj.position
		// update the player obj.orientation
		// update playerGraphicsComponent->cannonNode from graphics component with proper pitch
	}
private:

};

#endif
