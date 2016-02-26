#include "Player.hpp"

#include <iostream>

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

    btCylinderShape* shape = new btCylinderShape(btVector3(20.0, 50.0, 20.0));
    ghost = new btPairCachingGhostObject();

    btTransform t = ghost->getWorldTransform();
    t.setOrigin(btVector3(0.0, 0.0, 0.0));
    ghost->setWorldTransform(t);

    // physicsEngine->getDynamicsWorld()->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    ghost->setCollisionShape(shape);
    ghost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    player = new btKinematicCharacterController(ghost, shape, 1.0);
    physicsEngine->getDynamicsWorld()->addCollisionObject(ghost,
                                                          btBroadphaseProxy::CharacterFilter,
                                                          btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    physicsEngine->getDynamicsWorld()->addAction(player);
    

    // btTransform t;
    // t.setIdentity();
    // t.setOrigin(btVector3(0, 200, 0));

    // btScalar mass(100.0);
    // btVector3 inertia(1, 1, 1);

    // btCollisionShape* shape = new btBoxShape(btVector3(100.0, 10.0, 100.0));
    // btDefaultMotionState* ms = new btDefaultMotionState(t);

    // shape->calculateLocalInertia(mass, inertia);

    // btRigidBody::btRigidBodyConstructionInfo RBInfo(mass, ms, shape, inertia);
    // this->body = new btRigidBody(RBInfo);
    // this->body->setUserPointer(mMainNode);
    //this->body->setAngularFactor(btVector3(0, 1, 0));
    //this->body->setLinearFactor(btVector3(1, 0, 1));

    //add the body to the dynamics world
    // physicsEngine->getDynamicsWorld()->addRigidBody(body);
    // physicsEngine->trackRigidBodyWithName(body, std::string("Player"));
}

Player::~Player () 
{
    //  mMainNode->detachAllObjects ();
    delete mEntity;
//    mMainNode->removeAndDestroyAllChildren ();
    //mSceneMgr->destroySceneNode (mName);
}

// Updates the Player (movement...)
void Player::update (Ogre::Real elapsedTime, OIS::Keyboard *input) 
{
    player->setWalkDirection(btVector3(0, 0, 0));
//    Handle movement
    if (input->isKeyDown (OIS::KC_W) || input->isKeyDown(OIS::KC_COMMA))
    {
        Ogre::Quaternion orientation = mMainNode->getOrientation();
        Ogre::Vector3 direction = orientation * Ogre::Vector3(0, 0, -100 * elapsedTime);
        btVector3 move(direction.x, direction.y, direction.z);

        player->setWalkDirection (move);
        btVector3 t = player->getGhostObject()->getWorldTransform().getOrigin();
        std::cout << "player position: " << t.x() << " " << t.y() << " " << t.z() << std::endl;
    }
    if (input->isKeyDown (OIS::KC_S) || input->isKeyDown(OIS::KC_O))
    {
        Ogre::Quaternion orientation = mMainNode->getOrientation();
        Ogre::Vector3 direction = orientation * Ogre::Vector3(0, 0, 100 * elapsedTime);
        btVector3 move(direction.x, direction.y, direction.z);
        player->setWalkDirection (move);
    }
    if (input->isKeyDown (OIS::KC_A))
    {
        btTransform t = player->getGhostObject()->getWorldTransform();
        btQuaternion orientation = t.getRotation();
        btQuaternion rotation;
        rotation = rotation.getIdentity();
        rotation.setX(0);
        rotation.setY(elapsedTime);
        rotation.setZ(0);
        orientation = rotation * orientation;
        t.setRotation(orientation);
        player->getGhostObject()->setWorldTransform(t);
    }
    if (input->isKeyDown (OIS::KC_D) || input->isKeyDown(OIS::KC_E))
    {
        btTransform t = player->getGhostObject()->getWorldTransform();
        btQuaternion orientation = t.getRotation();
        btQuaternion rotation;
        rotation = rotation.getIdentity();
        rotation.setX(0);
        rotation.setY(-elapsedTime);
        rotation.setZ(0);
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

void Player::setPosition(Ogre::Vector3 vec) {
    this->mMainNode->setPosition(vec);
}

void Player::setOrientation(Ogre::Quaternion q) {
    this->mMainNode->setOrientation(q);
}

