#ifndef CatPhysicsComponent_hpp
#define CatPhysicsComponent_hpp

class CatPhysicsComponent : public PhysicsComponent
{
public:
	CatPhysicsComponent(Cat& obj, BulletPhysics* physics, Ogre::SceneManager* graphics, Player* player)
	{
		btTransform transform = player->getWorldTransform();
	    Vector vector = Vector(transform.getOrigin());

	    btScalar mass(CAT_MASS);
	    btVector3 localInertia(0, 0, 0);

	    btCollisionShape* shape = new btSphereShape(SPHERE_SIZE);
	    physics->getCollisionShapes().push_back(shape);
	    btDefaultMotionState* motionState = new btDefaultMotionState(transform);

	    shape->calculateLocalInertia(mass, localInertia);

	    btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass, motionState, 
	    	shape, localInertia);
	    obj->body = new btRigidBody(rigidBodyInfo);
	    physics->getDynamicsWorld()->addRigidBody(body);

	    obj->body->setRestitution(1);

	    // Set the velocity of the Cat based on sight and camera nodes attached to the player
	    btVector3 btDirection = player->getCannonDirection().toBullet();
	    btDirection.normalize();

	    obj->body->setLinearVelocity(btDirection * CAT_SPEED);
	    Vector direction = Vector(direction);

	    if (!meshCreated)
	    {
	      Ogre::MeshManager::getSingleton().create("Cat.mesh",
	                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	      meshCreated = true;
	    }

	    // Assign cat mesh to entity
		Ogre::Entity* entity = graphics->createEntity(Ogre::MeshManager::getSingleton()
	        .getByName("Cat.mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
	    entity->setCastShadows(false);

	    // Create node and graphics representation of cat
	    Ogre::SceneNode* node = graphics->getRootSceneNode()->createChildSceneNode();
	    Ogre::SceneNode* catNode = node->createChildSceneNode();
	    catNode->attachObject(entity);
	    obj->body->setUserPointer(node); ///// PHYSICS

	    Ogre::Vector3 lookDir = Ogre::Vector3(direction.toOgre());
	    Ogre::Vector3 nodePos = Ogre::Vector3(vector.toOgre());
	    Ogre::Vector3 up(0, 1, 0);

	    Ogre::Vector3 cannonOffset = lookDir.crossProduct(up);

	    nodePos += lookDir * SPAWN_DISTANCE + cannonOffset * 55;
	    node->setPosition(nodePos);

	    // Set the PHYSICS position of the Cat
	    transform.setOrigin(vector.toBullet() + direction.toBullet() * SPAWN_DISTANCE 
	        + btVector3(cannonOffset.x, cannonOffset.y, cannonOffset.z) * 55);
	    obj->body->setWorldTransform(transform);

	    // More PHYSICS
	    btQuaternion q = transform.getRotation();

	    // Set cat rotation
	    node->setOrientation(Ogre::Quaternion(q.w(), q.x(), q.y(), q.z()));

	    // Scale and rotate the cat
	    catNode->scale(Ogre::Vector3(CAT_SCALE, CAT_SCALE, CAT_SCALE));
	    catNode->yaw(Ogre::Radian(Ogre::Degree(180)));
	    catNode->pitch(Ogre::Radian(Ogre::Degree(90)));
	}

	virtual ~CatPhysicsComponent() 
	{
	}

	virtual void update(Cat& obj, BulletPhysics* physics, World& world)
	{
		for (int i = 0; i < physics->getCollisionObjectCount(); i++)
        {
            // Get object from collision array and cast to rigidbody
            btCollisionObject* obj = physics->getDynamicsWorld()->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);

            // Update physics and graphics for non-player objects (cats)
            if (body && body->getMotionState() && obj->getCollisionFlags() != btCollisionObject::CF_CHARACTER_OBJECT)
            {
                btTransform trans;
                body->getMotionState()->getWorldTransform(trans);
                void *userPointer = body->getUserPointer();

                // Play cat sound on collision
                mSound->playSound("meow");

                // Convert rigidbody to OGRE scenenode and update position and orientation
                if (userPointer)
                {
                    btQuaternion orientation = trans.getRotation();
                    Ogre::SceneNode* sceneNode = static_cast<Ogre::SceneNode *>(userPointer);

                    sceneNode->setPosition(Ogre::Vector3(trans.getOrigin().getX(),
                        trans.getOrigin().getY(),
                        trans.getOrigin().getZ()));

                    sceneNode->setOrientation(Ogre::Quaternion(orientation.getW(),
                        orientation.getX(),
                        orientation.getY(),
                        orientation.getZ()));
                }
            }
        }
	}
private:
	const float CAT_SPEED = 2000.0f;
	const float CAT_MASS = 10.0f;
	const float SPHERE_SIZE = 20.0f;

	static bool meshCreated = false;

	const Ogre::Real CAT_SCALE = 100.0f;
	const float SPAWN_DISTANCE = 150.0f;
};

#endif
