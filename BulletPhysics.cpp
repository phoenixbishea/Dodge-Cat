#include "BulletPhysics.hpp"
#include <stdexcept>

std::ostream& operator << (std::ostream& out, const btVector3& vec)
{
  out << "(" << vec.x() << ", " << vec.y() << ", " << vec.z() << ")";
}

BulletPhysics::BulletPhysics() {}

void BulletPhysics::initObjects()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
                                                overlappingPairCache,
                                                solver,
                                                collisionConfiguration);
}

btDiscreteDynamicsWorld* BulletPhysics::getDynamicsWorld()
{
    return this->dynamicsWorld;
}

std::vector<btCollisionShape *>& BulletPhysics::getCollisionShapes()
{
    return this->collisionShape;
}

void BulletPhysics::trackRigidBodyWithName(btRigidBody* body, std::string& name)
{
    if (this->physicsAccessors.find(name) != this->physicsAccessors.end())
    {
        throw std::invalid_argument("BulletPhysics::trackRigidBodyWithName() : Cannot track two objects with the same name.");
    }

    this->physicsAccessors[name] = body;
}

void BulletPhysics::trackRigidBodyWithName(btRigidBody* body, std::string&& name)
{
    if (this->physicsAccessors.find(name) != this->physicsAccessors.end())
    {
        throw std::invalid_argument("BulletPhysics::trackRigidBodyWithName() : Cannot track two objects with the same name.");
    }

    this->physicsAccessors[name] = body;
}

size_t BulletPhysics::getCollisionObjectCount()
{
    return this->dynamicsWorld->getNumCollisionObjects();
}
