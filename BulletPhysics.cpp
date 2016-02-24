#include "BulletPhysics.hpp"

void BulletPhysics::Physics() {}

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
