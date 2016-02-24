#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <btBulletDynamicsCommon.h>

/**
 * Wrapper for Bullet physics engine that needs to be included in the
 * main game application as a member variable, and subsequently initialized
 * by calling `initObjects()`.
 */
class BulletPhysics
{
private:
  btDefaultCollisionConfiguration* collisionConfiguration;
  btCollisionDispatcher* dispatcher;
  btBroadphaseInterface* overlappingPairCache;
  btSequentialImpulseConstraintSolver* solver;
  btDiscreteDynamicsWorld* dynamicsWorld;
  std::vector<btCollisionShape *> collisionShape;
  std::map<std::string, btRigidBody *> physicsAccessors;
public:
  void BulletPhysics();
  void initObjects();
  btDiscreteDynamicsWorld* getDynamicsWorld();
}

#endif // __PHYSICS_H__
