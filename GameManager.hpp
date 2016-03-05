#ifndef GameManager_h
#define GameManager_h

#include "Player.hpp"
#include "ExtendedCamera.hpp"

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
#include <OgreStringConverter.h>

/* Input OIS */
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

/* Bullet Physics */
#include "BulletPhysics.hpp"

/* Sound */
#include <SDL.h>
#include <SDL_mixer.h>

/* CEGUI */
// #include "GUIManager.hpp"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

enum gameState {mainState = 0, playState = 1, endState = 2};


class GameManager
  : public Ogre::WindowEventListener,
    public Ogre::FrameListener,
    public OIS::KeyListener,
    public OIS::MouseListener
{
public:
  GameManager();
  virtual ~GameManager();

  bool go();

  void setCharacter (Player* character);

  void setExtendedCamera (ExtendedCamera *cam);

private:
  virtual bool setup();
  virtual bool configure();
  virtual void chooseSceneManager();
  virtual void createCamera();
  virtual void createFrameListener();
  virtual void setupSound();
  virtual void createScene();
  virtual void destroyScene();
  virtual void createViewports();
  virtual void setupResources();
  virtual void loadResources();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
  virtual bool frameStarted(const Ogre::FrameEvent& fe);

  virtual void windowResized(Ogre::RenderWindow* rw);
  virtual void windowClosed(Ogre::RenderWindow* rw);

  // For buffered input tutorial
  // Key listener callbacks
  virtual bool keyPressed(const OIS::KeyEvent& ke);
  virtual bool keyReleased(const OIS::KeyEvent& ke);
  // Mouse listener callbacks
  virtual bool mouseMoved(const OIS::MouseEvent& me);
  virtual bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id);


  virtual void setupGUI();
  virtual bool quit(const CEGUI::EventArgs&);
  virtual bool start(const CEGUI::EventArgs&);
  // virtual bool reStart(const CEGUI::EventArgs&);


  void createWalls();
  void spawnCat();

  Ogre::Root* mRoot;
  Ogre::String mResourcesCfg;
  Ogre::String mPluginsCfg;
  Ogre::RenderWindow* mWindow;
  Ogre::SceneManager* mSceneMgr;
  Ogre::Camera* mCamera;

  bool mShutDown;
  OIS::InputManager* mInputMgr;
  OIS::Keyboard* mKeyboard;
  OIS::Mouse* mMouse;

  BulletPhysics* physicsEngine;

  Player* mChar;
  ExtendedCamera* mExCamera;

  int mScore;

  double timeSinceLastPhysicsStep;
  double timeSinceLastCat;

  Mix_Music* music;
  Mix_Chunk* effect1;
  Mix_Chunk* effect2;
  Mix_Chunk* effect3;
  std::vector<Mix_Chunk*> effects;
  Mix_Chunk* scoreUp;
  Mix_Chunk* spray;

  // GUIManager* mGUI;
  gameState mState;
  CEGUI::OgreRenderer* mRenderer;
  std::vector<CEGUI::Window*> sheets;
  std::vector<CEGUI::Window*> startButtons;
  std::vector<CEGUI::Window*> gameOverButtons;
  std::vector<CEGUI::Window*> playButtons;


};

#endif
