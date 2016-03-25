#ifndef PlayerCameraComponent_hpp
#define PlayerCameraComponent_hpp

class PlayerCameraComponent : public CameraComponent
{
public:
	PlayerCameraComponent(Ogre::SceneManager* graphics, Ogre::Camera* camera)
		: ogreCamera(camera)
	{
		cameraNode = graphics->getRootSceneNode()->createChildSceneNode("Camera");
		sightNode = graphics->getRootSceneNode()->createChildSceneNode("Sight Node");

		cameraNode->setAutoTracking(true, sightNode);
		cameraNode->setFixedYawAxis(true);

		cameraNode->attachObject(ogreCamera);
		cameraNode->setPosition(Ogre::Vector3(0.0, 1000.0, 0.0));
	}
	virtual ~PlayerCameraComponent() {}
	virtual void update(Player& obj)
	{
		Ogre::Vector3 sightPos = obj.sightPosition.toOgre();
		Ogre::Vector3 camPos = obj.cameraPosition.toOgre();

		Ogre::Vector3 displacement = TIGHTNESS * (sightPos - sightNode->getPosition());
		sightNode->translate(displacement);

		displacement = TIGHTNESS * (camPos - cameraNode->getPosition());
		cameraNode->translate(displacement);
	}
private:
	Ogre::SceneNode* sightNode;
	Ogre::SceneNode* cameraNode;
	Ogre::Camera* ogreCamera;

	const Ogre::Real TIGHTNESS = 0.08f;
};

#endif
