#include "Player.hpp"

#include <iostream>
#include <cmath>

#define WALK_SPEED 500
#define MAX_ROTATION 2
#define DAMPING_FACTOR 0.5f
#define FPS 60

Player::Player (Ogre::String name, Ogre::SceneManager *sceneMgr, BulletPhysics* physicsEngine) 
{
    // Setup basic member references
    mName = name;
    mSceneMgr = sceneMgr;

    // Setup basic node structure to handle 3rd person cameras
    mMainNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode (mName);
    mSightNode = mMainNode->createChildSceneNode (mName + "_sight", Ogre::Vector3 (0, 0, -200));
    mCameraNode = mMainNode->createChildSceneNode (mName + "_camera", Ogre::Vector3 (0, 300, 500));

    // Give this character a shape :)
    mEntity = mSceneMgr->createEntity (mName, "ninja.mesh");
    mMainNode->attachObject (mEntity);

    btBoxShape* shape = new btBoxShape(btVector3(40.0, 100.0, 10.0));
    ghost = new btPairCachingGhostObject();

    btTransform t = ghost->getWorldTransform();
    t.setOrigin(btVector3(0.0, 10.0, 0.0));
    ghost->setWorldTransform(t);

    // physicsEngine->getDynamicsWorld()->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    ghost->setCollisionShape(shape);
    ghost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    player = new btKinematicCharacterController(ghost, shape, 1.0);
    physicsEngine->getDynamicsWorld()->addCollisionObject(ghost,
                                                          btBroadphaseProxy::CharacterFilter,
                                                          btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    physicsEngine->getDynamicsWorld()->addAction(player);


    btVector3 trans = ghost->getWorldTransform().getOrigin();
    std::cout << trans.x() << ", " << trans.y() << ", " << trans.z() << std::endl; 

}

Player::~Player () 
{
    //  mMainNode->detachAllObjects ();
    delete mEntity;
//    mMainNode->removeAndDestroyAllChildren ();
    //mSceneMgr->destroySceneNode (mName);
}

// Updates the Player (movement...)
void Player::update (Ogre::Real elapsedTime, OIS::Keyboard* input, OIS::Mouse* mouse) 
{
    const OIS::MouseState& me = mouse->getMouseState();
    
    // Forward movement
    if (input->isKeyDown (OIS::KC_W) || input->isKeyDown(OIS::KC_COMMA))
    {
        Ogre::Quaternion orientation = mMainNode->getOrientation();
        Ogre::Vector3 direction = orientation * Ogre::Vector3(0, 0, -WALK_SPEED); // * elapsedTime);
        
        // Create bullet vector 3 where it will be moving
        btVector3 move(direction.x, direction.y, direction.z);

        // Update the player via bullet. Vector it will move along and how far they will move per second
        player->setVelocityForTimeInterval(move, elapsedTime * FPS);
        // btVector3 t = player->getGhostObject()->getWorldTransform().getOrigin();
        // std::cout << "player position: " << t.x() << " " << t.y() << " " << t.z() << std::endl;
    }
    // Backward Movement (same idea as in forward movement)
    if (input->isKeyDown (OIS::KC_S) || input->isKeyDown(OIS::KC_O))
    {
        Ogre::Quaternion orientation = mMainNode->getOrientation();
        Ogre::Vector3 direction = orientation * Ogre::Vector3(0, 0, WALK_SPEED);// * elapsedTime);
        btVector3 move(direction.x, direction.y, direction.z);

        player->setVelocityForTimeInterval(move, elapsedTime * FPS);
    }

    // Camera movement based on mouse movement
    if (me.Y.rel < -0.1f || me.Y.rel > 0.1f)
    {
        Ogre::Real upperCam = 600.0; // how high camera can go up
        Ogre::Real upperSight = 500.0; // how high sight node can go up
        Ogre::Real lower = 0.0; // how low each node can go

        // Get y of each node
        Ogre::Real sightY = mSightNode->getPosition().y;
        Ogre::Real camY = mCameraNode->getPosition().y;

        // Find the new position of the nodes relative to y mouse movement and clamp using the bounds
        sightY = std::max(lower, std::min(sightY - me.Y.rel * DAMPING_FACTOR, upperSight));
        camY = std::max(lower, std::min(camY + me.Y.rel * DAMPING_FACTOR, upperCam));

        // Only update sight node if camera is less than or equal to 300 in y
        if (camY <= 300.0f)
        {
            // Camera class relies on this...
            mSightNode->setPosition(Ogre::Vector3(0, sightY, -200));
        }   

        // Only update camera node y position if sight node y position is less than 300
        if (sightY <= 300.0f)
        {
            mCameraNode->setPosition(Ogre::Vector3(0, camY, 500));
        }
        // Sight node y pos is greater than 300 and cam node y == 0
        else
        {
            // The x movement of cam should be 50% slower than the y movement of sight
            Ogre::Real camZ = mCameraNode->getPosition().z;
            camZ += me.Y.rel * DAMPING_FACTOR * 0.5f;
            camZ = std::max(100.0f, std::min(camZ, 500.0f));
            mCameraNode->setPosition(Ogre::Vector3(0, 0, camZ));
        }
    }

    // Left rotation
    if (input->isKeyDown (OIS::KC_A))
    {
        // Ghost object is represenation of kinematic controller?
        btTransform t = player->getGhostObject()->getWorldTransform();
        btQuaternion orientation = t.getRotation();

        btQuaternion rotation;
        rotation = rotation.getIdentity();
        rotation.setX(0); // Is this necessary?
        rotation.setY(MAX_ROTATION * elapsedTime);
        rotation.setZ(0); // Is this necessary?

        orientation = rotation * orientation;

        // Set the results
        t.setRotation(orientation);
        player->getGhostObject()->setWorldTransform(t);
    }

    // Right rotation
    if (input->isKeyDown (OIS::KC_D) || input->isKeyDown(OIS::KC_E))
    {
        btTransform t = player->getGhostObject()->getWorldTransform();
        btQuaternion orientation = t.getRotation();

        btQuaternion rotation;
        rotation = rotation.getIdentity();
        rotation.setX(0); // Is this necessary?
        rotation.setY(-MAX_ROTATION * elapsedTime);
        rotation.setZ(0); // Is this necessary?

        orientation = rotation * orientation;

        t.setRotation(orientation);
        player->getGhostObject()->setWorldTransform(t);
    }
}

// The three methods below returns the two camera-related nodes, 
// and the current position of the Player (for the 1st person camera)
Ogre::SceneNode* Player::getSightNode () 
{
    return mSightNode;
}

Ogre::SceneNode* Player::getCameraNode () 
{
    return mCameraNode;
}

void Player::updateAction(btCollisionWorld* world, btScalar dt)
{
    this->player->playerStep(world, dt);
}

btPairCachingGhostObject* Player::getGhostObject()
{
    return this->player->getGhostObject();
}

btTransform& Player::getWorldTransform()
{
    return this->player->getGhostObject()->getWorldTransform();
}

Ogre::Vector3 Player::getOgrePosition()
{
    return this->mMainNode->_getDerivedPosition();
}

void Player::setOgrePosition(Ogre::Vector3 vec) {
    this->mMainNode->translate(vec - mMainNode->_getDerivedPosition());
}

void Player::setOgreOrientation(Ogre::Quaternion q) {
    this->mMainNode->setOrientation(q);
}

float Player::getCollisionObjectHalfHeight() {
    return 100.0;
}
