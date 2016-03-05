#ifndef Wall_hpp
#define Wall_hpp

#include "BulletPhysics.hpp"

#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreMeshManager.h>

#include <string>

class Wall
{
public:
    Wall(BulletPhysics*, Ogre::SceneManager*);

    void createWall(std::string, const float, const float, const float, const float, const float, 
    	Ogre::Vector3, Ogre::Vector3);

    void createWallPhysics(const float, const float, const float, const float, 
    	const float, const float);

    void createGroundPhysics(const float, const float, const float);

private:
	BulletPhysics* mPhysicsEngine;
	Ogre::SceneManager* mSceneMgr;
};

//---------------------------------------------------------------------------
Wall::Wall(BulletPhysics* physics, Ogre::SceneManager* scnMgr)
	: mPhysicsEngine(physics),
	mSceneMgr(scnMgr)
{	
}

//---------------------------------------------------------------------------
void Wall::createWall(std::string str, const float x, const float y, const float z, 
	const float height, const float width, Ogre::Vector3 textureDir,
	Ogre::Vector3 normal)
{
	Ogre::Plane plane(textureDir, 0); ////////////////////////////////////////////
    Ogre::MeshManager::getSingleton()
        .createPlane(str,  ////////////////////////////////////////////
                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                     plane, ////////////////////////////////////////////
                     height, width, 
                     20, 20,
                     true,
                     1, 5, 5,
                     normal); ////////////////////////////////////////////

    Ogre::Entity* wallEntity = mSceneMgr->createEntity(str);
    wallEntity->setCastShadows(false);
    wallEntity->setMaterialName("Examples/Rockwall");

    Ogre::SceneNode* wallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    wallNode->attachObject(wallEntity);
    wallNode->setPosition(Ogre::Vector3(x, y, z)); ////////////////////////////////////////////
}

//---------------------------------------------------------------------------
void Wall::createWallPhysics(const float x, const float y, const float z, 
	const float length, const float height, const float depth)
{
	// create the plane entity to the physics engine, and attach it to the node
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(x, y, z)); ////////////////////////////////////////////

    btScalar mass(0.0); // the mass is 0, because the LeftWall is immovable (static)
    btVector3 localInertia(0, 0, 0);

    btCollisionShape* shape = new btBoxShape(btVector3(length, height, depth));
    btDefaultMotionState* motionState = new btDefaultMotionState(transform); ////////////////////////////////////////////

    shape->calculateLocalInertia(mass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rigidBodyInfo);
    body->setRestitution(0.9);

    //add the body to the dynamics world
    this->mPhysicsEngine->getDynamicsWorld()->addRigidBody(body);
}

//---------------------------------------------------------------------------
void Wall::createGroundPhysics(const float x, const float y, const float z)
{
	// create the plane entity to the physics engine, and attach it to the node
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(x, y, z)); ////////////////////////////////////////////

    btScalar mass(0.0); // the mass is 0, because the LeftWall is immovable (static)
    btVector3 localInertia(0, 0, 0);

    btCollisionShape* shape = new btStaticPlaneShape(btVector3(0.0, 1.0, 0.0), 0.0);
    btDefaultMotionState* motionState = new btDefaultMotionState(transform); ////////////////////////////////////////////

    shape->calculateLocalInertia(mass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rigidBodyInfo);
    body->setRestitution(0.9);

    //add the body to the dynamics world
    this->mPhysicsEngine->getDynamicsWorld()->addRigidBody(body);
}

#endif
// 		Ground	Left 	Right	Front	Back	Ceiling

// x		0		-750	750		0		0		0
// y		0		3000	3000	3000	3000	6000
// z		0		0		0		-750	750		0

// height	1500	6000	6000	6000	6000	1500
// width	1500	1500	1500	1500	1500	1500

// tex 	y		x		negx	z		z		y
// norm 	z		z		z		x		x		z

// length 			0.1 	0.1 	1500	1500	1500
// height  		6000	6000	6000	6000	0.1
// depth			1500	1500	0.1 	0.1 	1500
