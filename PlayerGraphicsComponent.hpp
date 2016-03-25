#ifndef CameraGraphicsComponent_hpp
#define CameraGraphicsComponent_hpp

class CameraGraphicsComponent
{
public:
	CameraGraphicsComponent(Ogre::SceneManager* sceneMgr, Ogre::Camera* cam)
	: mCamera(cam)
	{
		mCameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode("Extended Camera");
		targetNode = sceneMgr->getRootSceneNode()->createChildSceneNode("Extended Camera Target");

		mCameraNode->setAutoTracking(true, targetNode);
		mCameraNode->setFixedYawAxis(true);

		mCameraNode->attachObject(mCamera);
		mCameraNode->setPosition(Ogre::Vector3(0.0, 1000.0, 0.0));
	}
	virtual ~CameraGraphicsComponent() {}
	virtual void update(Camera& obj, Graphics& graphics)
	{
		Ogre::Vector3 sightPos = obj.sightPosition.toOgre();
		Ogre::Vector3 camPos = obj.cameraPosition.toOgre();

		Ogre::Vector3 displacement;

		displacement = TIGHTNESS * (sightPos - mSightNode->getPosition());
		mSightNode->translate(displacement);

		displacement = TIGHTNESS * (camPos - mCameraNode->getPosition());
		mCameraNode->translate(displacement);
	}
private:
	Ogre::SceneNode* mSightNode;
	Ogre::SceneNode* mCameraNode;
	Ogre::Camera* mCamera;

	const Ogre::Real TIGHTNESS = 0.08f;
};

#endif
