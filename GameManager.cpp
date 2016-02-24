#include "GameManager.h"

#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreMeshManager.h>

#include <string>
#include <iostream>

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
    physicsEngine(0)
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

bool GameManager::setup()
{
  physicsEngine = new BulletPhysics();
  physicsEngine->initObjects();

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

void GameManager::chooseSceneManager()
{
  // Get the SceneManager, in this case a generic one
  mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}

void GameManager::createCamera()
{
  // Create the camera
  mCamera = mSceneMgr->createCamera("MainCam");
  mCamera->setPosition(1000, 1000, 1000);
  mCamera->lookAt(0, 0, 0);
  mCamera->setNearClipDistance(5);
}

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

  // Set initial mouse clipping size
  windowResized(mWindow);
  // Register as a Window listener
  Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
  // Register root as a frame listnener so that it will receive frame events
  mRoot->addFrameListener(this);
}

void GameManager::createScene()
{
  // Add ambient light
  mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));
  mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

  // Add a point light
  Ogre::Light* light = mSceneMgr->createLight("MainLight");
  light->setDiffuseColour(1.0, 1.0, 1.0);
  light->setSpecularColour(1.0, 1.0, 1.0);
  light->setDirection(Ogre::Vector3(0.0, -1.0, 0.0));
  light->setType(Ogre::Light::LT_DIRECTIONAL);

  //create the actual plane in Ogre3D
  Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton()
    .createPlane("ground",
                 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                 plane,
                 1500, 1500,
                 20, 20,
                 true,
                 1, 5, 5,
                 Ogre::Vector3::UNIT_Z);

  Ogre::Entity *entGround = mSceneMgr->createEntity("GroundEntity", "ground");
  entGround->setCastShadows(false);
  entGround->setMaterialName("Examples/Rockwall");
  Ogre::SceneNode *groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("groundNode");
  groundNode->attachObject(entGround);
  groundNode->setPosition(Ogre::Vector3(0.0, 0.0, 0.0));

  // create the plane entity to the physics engine, and attach it to the node
  btTransform groundTransform;
  groundTransform.setIdentity();
  groundTransform.setOrigin(btVector3(0, 0, 0));

  btScalar groundMass(0.0); // the mass is 0, because the ground is immovable (static)
  btVector3 localGroundInertia(0, 0, 0);

  btCollisionShape *groundShape = new btStaticPlaneShape(btVector3(0.0, 1.0, 0.0), 0.0);
  btDefaultMotionState *groundMotionState = new btDefaultMotionState(groundTransform);

  groundShape->calculateLocalInertia(groundMass, localGroundInertia);

  btRigidBody::btRigidBodyConstructionInfo groundRBInfo(groundMass, groundMotionState, groundShape, localGroundInertia);
  btRigidBody *groundBody = new btRigidBody(groundRBInfo);

  //add the body to the dynamics world
  this->physicsEngine->getDynamicsWorld()->addRigidBody(groundBody);


  std::string physicsCubeName = "PhysicsCube";
  btVector3 initialPosition(0.0, 1000.0, 0.0);

  Ogre::Entity *entity = this->mSceneMgr->createEntity("models/cube.mesh");

  Ogre::SceneNode *newNode = this->mSceneMgr->getRootSceneNode()->createChildSceneNode(physicsCubeName);
  newNode->attachObject(entity);
  newNode->setPosition(Ogre::Vector3(initialPosition.getX(), initialPosition.getY(), initialPosition.getZ()));
  newNode->setOrientation(Ogre::Quaternion(1.0, 1.0, 1.0, 0.0));

  //create the new shape, and tell the physics that is a Box
  btCollisionShape *newRigidShape = new btBoxShape(btVector3(50.0f, 50.0f, 50.0f));
  this->physicsEngine->getCollisionShapes().push_back(newRigidShape);

  //set the initial position and transform. For this demo, we set the tranform to be none
  btTransform startTransform;
  startTransform.setIdentity();
  startTransform.setRotation(btQuaternion(1.0f, 1.0f, 1.0f, 0));

  //set the mass of the object. a mass of "0" means that it is an immovable object
  btScalar mass = 0.1f;
  btVector3 localInertia(0,0,0);

  startTransform.setOrigin(initialPosition);
  newRigidShape->calculateLocalInertia(mass, localInertia);

  //actually contruvc the body and add it to the dynamics world
  btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);

  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, newRigidShape, localInertia);
  btRigidBody *body = new btRigidBody(rbInfo);
  body->setRestitution(1);
  body->setUserPointer(newNode);

  physicsEngine->getDynamicsWorld()->addRigidBody(body);
  physicsEngine->trackRigidBodyWithName(body, physicsCubeName);
}

void GameManager::destroyScene()
{
}

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

void GameManager::loadResources()
{
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

bool GameManager::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
  if (mWindow->isClosed()) return false;

  // Capture/Update each input device
  mKeyboard->capture();
  mMouse->capture();

  if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
    return false;

  return true;
}

bool GameManager::frameStarted(const Ogre::FrameEvent& fe)
{
  if (this->physicsEngine != NULL){
		physicsEngine->getDynamicsWorld()->stepSimulation(1.0f / 120.0f); //suppose you have 60 frames per second

		for (int i = 0; i < this->physicsEngine->getCollisionObjectCount(); i++) {
			btCollisionObject* obj = this->physicsEngine->getDynamicsWorld()->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);

			if (body && body->getMotionState()) {
				btTransform trans;
				body->getMotionState()->getWorldTransform(trans);

				void *userPointer = body->getUserPointer();
				if (userPointer) {
					btQuaternion orientation = trans.getRotation();
					Ogre::SceneNode *sceneNode = static_cast<Ogre::SceneNode *>(userPointer);
					sceneNode->setPosition(Ogre::Vector3(trans.getOrigin().getX(),
                                               trans.getOrigin().getY(),
                                               trans.getOrigin().getZ()));
					sceneNode->setOrientation(Ogre::Quaternion(orientation.getW(),
                                                     orientation.getX(),
                                                     orientation.getY(),
                                                     orientation.getZ()));
          std::cout << sceneNode->getPosition() << std::endl;
				}
			}
		}
	}
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
