#ifndef PlayerPhysicsComponent_hpp
#define PlayerPhysicsComponent_hpp

class PlayerPhysicsComponent : public PhysicsComponent
{
public:
	PlayerPhysicsComponent(Player& obj, Physics& physics)
	{
		// Create player shape
		btBoxShape* playerShape = new btBoxShape(btVector3(40.0, 70.0, 40.0));

		// Init player ghost object
		ghostObject = new btPairCachingGhostObject();
		transform = ghostObject->getWorldTransform();
		transform.setOrigin(btVector3(0.0, 0.0, 0.0));
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
    	btBoxShape* paddleShape = new btBoxShape(btVector3(PADDLE_HEIGHT, PADDLE_HEIGHT, 2));
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

	virtual ~PlayerPhysicsComponent() {}
	virtual void update(Player& obj, BulletPhysics* physics, World& world)
	{
		// Move the player
		charController->setVelocityForTimeInterval(obj.velocity, world.elapsedTime * FPS);

		// Orient the player
        transform.setRotation(obj.orientation.toBullet());
        charController->getGhostObject()->setWorldTransform(transform);

        /// Orientation of paddle scaled by an offset
        Vector direction = obj.cannonOrientation * Vector(0, 0, -PADDLE_OFFSET);
        // Player position + cannon offset pointing in cannon's direction
        btVector3 paddleOrigin = transform.getOrigin() + direction.toBullet();
        // Ensures paddle does not go through the floor
        if (origin.y() < PADDLE_HEIGHT / 2.0f)
        {
        	origin.setY(PADDLE_HEIGHT / 2.0f);
        }
        // Update paddle transform
       	btTransform paddleTrans = transform;
       	paddleTrans.setOrigin(paddleOrigin);
       	paddleTrans.setRotation(obj.cannonOrientation.toBullet());
       	paddleBody->setWorldTransform(paddleTrans);

       	// Step character controller
       	charController->step(physics->getDynamicsWorld(), world.timeSinceLastFrame);
	}
private:
	const float PADDLE_OFFSET = 110.0f;
	const float PADDLE_HEIGHT = 350.0f;

	btKinematicCharacterController* charController;
	btTransform transform; // Initialize this to player's transform
	btPairCachingGhostObject* ghostObject;
	btRigidBody* paddleBody;
};

#endif
