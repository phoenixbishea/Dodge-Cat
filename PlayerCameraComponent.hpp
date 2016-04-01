#ifndef PlayerCameraComponent_hpp
#define PlayerCameraComponent_hpp

#include "PlayerData.hpp"

class PlayerCameraComponent
{
public:
	PlayerCameraComponent(Ogre::SceneManager* graphics, Ogre::Camera* camera)
		: ogreCamera(camera),
		mSceneMgr(graphics)
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
		mSceneMgr->destroySceneNode(sightNode);
		mSceneMgr->destroySceneNode(cameraNode);
	}

	void update(PlayerData& obj)
	{
		Ogre::Vector3 sightPos = obj.derivedSight.toOgre();
		// printf("Camera sightPosition: %f, %f, %f\n", sightPos.x, sightPos.y, sightPos.z);
		Ogre::Vector3 camPos = obj.derivedCamera.toOgre();

		Ogre::Vector3 displacement = TIGHTNESS * (sightPos - sightNode->getPosition());
		sightNode->translate(displacement);
		displacement = TIGHTNESS * (camPos - cameraNode->getPosition());
		cameraNode->translate(displacement);
	}
private:
	Ogre::SceneNode* sightNode;
	Ogre::SceneNode* cameraNode;
	Ogre::Camera* ogreCamera;

	Ogre::SceneManager* mSceneMgr;

	const Ogre::Real TIGHTNESS = 0.08f;
};

#endif
