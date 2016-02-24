#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <btBulletDynamicsCommon.h>

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
}

#endif // __PHYSICS_H__
