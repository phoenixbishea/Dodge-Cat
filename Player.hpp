#ifndef Player_hpp
#define Player_hpp

#include <OgreEntity.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <OgreSceneManager.h>
#include <OgreSubMesh.h>
#include <OgreMeshManager.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "BulletPhysics.hpp"
#include "NetManager.h"

#include "PlayerInputComponent.hpp"
#include "PlayerPhysicsComponent.hpp"
#include "PlayerGraphicsComponent.hpp"
#include "PlayerCameraComponent.hpp"
#include "PlayerNetworkComponent.hpp"

#include "PlayerData.hpp"

class Player
{
public:
    Player(Ogre::SceneManager* graphics, BulletPhysics* physics, Ogre::Camera* camera)
        : mInput(new PlayerInputComponent()),
        mPhysics(new PlayerPhysicsComponent(data, physics)),
        mGraphics(new PlayerGraphicsComponent(data, graphics)),
        mCamera(new PlayerCameraComponent(graphics, camera)),
        mNetwork(new PlayerNetworkComponent())
    {
    }

    bool update(BulletPhysics* physics, OIS::Keyboard* keyboard, OIS::Mouse* mouse, float elapsedTime)
    {
        if (mInput) mInput->update(data, keyboard, mouse, elapsedTime);
        else if (mNetwork) mNetwork->update(data);

        if(!mPhysics->update(data, physics, elapsedTime)) return false;

        mGraphics->update(data);
        
        if (mCamera) mCamera->update(data);

        return true;
    }

    btTransform getWorldTransform()
    {
        return mPhysics->getTransform();
    }

    Vector getCannonDirection()
    {
        return Vector(mGraphics->cannonNode->_getDerivedOrientation() * Ogre::Vector3(0, 0, -1));
    }

    void serializeData(char* buf, int playerNum)
    {
        // Put DC_PINFO at the start of the string
        memcpy(buf, STR_PINFO.c_str(), STR_PINFO.length());

        int* buf_int = (int*) (buf + 8);

        // Put the player number in the string
        *buf_int++ = playerNum;

        // Put the player's x, y, and z
        btVector3 position = this->getWorldTransform().getOrigin();
        *buf_int++ = position.x();
        *buf_int++ = position.y();
        *buf_int++ = position.z();

        // Put the player's rotation with respect to 0, 1, 0
        btQuaternion orientation = this->getWorldTransform().getRotation();
        *buf_int++ = orientation.w();

        // Put the player's horizontal pitch
        Ogre::Real pitch = mGraphics->cannonNode->getOrientation().getPitch().valueDegrees();
        *((float*)buf_int) = pitch;
    }
private:
    PlayerInputComponent* mInput;
    PlayerPhysicsComponent* mPhysics;
    PlayerGraphicsComponent* mGraphics;
    PlayerCameraComponent* mCamera;
    PlayerNetworkComponent* mNetwork;

	PlayerData data;	 
};

#endif
