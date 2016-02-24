#ifndef GameManager_h
#define GameManager_h
 
#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
 
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
 
class GameManager
  : public Ogre::WindowEventListener,
    public Ogre::FrameListener
{
public:
  GameManager();
  virtual ~GameManager();
 
  bool go();
 
private:
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

  // For buffered input tutorial
  // Key listener callbacks
  virtual bool keyPressed(const OIS::KeyEvent& ke);
  virtual bool keyReleased(const OIS::KeyEvent& ke);
  // Mouse listener callbacks
  virtual bool mouseMoved(const OIS::MouseEvent& me);
  virtual bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id);
 
  Ogre::Root* mRoot;
  Ogre::String mResourcesCfg;
  Ogre::String mPluginsCfg;
  Ogre::RenderWindow* mWindow;
  Ogre::SceneManager* mSceneMgr;
  Ogre::Camera* mCamera;
 
  OIS::InputManager* mInputMgr;
  OIS::Keyboard* mKeyboard;
  OIS::Mouse* mMouse;

  // For buffered input tutorial
  Ogre::Real mRotate;
  Ogre::Real mMove;
  Ogre::SceneNode* mCamNode;
  Ogre::Vector3 mDirection;
 
};
 
#endif
