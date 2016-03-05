#ifndef Cat_hpp
#define Cat_hpp

#include "BulletPhysics.hpp"
#include "Player.hpp"

#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreMeshManager.h>

#define SPAWN_DISTANCE 80.0f

class Cat
{
public:
    Cat(BulletPhysics*, Ogre::SceneManager*, Player* player);

    void initCatPhysics(const float catMass, const float sphereSize);
    void setVelocity(const float velocity);
    void initCatOgre(const char* meshName);

private:
	BulletPhysics* mPhysicsEngine;
	Ogre::SceneManager* mSceneMgr;
	Player* mPlayer;

	btRigidBody* mBody;

	btVector3 mPhysLookDir;
	btVector3 mVector;
	btTransform mTransform;
};

#endif

//---------------------------------------------------------------------------
Cat::Cat(BulletPhysics* physics, Ogre::SceneManager* scnMgr, Player* player)
	: mPhysicsEngine(physics),
	mSceneMgr(scnMgr),
	mPlayer(player),
	mBody(0)
{
}

//---------------------------------------------------------------------------
void Cat::initCatPhysics(const float catMass, const float sphereSize)
{
	mTransform = mPlayer->getWorldTransform();
    mVector = mTransform.getOrigin();

    btScalar mass(catMass);
    btVector3 localInertia(0, 0, 0);

    btCollisionShape* shape = new btSphereShape(sphereSize);
    mPhysicsEngine->getCollisionShapes().push_back(shape);
    btDefaultMotionState* motionState = new btDefaultMotionState(mTransform);

    shape->calculateLocalInertia(mass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass, motionState, 
    	shape, localInertia);
    mBody = new btRigidBody(rigidBodyInfo);
    mPhysicsEngine->getDynamicsWorld()->addRigidBody(mBody);

    mBody->setRestitution(1);
}

//---------------------------------------------------------------------------
void Cat::setVelocity(const float velocity)
{
	 // Set the velocity of the Cat based on sight and camera nodes attached to the player
    Ogre::Vector3 playerPos = mPlayer->getSightNode()->_getDerivedPosition();
    Ogre::Vector3 cameraPos = mPlayer->getCameraNode()->_getDerivedPosition();

    btVector3 temp(playerPos.x - cameraPos.x, 
    	playerPos.y - cameraPos.y, playerPos.z - cameraPos.z);
    mPhysLookDir = temp;
    mPhysLookDir.normalize();

    mBody->setLinearVelocity(mPhysLookDir * velocity);

    // Set the physics position of the Cat
    mTransform.setOrigin(mVector + mPhysLookDir * SPAWN_DISTANCE);
    mBody->setWorldTransform(mTransform);
}

//---------------------------------------------------------------------------
void Cat::initCatOgre(const char* meshName)
{
	Ogre::Entity* entity = mSceneMgr->createEntity(meshName);
    entity->setCastShadows(false);

    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(entity);
    mBody->setUserPointer(node);

    Ogre::Vector3 lookDir = Ogre::Vector3(mPhysLookDir.x(), mPhysLookDir.y(), mPhysLookDir.z());

    Ogre::Vector3 nodePos = Ogre::Vector3(mVector.x(), mVector.y(), mVector.z());
    nodePos += lookDir * SPAWN_DISTANCE;
    node->setPosition(nodePos);

    btQuaternion q = mTransform.getRotation();
    node->setOrientation(Ogre::Quaternion(q.w(), q.x(), q.y(), q.z()));

    node->scale(Ogre::Vector3(0.25, 0.25, 0.25));
}
