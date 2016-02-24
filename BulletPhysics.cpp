#include "BulletPhysics.hpp"

BulletPhysics::BulletPhysics() {}

void BulletPhysics::initObjects()
{
  collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
                                              overlappingPairCache,
                                              solver,
                                              collisionConfiguration);
}

btDiscreteDynamicsWorld*
BulletPhysics::getDynamicsWorld()
{
  return this->dynamicsWorld;
}
