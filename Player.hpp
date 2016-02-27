
#ifndef Player_hpp
#define Player_hpp

#include <OgreEntity.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <OgreSceneManager.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "BulletPhysics.hpp"

class Player
{
public:
    Player (Ogre::String name, Ogre::SceneManager* sceneMgr, BulletPhysics* physicsEngine);

    ~Player ();

    // Updates the Player (movement...)
    void update (Ogre::Real elapsedTime, OIS::Keyboard* input, OIS::Mouse* mouse);

    // The three methods below returns the two camera-related nodes, 
    // and the current position of the Player (for the 1st person camera)
    Ogre::SceneNode* getSightNode ();

    Ogre::SceneNode* getCameraNode ();

    Ogre::Vector3 getWorldPosition ();

    void updateAction(btCollisionWorld* world, btScalar dt);

    btTransform& getWorldTransform();

    void setOgrePosition(Ogre::Vector3 vec);
    void setOgreOrientation(Ogre::Quaternion q);

    float getCollisionObjectHalfHeight();

protected:
    Ogre::String mName;
    btPairCachingGhostObject* ghost;
    btKinematicCharacterController* player;
    //btRigidBody* body;
    Ogre::SceneNode* mMainNode;
    Ogre::SceneNode* mSightNode; // "Sight" node - The Player is supposed to be looking here
    Ogre::SceneNode* mCameraNode; // Node for the chase camera
    Ogre::Entity* mEntity; // Player entity
    Ogre::SceneManager* mSceneMgr;
};

#endif
