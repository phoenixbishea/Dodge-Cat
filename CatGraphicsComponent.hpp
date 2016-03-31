#ifndef CatGraphicsComponent_hpp
#define CatGraphicsComponent_hpp

class CatGraphicsComponent : public GraphicsComponent
{
public:
	CatGraphicsComponent(Ogre::SceneManager* graphics)
	{
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

	    Ogre::Vector3 lookDir = Ogre::Vector3(obj.direction.toOgre());
	    Ogre::Vector3 nodePos = Ogre::Vector3(obj.vector.toOgre());
	    Ogre::Vector3 up(0, 1, 0);

	    Ogre::Vector3 cannonOffset = lookDir.crossProduct(up);

	    nodePos += lookDir * SPAWN_DISTANCE + cannonOffset * 55;
	    node->setPosition(nodePos);

	    // Set the PHYSICS position of the Cat
	    obj.transform.setOrigin(obj.vector.toBullet() + obj.direction.toBullet() * SPAWN_DISTANCE 
	        + btVector3(cannonOffset.x, cannonOffset.y, cannonOffset.z) * 55);
	    obj->body->setWorldTransform(obj.transform);

	    // More PHYSICS
	    btQuaternion q = obj.transform.getRotation();

	    // Set cat rotation
	    node->setOrientation(Ogre::Quaternion(q.w(), q.x(), q.y(), q.z()));

	    // Scale and rotate the cat
	    catNode->scale(Ogre::Vector3(CAT_SCALE, CAT_SCALE, CAT_SCALE));
	    catNode->yaw(Ogre::Radian(Ogre::Degree(180)));
	    catNode->pitch(Ogre::Radian(Ogre::Degree(90)));
	}
	virtual ~CatGraphicsComponent() {}
	virtual void update(Cat& obj)
	{
		obj.sceneNode->setPosition(Ogre::Vector3(trans.getOrigin().getX(),
                        trans.getOrigin().getY(),
                        trans.getOrigin().getZ()));

        obj.sceneNode->setOrientation(Ogre::Quaternion(orientation.getW(),
            orientation.getX(),
            orientation.getY(),
            orientation.getZ()));
	}
private:
	static bool meshCreated = false;

	const Ogre::Real CAT_SCALE = 100.0f;
	const float SPAWN_DISTANCE = 150.0f;
};

#endif
