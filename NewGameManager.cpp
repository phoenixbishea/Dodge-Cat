#include "GameManager.hpp"

//---------------------------------------------------------------------------
GameManager::GameManager()
  : mRoot(0),
    mWindow(0),
    mSceneMgr(0),
    mCamera(0),
    mExCamera(0),
    mPlayer(0),

    mPhysicsEngine(0),

    mInputMgr(0),
    mMouse(0),
    mKeyboard(0),

    mSound(0),

    mShutDown(false),
    mScore(0),

    mTimeSinceLastPhysicsStep(0),
    mTimeSinceLastCat(0),

    mState(MAIN_MENU),
    mRenderer(0),

    mCats(0)
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
    if (!initOgre())
    {
        return false;
    }

    initBullet();

    initInput();
    initListener();

    // initScene();

    mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
    initGUI();

    // mSound = new Sound();
    // mSound->initSound();

    // This starts the rendering loop
    // We don't need any special handling of the loop since we can
    // perform our per- tasks in the RenderingQueued() function
    mRoot->startRendering();

    return true;
}

//---------------------------------------------------------------------------
bool GameManager::initOgre()
{
  mRoot = new Ogre::Root("plugins.cfg");
  initOgreResources();

  if (!initOgreWindow())
  {
    return false;
  }

  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

  // Get the SceneManager, in this case a generic one
  mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

  // Create the camera
  mCamera = mSceneMgr->createCamera("MainCam");

  initOgreViewports();

  return true;
}

//---------------------------------------------------------------------------
void GameManager::initBullet()
{
    // Setup Bullet physics
    mPhysicsEngine = new BulletPhysics();
    mPhysicsEngine->initObjects();
    mPhysicsEngine->getDynamicsWorld()->setGravity(btVector3(0.0, -200.0, 0.0));
}

//---------------------------------------------------------------------------
void GameManager::initInput()
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
    mKeyboard = static_cast<OIS::Keyboard*>(
    mInputMgr->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>(
    mInputMgr->createInputObject(OIS::OISMouse, true));

    // Register GameManager as source of CALLBACK methods
    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
}

//---------------------------------------------------------------------------
void GameManager::initScene()
{
    // Add ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    mPlayer = new Player(mSceneMgr, mPhysicsEngine, mCamera);
    mCats = new Cat();

    // Add a point light
    Ogre::Light* light = mSceneMgr->createLight("MainLight");
    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setDirection(Ogre::Vector3(0.0, -1.0, 0.0));
    light->setType(Ogre::Light::LT_DIRECTIONAL);

    std::vector<Wall*> walls;
    for (int i = 0; i < 6; ++i)
    {
        walls.push_back(new Wall(mPhysicsEngine, mSceneMgr));
    }

    // Ground
    walls[0]->createWall("ground", 0.0f, 0.0f, 0.0f, 1500.0f, 1500.0f, 
        Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Z);
    walls[0]->createGroundPhysics(0.0f, 0.0f, 0.0f);

    // Left Wall
    walls[1]->createWall("left wall", -750.0f, 3000.0f, 0.0f, 6000.0f, 1500.0f, 
        Ogre::Vector3::UNIT_X, Ogre::Vector3::UNIT_Z);
    walls[1]->createWallPhysics(-750.0f, 3000.0f, 0.0f, 5.0f, 6000.0f, 1500.0f);

    // Right Wall
    walls[2]->createWall("right wall", 750.0f, 3000.0f, 0.0f, 6000.0f, 1500.0f, 
        Ogre::Vector3::NEGATIVE_UNIT_X, Ogre::Vector3::UNIT_Z);
    walls[2]->createWallPhysics(750.0f, 3000.0f, 0.0f, 5.0f, 6000.0f, 1500.0f);

    // Front Wall
    walls[3]->createWall("front wall", 0.0f, 3000.0f, -750.0f, 6000.0f, 1500.0f, 
        Ogre::Vector3::UNIT_Z, Ogre::Vector3::UNIT_X);
    walls[3]->createWallPhysics(0.0f, 3000.0f, -750.0f, 1500.0f, 6000.0f, 5.0f);

    // Back Wall
    walls[4]->createWall("back wall", 0.0f, 3000.0f, 750.0f, 6000.0f, 1500.0f, 
        Ogre::Vector3::NEGATIVE_UNIT_Z, Ogre::Vector3::UNIT_X);
    walls[4]->createWallPhysics(0.0f, 3000.0f, 750.0f, 1500.0f, 6000.0f, 5.0f);

    // Ceiling
    walls[5]->createWall("ceiling", 0.0f, 6000.0f, 0.0f, 1500.0f, 1500.0f, 
        Ogre::Vector3::NEGATIVE_UNIT_Y, Ogre::Vector3::UNIT_X);
    walls[5]->createWallPhysics(0.0f, 6000.0f, 0.0f, 1500.0f, 5.0f, 1500.0f);
}

//---------------------------------------------------------------------------
void GameManager::initListener()
{
    // Set initial mouse clipping size
    windowResized(mWindow);
    // Register Listener as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
    // Register a frame listnener to receive  events
    mRoot->addFrameListener(this);
}

//---------------------------------------------------------------------------
void GameManager::initGUI()
{
    // Sets up the default resource groups
    CEGUI::ImageManager::setImagesetDefaultResourceGroup("General");
    CEGUI::Font::setDefaultResourceGroup("General");
    CEGUI::Scheme::setDefaultResourceGroup("General");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("General");
    CEGUI::WindowManager::setDefaultResourceGroup("General");
    CEGUI::ScriptModule::setDefaultResourceGroup("General");

    CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
    CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window* mainSheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
    CEGUI::Window* quitSheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
    CEGUI::Window* playSheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

    //Create the main menu
    CEGUI::Window* start = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/StartButton");
    CEGUI::Window* quitMain = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
    CEGUI::Window* title = wmgr.createWindow("TaharezLook/Label", "CEGUIDemo/MainTitle");

    CEGUI::Window* scoreBoard = wmgr.createWindow("TaharezLook/StaticText", "CEGUIDemo/scoreBoard"); 

    start->setText("Start");
    start->setSize(CEGUI::USize(CEGUI::UDim(0.15,0), CEGUI::UDim(0.05,0)));
    start->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4f,0),CEGUI::UDim(0.4f,0)));

    quitMain->setText("Quit");
    quitMain->setSize(CEGUI::USize(CEGUI::UDim(0.15,0), CEGUI::UDim(0.05,0)));
    quitMain->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4f,0),CEGUI::UDim(0.4f,100)));

    title->setText("Dodge Cat");
    title->setSize(CEGUI::USize(CEGUI::UDim(0.30,0), CEGUI::UDim(0.10,0)));
    title->setPosition(CEGUI::UVector2(CEGUI::UDim(0.33f,0),CEGUI::UDim(0.18f,0)));

    scoreBoard->setText("Score: 0");
    scoreBoard->setSize(CEGUI::USize(CEGUI::UDim(0.15,0), CEGUI::UDim(0.05,0)));
    scoreBoard->setPosition(CEGUI::UVector2(CEGUI::UDim(0.05f,0),CEGUI::UDim(0.05f,0)));

    quitMain->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameManager::quit, this));

    start->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameManager::start, this));

    startButtons.push_back(start);
    startButtons.push_back(quitMain);

    mPlayButtons.push_back(scoreBoard);

    mainSheet->addChild(start);
    mainSheet->addChild(quitMain);
    mainSheet->addChild(title);


    playSheet->addChild(scoreBoard);

    sheets.push_back(mainSheet);
    sheets.push_back(quitSheet);
    sheets.push_back(playSheet);
}

//---------------------------------------------------------------------------
void GameManager::initOgreResources()
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
}

//---------------------------------------------------------------------------
bool GameManager::initOgreWindow()
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
void GameManager::initOgreViewports()
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
void GameManager::spawnCat()
{
    Cat cat(mPhysicsEngine, mSceneMgr, mPlayer);
    cat.initCatPhysics();
    cat.setVelocity();
    cat.initCatOgre();
}

// ---------------------Adjust mouse clipping area---------------------------
void GameManager::windowResized(Ogre::RenderWindow* rw)
{
    int left, top;
    unsigned int width, height, depth;

    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState& ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//-----------------Unattach OIS before window shutdown-----------------------
void GameManager::windowClosed(Ogre::RenderWindow* rw)
{
    // Only close for window that created OIS
    if (rw == mWindow)
    {
        if (mInputMgr)
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
    // CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    // context.injectKeyDown((CEGUI::Key::Scan)ke.key);
    // context.injectChar((CEGUI::Key::Scan)ke.text);

    if (ke.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
    else if (ke.key == OIS::KC_M)
    {
        mSound->muteUnmuteMusic();
    }
    else if (ke.key == OIS::KC_N)
    {
        mSound->muteUnmuteEffects();
    }

    return true;
}

//---------------------------------------------------------------------------
bool GameManager::keyReleased(const OIS::KeyEvent& ke)
{
    // CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)ke.key);
    return true;
}

//---------------------------------------------------------------------------
bool GameManager::mouseMoved(const OIS::MouseEvent& me)
{
    if (mState != PLAY) 
    {
        CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
        context.injectMouseMove(me.state.X.rel, me.state.Y.rel); 
    }  
    return true;
}

//---------------------------------------------------------------------------
bool GameManager::mousePressed(
    const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertButton(id));
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
bool GameManager::quit(const CEGUI::EventArgs&)
{
    mShutDown = true;
}

//---------------------------------------------------------------------------
bool GameManager::start(const CEGUI::EventArgs&)
{
    mState = PLAY;

    mSound = new Sound();
    mSound->initSound();

    initScene();

    CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheets.at(2));
    CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
}

//---------------------------------------------------------------------------
bool GameManager::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
    if (mWindow->isClosed())
    {
        return false;
    }

    if (mShutDown)
    {
        return false;
    }

    // Capture/Update each input device
    mKeyboard->capture();
    mMouse->capture();

    if (mState == PLAY)
    {
        mTimeSinceLastCat += fe.timeSinceLastFrame;
        if (mTimeSinceLastCat > 1.0)
        {
            spawnCat();
            ++mScore;
            mPlayButtons.at(0)->setText("Score: " + Ogre::StringConverter::toString(mScore));

            mTimeSinceLastCat -= 1.0;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
bool GameManager::frameStarted(const Ogre::FrameEvent& fe)
{
    if (mState == MAIN_MENU) 
    {
        CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheets.at(0));
        return true;
    }
    else
    {
        mTimeSinceLastPhysicsStep += fe.timeSinceLastFrame;
        if (mTimeSinceLastPhysicsStep > 1.0 / 60.0)
        {
            mTimeSinceLastPhysicsStep -= 1.0 / 60.0;
        }
        else
        {
            return true;
        }

        mPlayer->update(mPhysicsEngine, mKeyboard, mMouse, fe.timeSinceLastFrame);
        mCats->update(mPhysicsEngine, mSound);
    }
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
