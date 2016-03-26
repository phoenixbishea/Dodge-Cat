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

#include "PlayerInputComponent.hpp"
#include "PlayerPhysicsComponent.hpp"
#include "PlayerGraphicsComponent.hpp"
#include "PlayerCameraComponent.hpp"

#include "PlayerData.hpp"

class Player
{
public:
    Player(Ogre::SceneManager* graphics, BulletPhysics* physics, Ogre::Camera* camera)
        : mInput(new PlayerInputComponent()),
        mPhysics(new PlayerPhysicsComponent(data, physics)),
        mGraphics(new PlayerGraphicsComponent(data, graphics)),
        mCamera(new PlayerCameraComponent(graphics, camera))
    {
    }

    bool update(BulletPhysics* physics, OIS::Keyboard* keyboard, OIS::Mouse* mouse, float elapsedTime)
    {
        mInput->update(data, keyboard, mouse, elapsedTime);
        if(!mPhysics->update(data, physics, elapsedTime))
        {
            return false;
        }
        mGraphics->update(data);
        mCamera->update(data);

        return true;
    }

    btTransform getWorldTransform()
    {
        return mPhysics->charController->getGhostObject()->getWorldTransform();
    }

    Vector getCannonDirection()
    {
        return Vector(mGraphics->cannonNode->_getDerivedOrientation() * Ogre::Vector3(0, 0, -1));
    }
private:
    PlayerInputComponent* mInput;
    PlayerPhysicsComponent* mPhysics;
    PlayerGraphicsComponent* mGraphics;
    PlayerCameraComponent* mCamera;

    PlayerData data;
};

#endif
