#include "ExtendedCamera.hpp"

ExtendedCamera::ExtendedCamera (Ogre::String name, Ogre::SceneManager *sceneMgr, Ogre::Camera *camera) 
{
    // Basic member references setup
    mName = name;
    mSceneMgr = sceneMgr;

    // Create the camera's node structure
    mCameraNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode (mName);
    mTargetNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode (mName + "_target");
    mCameraNode->setAutoTracking (true, mTargetNode); // The camera will always look at the camera target
    mCameraNode->setFixedYawAxis (true); // Needed because of auto tracking

    // Create our camera if it wasn't passed as a parameter
    if (camera == nullptr)
    {
        mCamera = mSceneMgr->createCamera (mName);
        mOwnCamera = true;
    }
    else
    {
        mCamera = camera;
        // just to make sure that mCamera is set to 'origin' (same position as the mCameraNode)
        mCamera->setPosition(0.0,0.0,0.0);
        mOwnCamera = false;
    }
    // ... and attach the Ogre camera to the camera node
    mCameraNode->attachObject (mCamera);
    mCameraNode->setPosition(Ogre::Vector3(0.0, 1000.0, 0.0));

    // Default tightness
    mTightness = 0.08f;
}
ExtendedCamera::~ExtendedCamera () 
{
    mCameraNode->detachAllObjects ();
    if (mOwnCamera)
        delete mCamera;
    mSceneMgr->destroySceneNode (mName);
    mSceneMgr->destroySceneNode (mName + "_target");
}

Ogre::Vector3 ExtendedCamera::getCameraPosition () 
{
    return mCameraNode->getPosition ();
}

void ExtendedCamera::update (Ogre::Real elapsedTime, Ogre::Vector3 cameraPosition, Ogre::Vector3 targetPosition) 
{
    // Handle movement
    Ogre::Vector3 displacement;

    displacement = (cameraPosition - mCameraNode->getPosition ()) * mTightness;
    mCameraNode->translate (displacement);

    displacement = (targetPosition - mTargetNode->getPosition ()) * mTightness;
    mTargetNode->translate (displacement);
}
