#ifndef NewGameManager_hpp
#define NewGameManager_hpp

#include "BulletPhysics.hpp"
#include "ExtendedCamera.hpp"
#include "Player.hpp"
#include "Wall.hpp"

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

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <string>
#include <iostream>
#include <cmath>

#define WALL_COLLIDE_ERROR 745
#define CAT_SPAWN_DISTANCE 80.0f
#define CAT_VELOCITY 1500

class NewGameManager
    : public Ogre::WindowEventListener,
    public Ogre::FrameListener,
    public OIS::KeyListener,
    public OIS::MouseListener
{
public:
    NewGameManager();
    ~NewGameManager();

    bool go();

private:
    bool initOgre();
    void initBullet();
    void initInput();
    void initListener(); 
    void initScene();
    void initSound();

    void initOgreResources();
    bool initOgreWindow();
    void initOgreViewports();

    void spawnCat();

    void windowResized(Ogre::RenderWindow* rw);
    void windowClosed(Ogre::RenderWindow* rw);

    bool keyPressed(const OIS::KeyEvent& ke);
    bool keyReleased(const OIS::KeyEvent& ke);

    bool mouseMoved(const OIS::MouseEvent& me);
    bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id);

    bool frameRenderingQueued(const Ogre::FrameEvent& fe);
    bool frameStarted(const Ogre::FrameEvent& fe);

    Ogre::Root* mRoot;
    Ogre::RenderWindow* mWindow;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
    ExtendedCamera* mExCamera;
    Player* mPlayer;

    BulletPhysics* mPhysicsEngine;

    OIS::InputManager* mInputMgr;
    OIS::Keyboard* mKeyboard;
    OIS::Mouse* mMouse;

    bool mShutDown;

    double mTimeSinceLastPhysicsStep;
    double mTimeSinceLastCat;

    Mix_Music* music;
    Mix_Chunk* effect1;
    Mix_Chunk* effect2;
    Mix_Chunk* effect3;
    std::vector<Mix_Chunk*> effects;
};

#endif