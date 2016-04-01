#ifndef PlayerPhysicsComponent_hpp
#define PlayerPhysicsComponent_hpp

#include "PlayerData.hpp"

class PlayerPhysicsComponent
{
public:
	btKinematicCharacterController* charController;

    PlayerPhysicsComponent(PlayerData& obj, BulletPhysics* physics, const Vector& initialPosition)
	{
		// Create player shape
		btBoxShape* playerShape = new btBoxShape(btVector3(50.0, 70.0, 50.0));

		// Init player ghost object
		ghostObject = new btPairCachingGhostObject();
		transform = ghostObject->getWorldTransform();
		transform.setOrigin(initialPosition.toBullet());
		ghostObject->setWorldTransform(transform);

		// Set the shape and collision object type
		ghostObject->setCollisionShape(playerShape);
		ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

		// Setup the character controller and add it to the physics world
		charController = new btKinematicCharacterController(ghostObject, playerShape, 1.0);
		physics->getDynamicsWorld()->addCollisionObject(
				ghostObject,
                btBroadphaseProxy::CharacterFilter,
                btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
		physics->getDynamicsWorld()->addAction(charController);

		// Setup the paddle transform
		btTransform paddleTrans = ghostObject->getWorldTransform();
		btVector3 paddlePos = paddleTrans.getOrigin() + btVector3(0, PADDLE_HEIGHT / 2, -150);
	    paddleTrans.setOrigin(paddlePos);

	    btQuaternion paddleRotation;
	    paddleTrans.setRotation(paddleRotation);

	    // Create paddle mass and inertia
	    btScalar paddleMass(0.0);
    	btVector3 localPaddleInertia(0, 0, 0);

    	// Create and set paddle shape and motion state
    	btBoxShape* paddleShape = new btBoxShape(btVector3(PADDLE_HEIGHT, PADDLE_HEIGHT, 5));
	    physics->getCollisionShapes().push_back(paddleShape);
	    btDefaultMotionState* paddleMotionState = new btDefaultMotionState(paddleTrans);

	    paddleShape->calculateLocalInertia(paddleMass, localPaddleInertia);

	    // Setup rigid body for paddle and add it to the physics world
	    btRigidBody::btRigidBodyConstructionInfo paddleRBInfo(
	    		paddleMass, 
	    		paddleMotionState, 
	    		paddleShape, 
	    		localPaddleInertia);
	    paddleBody = new btRigidBody(paddleRBInfo);
	    paddleBody->setRestitution(1.0);
	    physics->getDynamicsWorld()->addRigidBody(paddleBody);
	}

	~PlayerPhysicsComponent()
	{
	}
	bool update(PlayerData& obj, BulletPhysics* physics, float elapsedTime, bool networkedPlayer)
	{
      // Move the player and update positon
      charController->setVelocityForTimeInterval(obj.velocity.toBullet(), btScalar(elapsedTime * FPS));

      btVector3 position;
      // If there is a network component we should set the player's position with it here
      if (networkedPlayer)
      {
          position.setValue(obj.position.x(), obj.position.y(), obj.position.z());
      }
      else
      {
          position = charController->getGhostObject()->getWorldTransform().getOrigin();
      }
      transform.setOrigin(position);

      // Update player position for graphics update
      obj.position = Vector(transform.getOrigin());

      // Orient the player
      transform.setRotation(obj.orientation.toBullet());
      charController->getGhostObject()->setWorldTransform(transform);

      /// Orientation of paddle scaled by an offset
      Vector direction = obj.cannonOrientation * Vector(0.0f, 0.0f, -PADDLE_OFFSET);
      // Player position + cannon offset pointing in cannon's direction
      btVector3 paddleOrigin = transform.getOrigin() + direction.toBullet();
      // Ensures paddle does not go through the floor
      if (paddleOrigin.y() < PADDLE_HEIGHT / 2.0f)
      {
        	paddleOrigin.setY(PADDLE_HEIGHT / 2.0f);
      }
      // Update paddle transform
      btTransform paddleTrans = transform;
      paddleTrans.setOrigin(paddleOrigin);
      paddleTrans.setRotation(obj.cannonOrientation.toBullet());
      paddleBody->setWorldTransform(paddleTrans);

      // Step character controller
      charController->playerStep(physics->getDynamicsWorld(), elapsedTime);

      return handleCollisions(obj, physics);
	}
	btTransform getTransform()
	{
		return transform;
	}
private:
	const float PADDLE_OFFSET = 110.0f;
	const float PADDLE_HEIGHT = 100.0f;

	const float WALL_COLLIDE_ERROR = 745.0f;
	const float FPS = 60.0f;

	btTransform transform; // Initialize this to player's transform
	btPairCachingGhostObject* ghostObject;
	btRigidBody* paddleBody;

	bool handleCollisions(PlayerData& obj, BulletPhysics* physics)
	{

	    btManifoldArray manifoldArray;
	    btBroadphasePairArray& pairArray = ghostObject->getOverlappingPairCache()
	    		->getOverlappingPairArray();

	    for (int i = 0; i < pairArray.size(); i++)
	    {
	        manifoldArray.clear();

	        const btBroadphasePair& pair = pairArray[i];

	        btBroadphasePair* collisionPair = physics->getDynamicsWorld()
	        		->getPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);

	        if (!collisionPair) 
	        {
	            continue;
	        }

	        if (collisionPair->m_algorithm)
	        {
	            collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
	        }

	        for (int j = 0;j < manifoldArray.size(); j++)
	        {
	            btPersistentManifold* manifold = manifoldArray[j];

	            bool isFirstBody = manifold->getBody0() == ghostObject;

	            btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);

	            for (int p = 0; p < manifold->getNumContacts(); p++)
	            {
	                const btManifoldPoint& pt = manifold->getContactPoint(p);

	                if (pt.getDistance() < 0.f)
	                {
	                    const btVector3& ptA = pt.getPositionWorldOnA();
	                    const btVector3& ptB = pt.getPositionWorldOnB();
	                    const btVector3& normalOnB = pt.m_normalWorldOnB;

	                    // Exclude collisions with walls
	                    if (std::abs(ptA.x()) >= WALL_COLLIDE_ERROR || std::abs(ptB.x()) >= WALL_COLLIDE_ERROR)
	                    {
	                        continue;
	                    }

	                    if (std::abs(ptA.z()) >= WALL_COLLIDE_ERROR || std::abs(ptB.z()) >= WALL_COLLIDE_ERROR)
	                    {
	                        continue;
	                    }

	                    if (std::abs(ptA.y()) <= 0.0 || std::abs(ptB.y()) <= 0.0)
	                    {    
	                        continue;
	                    }

	                    return false;
	                }
	            }
	        }
	    }
	    return true;
	}
};

#endif
