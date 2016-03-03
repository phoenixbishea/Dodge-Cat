#include "NewGameManager.hpp"

//---------------------------------------------------------------------------
NewGameManager::NewGameManager()
  : mRoot(0),
    mWindow(0),
    mSceneMgr(0),
    mCamera(0),
    mExCamera(0),

    mPhysicsEngine(0),

    mListener(0)
{
}

//---------------------------------------------------------------------------
NewGameManager::~NewGameManager()
{
  // Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, mListener);
  mListener->windowClosed(mWindow);
  delete mRoot;
}

//---------------------------------------------------------------------------
bool NewGameManager::go()
{
  if (!initOgre())
  {
    return false;
  }

  initBullet();

  initListener();

  // This starts the rendering loop
  // We don't need any special handling of the loop since we can
  // perform our per- tasks in the RenderingQueued() function
  mRoot->startRendering();

  return true;
}

//---------------------------------------------------------------------------
bool NewGameManager::initOgre()
{
  mRoot = new Ogre::Root("plugins.cfg");
  initOgreResources();

  if (!initOgreWindow())
  {
    return false;
  }

  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

  // Get the SceneManager, in this case a generic one
  mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

  // Create the camera
  mCamera = mSceneMgr->createCamera("MainCam");
  mExCamera = new ExtendedCamera("ExtendedCamera", mSceneMgr, mCamera);

  initOgreViewports();

  return true;
}

//---------------------------------------------------------------------------
void NewGameManager::initBullet()
{
    // Setup Bullet physics
    mPhysicsEngine = new BulletPhysics();
    mPhysicsEngine->initObjects();
    mPhysicsEngine->getDynamicsWorld()->setGravity(btVector3(0.0, -200.0, 0.0));
}

//---------------------------------------------------------------------------
void NewGameManager::initListener()
{
    mListener = new Listener(mWindow, mPhysicsEngine, mExCamera);
    mListener->initInput();
    // Set initial mouse clipping size
    mListener->windowResized(mWindow);
    // Register Listener as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, mListener);

    // Register a frame listnener to receive  events
    mRoot->addFrameListener(mListener);
}

//---------------------------------------------------------------------------
void NewGameManager::initOgreResources()
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

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

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//---------------------------------------------------------------------------
bool NewGameManager::initOgreWindow()
{
    // Show the configuration dialog and initialise the system.
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg.
    if (mRoot->restoreConfig() || mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initOgreialise.
        // Here we choose to let the system create a default rendering window by passing 'true'.
        mWindow = mRoot->initialise(true, "Render Window");
        return true;
    }
    else
    {
        return false;
    }
}

//---------------------------------------------------------------------------
void NewGameManager::initOgreViewports()
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) /
        Ogre::Real(vp->getActualHeight()));
}
