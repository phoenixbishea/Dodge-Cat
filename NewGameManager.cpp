#include "NewGameManager.hpp"

//---------------------------------------------------------------------------
NewGameManager::NewGameManager()
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

    mShutDown(false),

    mTimeSinceLastPhysicsStep(0),
    mTimeSinceLastCat(0)
{
}

//---------------------------------------------------------------------------
NewGameManager::~NewGameManager()
{
  // Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
  windowClosed(mWindow);
  delete mRoot;
}

//---------------------------------------------------------------------------
bool NewGameManager::go()
{
    std::cout << "Go called\n";
    if (!initOgre())
    {
        return false;
    }
    std::cout << "Ogre init\n";

    initBullet();
    std::cout << "Bullet init\n";

    initInput();
    std::cout << "Input init\n";
    initListener();
    std::cout << "Listener init\n";

    initScene();
    std::cout << "Scene init\n";

    initSound();

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
  std::cout << "Successful init\n";

  if (!initOgreWindow())
  {
    return false;
  }

  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  std::cout << "Successful init 1\n";

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
void NewGameManager::initInput()
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

    // Register NewGameManager as source of CALLBACK methods
    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
}

//---------------------------------------------------------------------------
void NewGameManager::initScene()
{
    // Add ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    mPlayer = new Player("Player 1", mSceneMgr, this->mPhysicsEngine);

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

void NewGameManager::initSound()
{
  srand (time(NULL));
  Mix_Chunk* effectTemp;

  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
  music = Mix_LoadMUS("The-Power-I-Feel.mp3");
  effect1 = Mix_LoadWAV("angryMeow.wav");
  Mix_VolumeChunk(effect1, MIX_MAX_VOLUME* 0.1);
  effects.push_back(effect1);
  effect2 = Mix_LoadWAV("cat.wav");
  Mix_VolumeChunk(effect2, MIX_MAX_VOLUME* 0.1);
  effects.push_back(effect2);
  effect3 = Mix_LoadWAV("happyPurr.wav");
  Mix_VolumeChunk(effect3, MIX_MAX_VOLUME* 0.1);
  effects.push_back(effect3);


  Mix_VolumeMusic(MIX_MAX_VOLUME * 0.3);
   Mix_PlayMusic(music, -1);
}

//---------------------------------------------------------------------------
void NewGameManager::initListener()
{
    // Set initial mouse clipping size
    windowResized(mWindow);
    // Register Listener as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
    // Register a frame listnener to receive  events
    mRoot->addFrameListener(this);
}

//---------------------------------------------------------------------------
void NewGameManager::initOgreResources()
{
    std::cout << "initOgreResources called\n";
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");
    std::cout << "initOgreResources called 1\n";
    // Go through all sections & settings in the file
    Ogre::String name, locType;
    Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();
    std::cout << "initOgreResources called 2\n";
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
    std::cout << "initOgreResources called 3\n";
    std::cout << "initOgreResources called 4\n";
}

//---------------------------------------------------------------------------
bool NewGameManager::initOgreWindow()
{
    std::cout << "initOgreWindow called\n";
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
    std::cout << "initOgreViewports called\n";
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) /
        Ogre::Real(vp->getActualHeight()));
}

//--------------------------Needs Refactoring--------------------------------
void NewGameManager::spawnCat()
{
    // create the plane entity to the physics engine, and attach it to the node
    btTransform CatTransform = mPlayer->getWorldTransform();
    btVector3 vec = CatTransform.getOrigin();

    btScalar CatMass(10.0);
    btVector3 localCatInertia(0, 0, 0);

    btCollisionShape *CatShape = new btSphereShape(20.0);
    this->mPhysicsEngine->getCollisionShapes().push_back(CatShape);
    btDefaultMotionState *CatMotionState = new btDefaultMotionState(CatTransform);

    CatShape->calculateLocalInertia(CatMass, localCatInertia);

    btRigidBody::btRigidBodyConstructionInfo CatRBInfo(CatMass, CatMotionState, CatShape, localCatInertia);
    btRigidBody *CatBody = new btRigidBody(CatRBInfo);

    // Set the velocity of the Cat based on sight and camera nodes attached to the player
    Ogre::Vector3 pos = this->mPlayer->getSightNode()->_getDerivedPosition();
    Ogre::Vector3 cpos = this->mPlayer->getCameraNode()->_getDerivedPosition();
    btVector3 lookDirection(pos.x - cpos.x, pos.y - cpos.y, pos.z - cpos.z);
    lookDirection.normalize();
    CatBody->setLinearVelocity(lookDirection * CAT_VELOCITY); // bullet

    // OGRE stuff
    Ogre::Entity *entCat = mSceneMgr->createEntity("models/sphere.mesh");
    entCat->setCastShadows(false);
    Ogre::SceneNode *CatNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    CatNode->attachObject(entCat);
    Ogre::Vector3 nodepos = Ogre::Vector3(vec.x(), vec.y(), vec.z());
    Ogre::Vector3 lookdir = Ogre::Vector3(lookDirection.x(), lookDirection.y(), lookDirection.z());
    nodepos += lookdir * CAT_SPAWN_DISTANCE;

    // Set the position of the Cat
    CatTransform.setOrigin(vec + lookDirection * CAT_SPAWN_DISTANCE);
    CatBody->setWorldTransform(CatTransform);
    CatNode->setPosition(nodepos);

    btQuaternion q = CatTransform.getRotation();
    CatNode->setOrientation(Ogre::Quaternion(q.w(), q.x(), q.y(), q.z()));
    CatNode->scale(Ogre::Vector3(0.25, 0.25, 0.25));

    CatBody->setRestitution(1);
    CatBody->setUserPointer(CatNode);

    //add the body to the dynamics world
    this->mPhysicsEngine->getDynamicsWorld()->addRigidBody(CatBody);
}

// ---------------------Adjust mouse clipping area---------------------------
void NewGameManager::windowResized(Ogre::RenderWindow* rw)
{
    int left, top;
    unsigned int width, height, depth;

    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState& ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//-----------------Unattach OIS before window shutdown-----------------------
void NewGameManager::windowClosed(Ogre::RenderWindow* rw)
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
bool NewGameManager::keyPressed(const OIS::KeyEvent& ke)
{
    if (ke.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
    return true;
}

//---------------------------------------------------------------------------
bool NewGameManager::keyReleased(const OIS::KeyEvent& ke)
{
    return true;
}

//---------------------------------------------------------------------------
bool NewGameManager::mouseMoved(const OIS::MouseEvent& me)
{
    return true;
}

//---------------------------------------------------------------------------
bool NewGameManager::mousePressed(
    const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
    return true;
}

//---------------------------------------------------------------------------
bool NewGameManager::mouseReleased(
    const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
    return true;
}

//---------------------------------------------------------------------------
bool NewGameManager::frameRenderingQueued(const Ogre::FrameEvent& fe)
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

    mTimeSinceLastCat += fe.timeSinceLastFrame;
    if (mTimeSinceLastCat > 1.0)
    {
        spawnCat();
        mTimeSinceLastCat -= 1.0;
    }

    return true;
}

//---------------------------------------------------------------------------
bool NewGameManager::frameStarted(const Ogre::FrameEvent& fe)
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

    if (mPlayer != NULL)
    {
        mPlayer->update (fe.timeSinceLastFrame, mKeyboard, mMouse);

        if (mExCamera)
        {
            mExCamera->update (fe.timeSinceLastFrame,
            mPlayer->getCameraNode ()->_getDerivedPosition(),
            mPlayer->getSightNode ()->_getDerivedPosition());
        }
    }

    if (this->mPhysicsEngine != nullptr)
    {
        mPhysicsEngine->getDynamicsWorld()->stepSimulation(1.0f / 60.0f);

        if (mPlayer != nullptr)
        {
            this->mPlayer->updateAction(this->mPhysicsEngine->getDynamicsWorld(), fe.timeSinceLastFrame);
            btTransform& trans = this->mPlayer->getWorldTransform();

            // Update player rendering position
            this->mPlayer->setOgrePosition(Ogre::Vector3(trans.getOrigin().getX(),
            trans.getOrigin().getY() - this->mPlayer->getCollisionObjectHalfHeight(),
            trans.getOrigin().getZ()));

            this->mPlayer->setOgreOrientation(Ogre::Quaternion(trans.getRotation().getW(),
            trans.getRotation().getX(),
            trans.getRotation().getY(),
            trans.getRotation().getZ()));

            // btVector3 t2 = mPlayer->getWorldTransform().getOrigin();
            // std::cout << "player position: " << t2.x() << " " << t2.y() << " " << t2.z() << std::endl;
        }

        for (int i = 0; i < this->mPhysicsEngine->getCollisionObjectCount(); i++)
        {
            // Get object from collision array and cast to rigidbody
            btCollisionObject* obj = this->mPhysicsEngine->getDynamicsWorld()->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);

            // Check collisions that are not with the player?
            if (body && body->getMotionState() && obj->getCollisionFlags() != btCollisionObject::CF_CHARACTER_OBJECT)
            {
                btTransform trans;
                body->getMotionState()->getWorldTransform(trans);
                void *userPointer = body->getUserPointer();

                // Play cat sound on collision
                Mix_PlayChannel(-1, effects.at(rand()%effects.size()) ,0);

                // Convert rigidbody to OGRE scenenode and update position and orientation
                if (userPointer)
                {
                    btQuaternion orientation = trans.getRotation();
                    Ogre::SceneNode *sceneNode = static_cast<Ogre::SceneNode *>(userPointer);

                    sceneNode->setPosition(Ogre::Vector3(trans.getOrigin().getX(),
                        trans.getOrigin().getY(),
                        trans.getOrigin().getZ()));

                    sceneNode->setOrientation(Ogre::Quaternion(orientation.getW(),
                        orientation.getX(),
                        orientation.getY(),
                        orientation.getZ()));
                }
            }
        }

        // Check to see if the player was hit by a ball
        if (mPlayer != nullptr)
        {
            btManifoldArray manifoldArray;
            btPairCachingGhostObject* ghostObject = this->mPlayer->getGhostObject();
            btBroadphasePairArray& pairArray =
            ghostObject->getOverlappingPairCache()->getOverlappingPairArray();

            int numPairs = pairArray.size();

            for (int i = 0; i < numPairs; ++i)
            {
                manifoldArray.clear();

                const btBroadphasePair& pair = pairArray[i];

                btBroadphasePair* collisionPair =
                this->mPhysicsEngine->getDynamicsWorld()->getPairCache()->findPair(
                pair.m_pProxy0,pair.m_pProxy1);

                if (!collisionPair) 
                {
                    continue;
                }

                if (collisionPair->m_algorithm)
                {
                    collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
                }

                for (int j=0;j<manifoldArray.size();j++)
                {
                    btPersistentManifold* manifold = manifoldArray[j];

                    bool isFirstBody = manifold->getBody0() == ghostObject;

                    btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);

                    for (int p = 0; p < manifold->getNumContacts(); ++p)
                    {
                        const btManifoldPoint& pt = manifold->getContactPoint(p);

                        if (pt.getDistance() < 0.f)
                        {
                            const btVector3& ptA = pt.getPositionWorldOnA();
                            const btVector3& ptB = pt.getPositionWorldOnB();
                            const btVector3& normalOnB = pt.m_normalWorldOnB;

                            if (numPairs > 1)
                            {
                                std::cout << std::endl << "********* MANIFOLD COLLISION *********" << std::endl;
                                std::cout << ptA << std::endl;
                                std::cout << ptB << std::endl;
                                std::cout << normalOnB << std::endl;
                                std::cout << "**************************************" << std::endl;
                            }

                            // Exclude collisions with walls
                            if (std::abs(ptA.x()) >= WALL_COLLIDE_ERROR || std::abs(ptB.x()) >= WALL_COLLIDE_ERROR)
                            {
                                continue;
                            }

                            if (std::abs(ptA.z()) >= WALL_COLLIDE_ERROR || std::abs(ptB.z()) >= WALL_COLLIDE_ERROR)
                            {
                                continue;
                            }

                            if (std::abs(ptA.y()) <= 0.0 || std::abs(ptB.y()) <= 0.0)
                            {    
                                continue;
                            }

                            return false;
                        }
                    }
                }
            }
        }
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
    NewGameManager app;

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