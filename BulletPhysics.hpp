#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <btBulletDynamicsCommon.h>
#include <vector>
#include <string>
#include <map>

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
  BulletPhysics();
  void initObjects();
  btDiscreteDynamicsWorld* getDynamicsWorld();
};

#endif // __PHYSICS_H__
