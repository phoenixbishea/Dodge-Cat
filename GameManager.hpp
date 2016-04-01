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

#include "NetManager.h"

#define CATS_ON_SCREEN 30

enum GameState {MAIN_MENU = 0, PLAY = 1, NETWORK = 2, LOADING = 3, CLIENT = 4};

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
    bool initServer();

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
    void startScene();
    bool start(const CEGUI::EventArgs&);
    bool mpSheet(const CEGUI::EventArgs&);
    bool handleText(const CEGUI::EventArgs&);
    bool setupServer(const CEGUI::EventArgs&);
    bool connectServer(const CEGUI::EventArgs&);
    bool back(const CEGUI::EventArgs&);

    void menuChange();

    bool frameRenderingQueued(const Ogre::FrameEvent& fe);

    bool frameStartedClient(const Ogre::FrameEvent& fe);
    bool frameStartedServer(const Ogre::FrameEvent& fe);
    bool frameStarted(const Ogre::FrameEvent& fe);
    bool frameEnded(const Ogre::FrameEvent& fe);

    Ogre::Root* mRoot;
    Ogre::RenderWindow* mWindow;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
    Player* mPlayer;
    Player* mPlayer2;

    BulletPhysics* mPhysicsEngine;

    OIS::InputManager* mInputMgr;
    OIS::Keyboard* mKeyboard;
    OIS::Mouse* mMouse;

    Sound* mSound;

    bool mShutDown;
    int mScore;

    double mTimeSinceLastPhysicsStep;
    double mTimeSinceLastCat;

    Cat* mCatSim;
    Cat* mCats[CATS_ON_SCREEN];
    int mCatIndex;

    GameState mState;
    CEGUI::OgreRenderer* mRenderer;
    std::vector<CEGUI::Window*> sheets;
    std::vector<CEGUI::Window*> startButtons;
    std::vector<CEGUI::Window*> multiplayerButtons;
    std::vector<CEGUI::Window*> mPlayButtons;
    std::vector<CEGUI::Window*> loadingButtons;

    /*Network */
    NetManager mNetManager;
    bool connected;
    int playerNumber;
};

#endif
