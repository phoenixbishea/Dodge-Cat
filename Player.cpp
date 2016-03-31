#include "Player.hpp"

#include <iostream>
#include <cmath>
#include <string>

#define WALK_SPEED 500
#define MAX_ROTATION 2
#define DAMPING_FACTOR 0.5f
#define FPS 60
#define PADDLE_HEIGHT 350.0
#define PADDLE_OFFSET 110.0

Player::Player (Ogre::String name, Ogre::SceneManager *sceneMgr, BulletPhysics* physicsEngine, Sound* sound) 
{
    // Setup basic member references
    mName = name;
    mSceneMgr = sceneMgr;

    mSound = sound;

    // Setup basic node structure to handle 3rd person cameras
    mMainNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode (mName);
    mSightNode = mMainNode->createChildSceneNode (mName + "_sight", Ogre::Vector3 (0, 0, -200));
    mCameraNode = mMainNode->createChildSceneNode (mName + "_camera", Ogre::Vector3 (0, 300, 500));

    // Give this character a shape :)
    Ogre::MeshManager::getSingleton()
      .create("cannon/CannonBase.mesh",
              Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::MeshManager::getSingleton()
      .create("cannon/CannonSpray.mesh",
              Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    Ogre::Entity* baseEntity =
      mSceneMgr
      ->createEntity(Ogre::MeshManager::getSingleton()
                     .getByName("cannon/CannonBase.mesh",
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
    Ogre::Entity* sprayEntity =
      mSceneMgr
      ->createEntity(Ogre::MeshManager::getSingleton()
                     .getByName("cannon/CannonSpray.mesh",
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
    Ogre::SceneNode* baseNode = mMainNode->createChildSceneNode();
    mCannonNode = mMainNode->createChildSceneNode();

    // Put the cannon base into position
    baseNode->attachObject(baseEntity);
    baseNode->roll(Ogre::Radian(Ogre::Degree(180)));
    baseNode->yaw(Ogre::Radian(Ogre::Degree(90)));

    // Put the cannon and spray bottle into position
    Ogre::SceneNode* cannonNode = mCannonNode->createChildSceneNode();
    cannonNode->attachObject(sprayEntity);
    cannonNode->roll(Ogre::Radian(Ogre::Degree(180)));
    cannonNode->yaw(Ogre::Radian(Ogre::Degree(180)));
    cannonNode->translate(Ogre::Vector3(0.0, 100.0, 0.0));
    cannonNode->scale(Ogre::Vector3(0.77, 0.77, 0.77));

    // Scale both parts of the cannon
    mMainNode->scale(Ogre::Vector3(0.6, 0.6, 0.6));

    btBoxShape* boxShape = new btBoxShape(btVector3(40.0, 70.0, 40.0));

    ghost = new btPairCachingGhostObject();

    btTransform t = ghost->getWorldTransform();
    t.setOrigin(btVector3(0.0, 0.0, 0.0));
    ghost->setWorldTransform(t);

    // physicsEngine->getDynamicsWorld()->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    ghost->setCollisionShape(boxShape);
    ghost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    player = new btKinematicCharacterController(ghost, boxShape, 1.0);
    physicsEngine->getDynamicsWorld()->addCollisionObject(ghost,
                                                          btBroadphaseProxy::CharacterFilter,
                                                          btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    physicsEngine->getDynamicsWorld()->addAction(player);

    btVector3 trans = ghost->getWorldTransform().getOrigin();

    btTransform boxTrans = ghost->getWorldTransform();
    btVector3 vec = boxTrans.getOrigin() + btVector3(0, PADDLE_HEIGHT / 2, -150);
    btQuaternion rotation;
    boxTrans.setOrigin(vec);
    boxTrans.setRotation(rotation);

    btScalar boxMass(0.0);
    btVector3 localBoxInertia(0, 0, 0);

    btBoxShape* boxShape2 = new btBoxShape(btVector3(PADDLE_HEIGHT, PADDLE_HEIGHT, 2));
    physicsEngine->getCollisionShapes().push_back(boxShape2);
    btDefaultMotionState* boxMotionState = new btDefaultMotionState(boxTrans);

    boxShape2->calculateLocalInertia(boxMass, localBoxInertia);

    btRigidBody::btRigidBodyConstructionInfo boxRBInfo(boxMass, boxMotionState, boxShape2, localBoxInertia);
    paddleBody = new btRigidBody(boxRBInfo);
    paddleBody->setRestitution(1.0);

    physicsEngine->getDynamicsWorld()->addRigidBody(paddleBody);
}

Player::~Player ()
{
    delete mEntity;
}

// Updates the Player (movement...)
void Player::update (Ogre::Real elapsedTime, OIS::Keyboard* input, OIS::Mouse* mouse) 
{
    const OIS::MouseState& me = mouse->getMouseState();

    // Forward movement
    if (input->isKeyDown (OIS::KC_W) || input->isKeyDown(OIS::KC_COMMA) || input->isKeyDown(OIS::KC_UP))
    {
        Ogre::Quaternion orientation = mMainNode->getOrientation();
        Ogre::Vector3 direction = orientation * Ogre::Vector3(0, 0, -WALK_SPEED); // * elapsedTime);

        // Create bullet vector 3 where it will be moving
        btVector3 move(direction.x, direction.y, direction.z);

        // Update the player via bullet. Vector it will move along and how far they will move per second
        player->setVelocityForTimeInterval(move, elapsedTime * FPS);
    }
    // Backward Movement (same idea as in forward movement)
    if (input->isKeyDown (OIS::KC_S) || input->isKeyDown(OIS::KC_O) || input->isKeyDown(OIS::KC_DOWN))
    {
        Ogre::Quaternion orientation = mMainNode->getOrientation();
        Ogre::Vector3 direction = orientation * Ogre::Vector3(0, 0, WALK_SPEED);
        btVector3 move(direction.x, direction.y, direction.z);

        player->setVelocityForTimeInterval(move, elapsedTime * FPS);
    }

    // Camera movement based on mouse movement
    if (me.Y.rel < -0.1f || me.Y.rel > 0.1f)
    {
        mSound->playSound("move");

        Ogre::Real upperCam = 600.0; // how high camera can go up
        Ogre::Real upperSight = 500.0; // how high sight node can go up
        Ogre::Real lower = 0.0; // how low each node can go

        // Get y of each node
        Ogre::Real sightY = mSightNode->getPosition().y;
        Ogre::Real camY = mCameraNode->getPosition().y;

        // Find the new position of the nodes relative to y mouse movement and
        // clamp using the bounds
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

        // Rotate the cannon and spray bottle based on the location of the
        // sight node and the camera node
        Ogre::Degree pitch(mCannonNode->getOrientation().getPitch());
        pitch -= Ogre::Degree(me.Y.rel * 0.03);
        if (pitch < Ogre::Degree(85.0f) && pitch > Ogre::Degree(-10.0f))
          mCannonNode->pitch(-Ogre::Radian(Ogre::Degree(me.Y.rel * 0.03)));
    }

    // Left rotation
    if (input->isKeyDown (OIS::KC_A) || input->isKeyDown(OIS::KC_LEFT))
    {
        // Ghost object is represenation of kinematic controller
        btTransform t = player->getGhostObject()->getWorldTransform();
        btQuaternion orientation = t.getRotation();

        btQuaternion rotation;
        rotation = rotation.getIdentity();
        rotation.setX(0);
        rotation.setY(MAX_ROTATION * elapsedTime);
        rotation.setZ(0);

        orientation = rotation * orientation;

        // Set the results
        t.setRotation(orientation);
        player->getGhostObject()->setWorldTransform(t);
        mSound->playSound("move");
    }

    // Right rotation
    if (input->isKeyDown(OIS::KC_D) || input->isKeyDown(OIS::KC_E) || input->isKeyDown(OIS::KC_RIGHT))
    {
        btTransform t = player->getGhostObject()->getWorldTransform();
        btQuaternion orientation = t.getRotation();

        btQuaternion rotation;
        rotation = rotation.getIdentity();
        rotation.setX(0);
        rotation.setY(-MAX_ROTATION * elapsedTime);
        rotation.setZ(0);

        orientation = rotation * orientation;

        t.setRotation(orientation);
        player->getGhostObject()->setWorldTransform(t);
        mSound->playSound("move");
    }

    Ogre::Quaternion orientation = mCannonNode->_getDerivedOrientation();

    Ogre::Vector3 direction = orientation * Ogre::Vector3(0, 0, -PADDLE_OFFSET);

    btVector3 move(direction.x, direction.y, direction.z);

    btTransform trans = ghost->getWorldTransform();
    btVector3 origin = trans.getOrigin() + move;
    if (origin.y() < PADDLE_HEIGHT / 2)
      origin.setY(PADDLE_HEIGHT / 2);
    trans.setOrigin(origin);
    trans.setRotation(btQuaternion(orientation.x,
                                   orientation.y,
                                   orientation.z,
                                   orientation.w));
    paddleBody->setWorldTransform(trans);
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

Ogre::Vector3 Player::getOgreLookDirection()
{
    return this->mCannonNode->_getDerivedOrientation() * Ogre::Vector3(0, 0, -1);
}

void Player::setOgrePosition(Ogre::Vector3 vec) {
    this->mMainNode->translate(vec - mMainNode->_getDerivedPosition());
}

void Player::setOgreOrientation(Ogre::Quaternion q) {
    this->mMainNode->setOrientation(q);
}

float Player::getCollisionObjectHalfHeight() {
    return 70.0;
}

std::string Player::serializeData() {
    return std::string();
}

