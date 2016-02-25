#include "GameManager.h"

#include <iostream>
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
    mShutDown(false),
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

  mCamera->setPosition(Ogre::Vector3(0, 300, 500));
  mCamera->lookAt(Ogre::Vector3(0, 0, 0));
  mCamera->setNearClipDistance(5);

  /* Tutorial 5 */
  // mCamera->setPosition(0, 0, 80);
  // // Look back along -Z
  // mCamera->lookAt(0, 0, -300);
  // mCamera->setNearClipDistance(5);
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
    mInputMgr->createInputObject(OIS::OISKeyboard, true));
  mMouse = static_cast<OIS::Mouse*>(
    mInputMgr->createInputObject(OIS::OISMouse, true));

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

/* Setups up the basic resources needed for CEGUI */
void GameManager::setupCEGUI()
{
  mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();


  //Sets up the default resource groups
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");


  //Creates a scheme file and sets the mouse arrow to a default image
 //CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
 //CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
  
}

//---------------------------------------------------------------------------
void GameManager::createScene()
{

  

  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
  music = Mix_LoadMUS("rules.mp3");
  Mix_PlayMusic(music, -1);

  setupCEGUI();


  mSceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));
  mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
 
  // Create ninja
  Ogre::Entity* ninjaEntity = mSceneMgr->createEntity("ninja.mesh");
  ninjaEntity->setCastShadows(true);
 
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ninjaEntity);
 
  // Create ground
  Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
 
  Ogre::MeshManager::getSingleton().createPlane(
    "ground",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
 
  Ogre::Entity* groundEntity = mSceneMgr->createEntity("ground");
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
 
  groundEntity->setMaterialName("Examples/Rockwall");
  groundEntity->setCastShadows(false);
 
  // Spotlight
  Ogre::Light* spotLight = mSceneMgr->createLight("Spotlight");
  spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
 
  spotLight->setDiffuseColour(Ogre::ColourValue(0, 0, 1));
  spotLight->setSpecularColour(Ogre::ColourValue(0, 0, 1));
 
  spotLight->setDirection(-1, -1, 0);
  spotLight->setPosition(Ogre::Vector3(200, 200, 0));
 
  spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
 
  // Directional light
  Ogre::Light* directionalLight = mSceneMgr->createLight("DirectionalLight");
  directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
 
  directionalLight->setDiffuseColour(Ogre::ColourValue(.4, 0, 0));
  directionalLight->setSpecularColour(Ogre::ColourValue(.4, 0, 0));
 
  directionalLight->setDirection(Ogre::Vector3(0, -1, 1));
 
  // Point light
  Ogre::Light* pointLight = mSceneMgr->createLight("PointLight");
  pointLight->setType(Ogre::Light::LT_POINT);
 
  pointLight->setDiffuseColour(.3, .3, .3);
  pointLight->setSpecularColour(.3, .3, .3);
 
  pointLight->setPosition(Ogre::Vector3(0, 150, 250));

  Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
  "CamNode1", Ogre::Vector3(0, 300, 500));
  mCamNode = node;
  node->attachObject(mCamera);

  CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
  CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

  // CEGUI::Window *quit = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
  // quit->setText("Quit");
  //quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));

  // sheet->addChildWindow(quit);
  // CEGUI::System::getSingleton().setGUISheet(sheet);



  /* Tutorial 5 */
  // mSceneMgr->setAmbientLight(Ogre::ColourValue(.2, .2, .2));
 
  // Ogre::Entity* tudorEntity = mSceneMgr->createEntity("tudorhouse.mesh");
  // Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
  //   "Node");
  // node->attachObject(tudorEntity);
 
  // Ogre::Light* light = mSceneMgr->createLight("Light1");
  // light->setType(Ogre::Light::LT_POINT);
  // light->setPosition(Ogre::Vector3(250, 150, 250));
  // light->setDiffuseColour(Ogre::ColourValue::White);
  // light->setSpecularColour(Ogre::ColourValue::White);
 
  // node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
  // "CamNode1", Ogre::Vector3(1200, -370, 0));
  // node->yaw(Ogre::Degree(90));

  // mCamNode = node;
  // node->attachObject(mCamera);

  // node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
  // "CamNode2", Ogre::Vector3(-500, -370, 1000));
  // node->yaw(Ogre::Degree(-30));
}

//---------------------------------------------------------------------------
void GameManager::destroyScene()
{
  Mix_FreeMusic(music);
  Mix_CloseAudio();
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

  if (mShutDown) return false;
  
  // Capture/Update each input device
  mKeyboard->capture();
  mMouse->capture();
 
  mCamNode->translate(mDirection * fe.timeSinceLastFrame, Ogre::Node::TS_LOCAL);

  //Need to inject timestaps to CEGUI System
  
  //CEGUI::System::getSingleton().injectTimePulse(fe.timeSinceLastFrame);
  // if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;
 
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
  //Injects the current key pressed to CEGUI
  CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  context.injectKeyDown((CEGUI::Key::Scan)ke.key);
  context.injectChar((CEGUI::Key::Scan)ke.text);


  switch (ke.key)
  {
  case OIS::KC_ESCAPE: 
    mShutDown = true;
    break;

  case OIS::KC_1:
    // mCamera->getParentSceneNode()->detachObject(mCamera);
    // mCamNode = mSceneMgr->getSceneNode("CamNode1");
    // mCamNode->attachObject(mCamera);
    break;

  case OIS::KC_2:
    // mCamera->getParentSceneNode()->detachObject(mCamera);
    // mCamNode = mSceneMgr->getSceneNode("CamNode2");
    // mCamNode->attachObject(mCamera);
    break;
  
  case OIS::KC_UP:
  case OIS::KC_W:
    mDirection.z = -mMove;
    break;

  case OIS::KC_DOWN:
  case OIS::KC_S:
    mDirection.z = mMove;
    break;

  case OIS::KC_LEFT:
  case OIS::KC_A:
    mDirection.x = -mMove;
    break;

  case OIS::KC_RIGHT:
  case OIS::KC_D:
    mDirection.x = mMove;
    break;

  case OIS::KC_PGDOWN:
  case OIS::KC_E:
    mDirection.y = -mMove;
    break;

  case OIS::KC_PGUP:
  case OIS::KC_Q:
    mDirection.y = mMove;
    break;
  default:
    break;
  }
  return true; 
}

//---------------------------------------------------------------------------
bool GameManager::keyReleased(const OIS::KeyEvent& ke) 
{ 

 CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)ke.key);

  switch (ke.key)
  {
  case OIS::KC_UP:
  case OIS::KC_W:
      mDirection.z = 0;
      break;
   
  case OIS::KC_DOWN:
  case OIS::KC_S:
      mDirection.z = 0;
      break;
   
  case OIS::KC_LEFT:
  case OIS::KC_A:
      mDirection.x = 0;
      break;
   
  case OIS::KC_RIGHT:
  case OIS::KC_D:
      mDirection.x = 0;
      break;
   
  case OIS::KC_PGDOWN:
  case OIS::KC_E:
      mDirection.y = 0;
      break;
   
  case OIS::KC_PGUP:
  case OIS::KC_Q:
      mDirection.y = 0;
      break;
   
  default:
      break;
  }
  return true;
}

/* Used to covert OIS Mouse Into CEGUI Mouse Buttons */
CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
  switch (buttonID)
  {
    case OIS::MB_Left:
      return CEGUI::LeftButton;
    case OIS::MB_Right:
      return CEGUI::RightButton;
    case OIS::MB_Middle:
      return CEGUI::MiddleButton;

    default:
      return CEGUI::LeftButton;
  }
}


//---------------------------------------------------------------------------
bool GameManager::mouseMoved(const OIS::MouseEvent& me) 
{ 

  CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  context.injectMouseMove(me.state.X.rel, me.state.Y.rel);
  // Scroll wheel.
  if (me.state.Z.rel)
    context.injectMouseWheelChange(me.state.Z.rel / 120.0f);

  if (me.state.buttonDown(OIS::MB_Right))
  {
    mCamNode->yaw(Ogre::Degree(-mRotate * me.state.X.rel), Ogre::Node::TS_WORLD);
    mCamNode->pitch(Ogre::Degree(-mRotate * me.state.Y.rel), Ogre::Node::TS_LOCAL);
  }
  return true; 
}

//---------------------------------------------------------------------------
bool GameManager::mousePressed(
  const OIS::MouseEvent& me, OIS::MouseButtonID id) 
{ 

  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertButton(id));
  // if (id == OIS::MB_Left)
  // {
  //   Ogre::Light* light2 = mSceneMgr->getLight("Light1");
  //   light2->setVisible(!light2->isVisible());
  // }
  return true; 
}

//---------------------------------------------------------------------------
bool GameManager::mouseReleased(
  const OIS::MouseEvent& me, OIS::MouseButtonID id) 
{ 
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertButton(id));
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
