#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <vector>
#include <string>
#include <map>
#include <iostream>

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
  std::vector<btCollisionShape *>& getCollisionShapes();
  void trackRigidBodyWithName(btRigidBody* body, std::string& name);
  void trackRigidBodyWithName(btRigidBody* body, std::string&& name);
  size_t getCollisionObjectCount();
};

std::ostream& operator << (std::ostream& out, const btVector3& vec);

#endif // __PHYSICS_H__
