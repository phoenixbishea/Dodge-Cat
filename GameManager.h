#ifndef GameManager_h
#define GameManager_h
 
#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

//OIS Libraries
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

//CEGUI Libraries
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

class GameManager
  : public Ogre::WindowEventListener,
    public Ogre::FrameListener
{
public:
  GameManager();
  virtual ~GameManager();
 
  bool go();
 
private:
  CEGUI::OgreRenderer* mRenderer;
  virtual bool setup();
  virtual bool configure();
  virtual void chooseSceneManager();
  virtual void createCamera();
  virtual void createFrameListener();
  virtual void createScene();
  virtual void destroyScene();
  virtual void createViewports();
  virtual void setupResources();
  virtual void loadResources();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
 
  virtual void windowResized(Ogre::RenderWindow* rw);
  virtual void windowClosed(Ogre::RenderWindow* rw);
 
  // // OIS::KeyListener
  // virtual bool keyPressed( const OIS::KeyEvent &arg );
  // virtual bool keyReleased( const OIS::KeyEvent &arg );
  // // OIS::MouseListener
  // virtual bool mouseMoved( const OIS::MouseEvent &arg );
  // virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  // virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  // bool quit(const CEGUI::EventArgs &e);

  Ogre::Root* mRoot;
  Ogre::String mResourcesCfg;
  Ogre::String mPluginsCfg;
  Ogre::RenderWindow* mWindow;
  Ogre::SceneManager* mSceneMgr;
  Ogre::Camera* mCamera;
 
  OIS::InputManager* mInputMgr;
  OIS::Keyboard* mKeyboard;
  OIS::Mouse* mMouse;
 
};
 
#endif
