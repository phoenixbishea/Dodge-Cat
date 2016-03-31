#ifndef Cat_hpp
#define Cat_hpp

#include "BulletPhysics.hpp"
#include "Player.hpp"
#include "Sound.hpp"
#include "Vector.hpp"

#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreMeshManager.h>

class Cat
{
public:
    Cat();
    Cat(BulletPhysics*, Ogre::SceneManager*, Player* player);

    void update(BulletPhysics* physics, Sound* sound);

    void initCatPhysics();
    void setVelocity();
    void initCatOgre();

private:
	BulletPhysics* mPhysicsEngine;
	Ogre::SceneManager* mSceneMgr;
	Player* mPlayer;

	btRigidBody* mBody;

	btVector3 mPhysLookDir;
	btVector3 mVector;
	btTransform mTransform;

    const float CAT_SPEED = 1500.0f;
    const float CAT_MASS = 10.0f;
    const float SPHERE_SIZE = 20.0f;

    const Ogre::Real CAT_SCALE = 100.0f;
    const float SPAWN_DISTANCE = 150.0f;
};

#endif

//---------------------------------------------------------------------------
Cat::Cat()
{
}

//---------------------------------------------------------------------------
Cat::Cat(BulletPhysics* physics, Ogre::SceneManager* scnMgr, Player* player)
	: mPhysicsEngine(physics),
	mSceneMgr(scnMgr),
	mPlayer(player),
	mBody(0)
{
}

//---------------------------------------------------------------------------
void Cat::update(BulletPhysics* physics, Sound* sound)
{
    for (int i = 0; i < physics->getCollisionObjectCount(); i++)
    {
        // Get object from collision array and cast to rigidbody
        btCollisionObject* obj = physics->getDynamicsWorld()->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);

        // Update physics and graphics for non-player objects (cats)
        if (body && body->getMotionState() && obj->getCollisionFlags() != btCollisionObject::CF_CHARACTER_OBJECT)
        {
            btTransform trans;
            body->getMotionState()->getWorldTransform(trans);
            void *userPointer = body->getUserPointer();

            // Play cat sound on collision
            sound->playSound("meow");

            // Convert rigidbody to OGRE scenenode and update position and orientation
            if (userPointer)
            {
                btQuaternion orientation = trans.getRotation();
                Ogre::SceneNode* sceneNode = static_cast<Ogre::SceneNode *>(userPointer);

                sceneNode->setPosition(Ogre::Vector3(trans.getOrigin().getX(),
                    trans.getOrigin().getY(),
                    trans.getOrigin().getZ()));

                sceneNode->setOrientation(Ogre::Quaternion(orientation.getW(),
                    orientation.getX(),
                    orientation.getY(),
                    orientation.getZ()));
            }
        }
    }
}

//---------------------------------------------------------------------------
void Cat::initCatPhysics()
{
	mTransform = mPlayer->getWorldTransform();
    mVector = mTransform.getOrigin();

    btScalar mass(CAT_MASS);
    btVector3 localInertia(0, 0, 0);

    btCollisionShape* shape = new btSphereShape(SPHERE_SIZE);
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
    mPhysLookDir = mPlayer->getCannonDirection().toBullet();
    mPhysLookDir.normalize();

    mBody->setLinearVelocity(mPhysLookDir * CAT_SPEED);
}

//---------------------------------------------------------------------------
void Cat::initCatOgre()
{   
    static bool meshCreated = false;
    if (!meshCreated)
    {
      Ogre::MeshManager::getSingleton().create("Cat.mesh",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      meshCreated = true;
    }

	Ogre::Entity* entity = mSceneMgr->createEntity(Ogre::MeshManager::getSingleton()
        .getByName("Cat.mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
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
