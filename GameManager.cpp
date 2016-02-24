#include "GameManager.h"
 
#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreException.h>
 
GameManager::GameManager()
  : mRoot(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mWindow(0),
    mSceneMgr(0),
    mCamera(0),
    mInputMgr(0),
    mMouse(0),
    mKeyboard(0)
{
}
 
GameManager::~GameManager()
{
  // Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
 
  windowClosed(mWindow);
 
  delete mRoot;
}
 
bool GameManager::go()
{
/* Setting up resources */
#ifdef _DEBUG
  mResourcesCfg = "resources_d.cfg";
  mPluginsCfg = "plugins_d.cfg";
#else
  mResourcesCfg = "resources.cfg";
  mPluginsCfg = "plugins.cfg";
#endif
 
  mRoot = new Ogre::Root(mPluginsCfg);
 
  Ogre::ConfigFile cf;
  cf.load(mResourcesCfg);
 
  Ogre::String name, locType;
  Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();
 
  while (secIt.hasMoreElements())
  {
    Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator it;
 
    for (it = settings->begin(); it != settings->end(); ++it)
    {
      locType = it->first;
      name = it->second;
 
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
    }
  }
  
  /* Configure the RenderSystem */
  // Only show configuration dialogue if ogre.cfg is not present
  // OPTIONAL: Set up own render system or add a settings menu
  if (!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
    return false;
  
  // Bool determines whether Ogre create a RenderWindow for us
  mWindow = mRoot->initialise(true, "GameManager Render Window");
  
  /* Initialize Resources */
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  /* Create a SceneManager */
  mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
 
  /* Create the Camera */
  mCamera = mSceneMgr->createCamera("MainCam");
  mCamera->setPosition(0, 0, 80);
  mCamera->lookAt(0, 0, -300);
  mCamera->setNearClipDistance(5);
  
  /* Create the Viewport */
  Ogre::Viewport* vp = mWindow->addViewport(mCamera);
  vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
 
  mCamera->setAspectRatio(
    Ogre::Real(vp->getActualWidth()) /
    Ogre::Real(vp->getActualHeight()));
 
  /* Setup the Scene */
  Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
 
  Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
  ogreNode->attachObject(ogreEntity);
 
  mSceneMgr->setAmbientLight(Ogre::ColourValue(.5, .5, .5));
 
  Ogre::Light* light = mSceneMgr->createLight("MainLight");
  light->setPosition(20, 80, 50);
 
  /* Setup Object-Oriented Input System (OIS) */
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
 
  OIS::ParamList pl;
  size_t windowHandle = 0;
  std::ostringstream windowHandleStr;
 
  mWindow->getCustomAttribute("WINDOW", &windowHandle);
  windowHandleStr << windowHandle;
  pl.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));
 
  mInputMgr = OIS::InputManager::createInputSystem(pl);
  
  // Create Keyboard and Mouse to be used
  // OPTIONAL: Joystick
  // We pass false because we want the keyboard input unbuffered
  // TODO: Change to buffered inputs
  mKeyboard = static_cast<OIS::Keyboard*>(
    mInputMgr->createInputObject(OIS::OISKeyboard, false));
  mMouse = static_cast<OIS::Mouse*>(
    mInputMgr->createInputObject(OIS::OISMouse, false));
  
  // Set initial mouse clipping size
  windowResized(mWindow);
  // Register as a Window listener
  Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
  
  // Register root as a frame listnener so that it will receive frame events
  mRoot->addFrameListener(this);

  // This starts the rendering loop
  // We don't need any special handling of the loop since we can 
  // perform our per-frame tasks in the frameRenderingQueued() function
  mRoot->startRendering();
 
  return true;
}
 
bool GameManager::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
  if (mWindow->isClosed()) return false;
  
  // Capture/Update each input device
  mKeyboard->capture();
  mMouse->capture();
 
  if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;
 
  return true;
}

// Adjust mouse clipping area
void GameManager::windowResized(Ogre::RenderWindow* rw)
{
  int left, top;
  unsigned int width, height, depth;
 
  rw->getMetrics(width, height, depth, left, top);
 
  const OIS::MouseState& ms = mMouse->getMouseState();
  ms.width = width;
  ms.height = height;
}

// Unattach OIS before window shutdown
void GameManager::windowClosed(Ogre::RenderWindow* rw)
{
  // Only close for window that created OIS
  if(rw == mWindow)
  {
    if(mInputMgr)
    {
      mInputMgr->destroyInputObject(mMouse);
      mInputMgr->destroyInputObject(mKeyboard);
 
      OIS::InputManager::destroyInputSystem(mInputMgr);
      mInputMgr = 0;
    }
  }
}
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C"
{
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
  int main(int argc, char *argv[])
#endif
  {
    GameManager app;
 
    try
    {
      app.go();
    }
    catch(Ogre::Exception& e)
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      MessageBox(
	NULL,
	e.getFullDescription().c_str(),
	"An exception has occured!",
	MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      std::cerr << "An exception has occured: " <<
	e.getFullDescription().c_str() << std::endl;
#endif
    }
 
    return 0;
  }
#ifdef __cplusplus
}
#endif
