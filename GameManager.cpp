#include "GameManager.h"
 
#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreException.h>

//---------------------------------------------------------------------------
GameManager::GameManager()
  : mRoot(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mWindow(0),
    mSceneMgr(0),
    mCamera(0),
    mInputMgr(0),
    mMouse(0),
    mKeyboard(0),
    // For buffered input tutorial
    mRotate(.13),
    mMove(250),
    mCamNode(0),
    mDirection(Ogre::Vector3::ZERO)
{
}

//---------------------------------------------------------------------------
GameManager::~GameManager()
{
  // Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
 
  windowClosed(mWindow);
 
  delete mRoot;
}

//---------------------------------------------------------------------------
bool GameManager::go()
{
#ifdef _DEBUG
  mResourcesCfg = "resources_d.cfg";
  mPluginsCfg = "plugins_d.cfg";
#else
  mResourcesCfg = "resources.cfg";
  mPluginsCfg = "plugins.cfg";
#endif
  
  if (!setup()) return false;

  // This starts the rendering loop
  // We don't need any special handling of the loop since we can 
  // perform our per-frame tasks in the frameRenderingQueued() function
  mRoot->startRendering();
  
  // Clean up
  destroyScene();

  return true;
}

//---------------------------------------------------------------------------
bool GameManager::setup()
{
  mRoot = new Ogre::Root(mPluginsCfg);

  setupResources();

  // Attempt to configure render system
  bool carryOn = configure();
  if (!carryOn) return false;

  // Load resources
  loadResources();
  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

  chooseSceneManager();
  createCamera();
  createViewports();

  // Create the scene
  createScene();

  createFrameListener();

  return true;
}

//---------------------------------------------------------------------------
bool GameManager::configure()
{
  // Show the configuration dialog and initialise the system.
  // You can skip this and use root.restoreConfig() to load configuration
  // settings if you were sure there are valid ones saved in ogre.cfg.
  if (mRoot->restoreConfig() || mRoot->showConfigDialog())
  {
    // If returned true, user clicked OK so initialise.
    // Here we choose to let the system create a default rendering window by passing 'true'.
    mWindow = mRoot->initialise(true, "GameManager Render Window");
    return true;
  }
  else
  {
    return false;
  }
}

//---------------------------------------------------------------------------
void GameManager::chooseSceneManager()
{
  // Get the SceneManager, in this case a generic one
  mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}

//---------------------------------------------------------------------------
void GameManager::createCamera()
{
  // Create the camera
  mCamera = mSceneMgr->createCamera("MainCam");
  mCamera->setPosition(0, 0, 80);
  // Look back along -Z
  mCamera->lookAt(0, 0, -300);
  mCamera->setNearClipDistance(5);
}

//---------------------------------------------------------------------------
void GameManager::createFrameListener()
{
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

  // Register GameManager as source of callback methods
  mMouse->setEventCallback(this);
  mKeyboard->setEventCallback(this);
  
  // Set initial mouse clipping size
  windowResized(mWindow);
  // Register as a Window listener
  Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
  // Register root as a frame listnener so that it will receive frame events
  mRoot->addFrameListener(this);
}

//---------------------------------------------------------------------------
void GameManager::createScene()
{
  mSceneMgr->setAmbientLight(Ogre::ColourValue(.2, .2, .2));
 
  Ogre::Entity* tudorEntity = mSceneMgr->createEntity("tudorhouse.mesh");
  Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
    "Node");
  node->attachObject(tudorEntity);
 
  Ogre::Light* light = mSceneMgr->createLight("Light1");
  light->setType(Ogre::Light::LT_POINT);
  light->setPosition(Ogre::Vector3(250, 150, 250));
  light->setDiffuseColour(Ogre::ColourValue::White);
  light->setSpecularColour(Ogre::ColourValue::White);
 
  node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
  "CamNode1", Ogre::Vector3(1200, -370, 0));
  node->yaw(Ogre::Degree(90));

  mCamNode = node;
  node->attachObject(mCamera);

  node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
  "CamNode2", Ogre::Vector3(-500, -370, 1000));
  node->yaw(Ogre::Degree(-30));
}

//---------------------------------------------------------------------------
void GameManager::destroyScene()
{
}

//---------------------------------------------------------------------------
void GameManager::createViewports()
{
  // Create one viewport, entire window
  Ogre::Viewport* vp = mWindow->addViewport(mCamera);
  vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
  
  // Alter the camera aspect ratio to match the viewport
  mCamera->setAspectRatio(
    Ogre::Real(vp->getActualWidth()) /
    Ogre::Real(vp->getActualHeight()));
}

//---------------------------------------------------------------------------
void GameManager::setupResources()
{
  // Load resource paths from config file
  Ogre::ConfigFile cf;
  cf.load(mResourcesCfg);

  // Go through all sections & settings in the file
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
}

//---------------------------------------------------------------------------
void GameManager::loadResources()
{
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//---------------------------------------------------------------------------
bool GameManager::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
  if (mWindow->isClosed()) return false;
  
  // Capture/Update each input device
  mKeyboard->capture();
  mMouse->capture();
 
  if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;
 
  return true;
}

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
bool GameManager::keyPressed(const OIS::KeyEvent& ke) 
{ 
  switch (ke.key)
  {
  case OIS::KC_ESCAPE: 
      mShutDown = true;
      break;
  default:
      break;
  }
  return true; 
}

//---------------------------------------------------------------------------
bool GameManager::keyReleased(const OIS::KeyEvent& ke) 
{ 
  return true; 
}

//---------------------------------------------------------------------------
bool GameManager::mouseMoved(const OIS::MouseEvent& me) 
{ 
  return true; 
}

//---------------------------------------------------------------------------
bool GameManager::mousePressed(
  const OIS::MouseEvent& me, OIS::MouseButtonID id) 
{ 
  return true; 
}

//---------------------------------------------------------------------------
bool GameManager::mouseReleased(
  const OIS::MouseEvent& me, OIS::MouseButtonID id) 
{ 
  return true; 
}

//---------------------------------------------------------------------------
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
