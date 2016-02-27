#include "Player.hpp"

#include <iostream>
#include <cmath>

#define WALK_SPEED 500
#define MAX_ROTATION 2
#define DAMPING_FACTOR 0.3f
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

    // player->setWalkDirection(btVector3(0, 0, 0));
//    Handle movement
    if (input->isKeyDown (OIS::KC_W) || input->isKeyDown(OIS::KC_COMMA))
    {
        Ogre::Quaternion orientation = mMainNode->getOrientation();
        Ogre::Vector3 direction = orientation * Ogre::Vector3(0, 0, -WALK_SPEED); // * elapsedTime);
        btVector3 move(direction.x, direction.y, direction.z);

        // player->setWalkDirection (move);
        player->setVelocityForTimeInterval(move, elapsedTime * FPS);
        btVector3 t = player->getGhostObject()->getWorldTransform().getOrigin();
        std::cout << "player position: " << t.x() << " " << t.y() << " " << t.z() << std::endl;
    }
    if (input->isKeyDown (OIS::KC_S) || input->isKeyDown(OIS::KC_O))
    {
        Ogre::Quaternion orientation = mMainNode->getOrientation();
        Ogre::Vector3 direction = orientation * Ogre::Vector3(0, 0, WALK_SPEED);// * elapsedTime);
        btVector3 move(direction.x, direction.y, direction.z);
        // player->setWalkDirection (move);
        player->setVelocityForTimeInterval(move, elapsedTime * FPS);
    }

    if (me.Y.rel < -0.1f || me.Y.rel > 0.1f)
    {
        Ogre::Real upperCam = 600.0;
        Ogre::Real upperSight = 300.0;
        Ogre::Real lower = 0.0;

        Ogre::Real camY = mCameraNode->getPosition().y;
        camY = std::max(lower, std::min(camY + me.Y.rel * DAMPING_FACTOR, upperCam));
        mCameraNode->setPosition(Ogre::Vector3(0, camY, 500));

        // Only update sight node if camera is less than or equal to 300 in y
        if (camY <= 300.0f)
        {
            Ogre::Real sightY = mSightNode->getPosition().y;
            sightY = std::max(lower, std::min(sightY - me.Y.rel * DAMPING_FACTOR, upperSight));
            mSightNode->setPosition(Ogre::Vector3(0, sightY, -200));
        }   
    }

    float rotationSpeed = 0;
    if (me.X.rel > 0.1f)
    {
        rotationSpeed = std::max(me.X.rel, MAX_ROTATION) * DAMPING_FACTOR;
    }
    else if (me.X.rel < -0.1f)
    {
        rotationSpeed = std::min(me.X.rel, -MAX_ROTATION) * DAMPING_FACTOR;
    }

    btTransform t = player->getGhostObject()->getWorldTransform();
    btQuaternion orientation = t.getRotation();
    btQuaternion rotation;
    rotation = rotation.getIdentity();
    rotation.setX(0);
    rotation.setY(-rotationSpeed * elapsedTime);
    rotation.setZ(0);
    orientation = rotation * orientation;
    t.setRotation(orientation);
    player->getGhostObject()->setWorldTransform(t);

    // }
    // if (input->isKeyDown (OIS::KC_D) || input->isKeyDown(OIS::KC_E))
    // {
    //     btTransform t = player->getGhostObject()->getWorldTransform();
    //     btQuaternion orientation = t.getRotation();
    //     btQuaternion rotation;
    //     rotation = rotation.getIdentity();
    //     rotation.setX(0);
    //     rotation.setY(-ROTATION_SPEED * elapsedTime);
    //     rotation.setZ(0);
    //     orientation = rotation * orientation;
    //     t.setRotation(orientation);
    //     player->getGhostObject()->setWorldTransform(t);
    // }

    // if (input->isKeyDown(OIS::KC_SPACE))
    //     WALK_SPEED = 3000.0f;
    // else
    //     WALK_SPEED = 800.0f;
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

Ogre::Vector3 Player::getWorldPosition () 
{
//    return mMainNode->_getDerivedPosition ();
}

void Player::updateAction(btCollisionWorld* world, btScalar dt)
{
    this->player->playerStep(world, dt);
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
