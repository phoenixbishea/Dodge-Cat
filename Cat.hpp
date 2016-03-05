#ifndef Cat_hpp
#define Cat_hpp

#include "BulletPhysics.hpp"
#include "Player.hpp"

#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreMeshManager.h>

#define SPAWN_DISTANCE 150.0f
#define CAT_SPEED 2000

class Cat
{
public:
    Cat(BulletPhysics*, Ogre::SceneManager*, Player* player);

    void initCatPhysics(const float catMass, const float sphereSize);
    void setVelocity();
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
void Cat::setVelocity()
{
	 // Set the velocity of the Cat based on sight and camera nodes attached to the player
    Ogre::Vector3 direction = mPlayer->getOgreLookDirection();

    btVector3 temp(direction.x, direction.y, direction.z);
    mPhysLookDir = temp;
    mPhysLookDir.normalize();

    mBody->setLinearVelocity(mPhysLookDir * CAT_SPEED);
}

//---------------------------------------------------------------------------
void Cat::initCatOgre(const char* meshName)
{
    static bool meshCreated = false;
    if (!meshCreated)
    {
      Ogre::MeshManager::getSingleton().create(meshName,
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      meshCreated = true;
    }

	Ogre::Entity* entity = mSceneMgr->createEntity(Ogre::MeshManager::getSingleton()
        .getByName(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
    entity->setCastShadows(false);

    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::SceneNode* catNode = node->createChildSceneNode();
    catNode->attachObject(entity);
    mBody->setUserPointer(node);

    Ogre::Vector3 lookDir = Ogre::Vector3(mPhysLookDir.x(), mPhysLookDir.y(), mPhysLookDir.z());
    Ogre::Vector3 nodePos = Ogre::Vector3(mVector.x(), mVector.y(), mVector.z());
    Ogre::Vector3 up(0, 1, 0);

    Ogre::Vector3 cannonOffset = lookDir.crossProduct(up);

    nodePos += lookDir * SPAWN_DISTANCE + cannonOffset * 55;
    node->setPosition(nodePos);

    // Set the physics position of the Cat
    mTransform.setOrigin(mVector + mPhysLookDir * SPAWN_DISTANCE 
        + btVector3(cannonOffset.x, cannonOffset.y, cannonOffset.z) * 55);
    mBody->setWorldTransform(mTransform);

    btQuaternion q = mTransform.getRotation();
    node->setOrientation(Ogre::Quaternion(q.w(), q.x(), q.y(), q.z()));

    Ogre::Real catScale = 100.0;
    catNode->scale(Ogre::Vector3(catScale, catScale, catScale));
    catNode->yaw(Ogre::Radian(Ogre::Degree(180)));
    catNode->pitch(Ogre::Radian(Ogre::Degree(90)));
}
