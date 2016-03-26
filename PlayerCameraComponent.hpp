#ifndef PlayerCameraComponent_hpp
#define PlayerCameraComponent_hpp

#include "PlayerData.hpp"

class PlayerCameraComponent
{
public:
	PlayerCameraComponent(Ogre::SceneManager* graphics, Ogre::Camera* camera)
		: ogreCamera(camera)
	{
		ogreCamera->setPosition(0.0,0.0,0.0);

		cameraNode = graphics->getRootSceneNode()->createChildSceneNode("Camera");
		sightNode = graphics->getRootSceneNode()->createChildSceneNode("Sight Node");

		cameraNode->setAutoTracking(true, sightNode);
		cameraNode->setFixedYawAxis(true);

		cameraNode->attachObject(ogreCamera);
		cameraNode->setPosition(Ogre::Vector3(0.0, 1000.0, 0.0));
	}
	~PlayerCameraComponent()
	{
	}
	void update(PlayerData& obj)
	{
		printf("Updating camera, sightPos %f, %f, %f\n", obj.sightPosition.x(), obj.sightPosition.y(), obj.sightPosition.z());
		Ogre::Vector3 sightPos = obj.sightPosition.toOgre();
		printf("Ogre sightPos %f, %f, %f\n", sightPos.x, sightPos.y, sightPos.z);
		Ogre::Vector3 camPos = obj.cameraPosition.toOgre();

		Ogre::Vector3 displacement = TIGHTNESS * (sightPos - sightNode->getPosition());
		sightNode->translate(displacement);
		printf("Ogre translated sightPos %f, %f, %f\n", sightPos.x, sightPos.y, sightPos.z);
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
