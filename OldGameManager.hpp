#ifndef GameManager_hpp
#define GameManager_hpp

#include "BulletPhysics.hpp"
#include "Cat.hpp"
#include "ExtendedCamera.hpp"
#include "Player.hpp"
#include "Sound.hpp"
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

#include <vector>

#include <string>
#include <iostream>
#include <cmath>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

#define WALL_COLLIDE_ERROR 745

enum GameState {MAIN_MENU = 0, PLAY = 1};

CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);

class GameManager
    : public Ogre::WindowEventListener,
    public Ogre::FrameListener,
    public OIS::KeyListener,
    public OIS::MouseListener
{
public:
    GameManager();
    ~GameManager();

    bool go();

private:
    bool initOgre();
    void initBullet();
    void initInput();
    void initScene();
    void initListener(); 
    
    void initGUI();

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

    bool quit(const CEGUI::EventArgs&);
    bool start(const CEGUI::EventArgs&);

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

    Sound* mSound;

    bool mShutDown;
    int mScore;

    double mTimeSinceLastPhysicsStep;
    double mTimeSinceLastCat;

    GameState mState;
    CEGUI::OgreRenderer* mRenderer;
    std::vector<CEGUI::Window*> sheets;
    std::vector<CEGUI::Window*> startButtons;
    std::vector<CEGUI::Window*> gameOverButtons;
    std::vector<CEGUI::Window*> mPlayButtons;
};

#endif
