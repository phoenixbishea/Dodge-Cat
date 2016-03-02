#ifndef ExtendedCamera_hpp
#define ExtendedCamera_hpp

#include <OgreSceneManager.h>

class ExtendedCamera 
{
public:
    ExtendedCamera (Ogre::String name, Ogre::SceneManager *sceneMgr, Ogre::Camera *camera = 0);
    ~ExtendedCamera ();

    Ogre::Vector3 getCameraPosition ();

    void update (Ogre::Real elapsedTime, Ogre::Vector3 cameraPosition, Ogre::Vector3 targetPosition);

protected:
    Ogre::SceneNode *mTargetNode; // The camera target
    Ogre::SceneNode *mCameraNode; // The camera itself
    Ogre::Camera *mCamera; // Ogre camera

    Ogre::SceneManager *mSceneMgr;
    Ogre::String mName;

    bool mOwnCamera; // To know if the ogre camera binded has been created outside or inside of this class

    Ogre::Real mTightness; // Determines the movement of the camera - 1 means tight movement, while 0 means no movement
};

#endif
