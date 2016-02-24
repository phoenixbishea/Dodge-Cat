#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <btBulletDynamicsCommon.h>

class Physics
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
  void Physics();
  void initObjects();
}

#endif // __PHYSICS_H__
