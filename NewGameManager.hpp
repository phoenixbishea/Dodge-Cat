#ifndef NewGameManager_hpp
#define NewGameManager_hpp

#include "BulletPhysics.hpp"
#include "ExtendedCamera.hpp"
#include "Listener.hpp"

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>

#include <OgreEntity.h>
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreMeshManager.h>

class NewGameManager
{
public:
    NewGameManager();
    ~NewGameManager();

    bool go();

private:
    bool initOgre();
    void initBullet();
    void initListener();

    void initOgreResources();
    bool initOgreWindow();
    void initOgreViewports();

    Ogre::Root* mRoot;
    Ogre::RenderWindow* mWindow;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
    ExtendedCamera* mExCamera;

    BulletPhysics* mPhysicsEngine;

    Listener* mListener;
};

#endif
