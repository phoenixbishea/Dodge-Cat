#include "GameManager.hpp"

#define WALL_COLLIDE_ERROR 749
#define CAT_SPAWN_DISTANCE 150.0f
#define CAT_SPEED 2000

#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>

#include <string>
#include <iostream>
#include <cmath>

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
    physicsEngine(0),
    mPlayer(0),
    mExCamera(0),
    timeSinceLastPhysicsStep(0),
    timeSinceLastCat(0),
    mLose(false)
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
void GameManager::setCharacter (Player* player)
{
  mPlayer = player;
}

//---------------------------------------------------------------------------
void GameManager::setExtendedCamera (ExtendedCamera* cam)
{
  mExCamera = cam;
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
  mRoot = new Ogre::Root(mPluginsCfg); // OGRE

  setupResources(); // OGRE

  // Attempt to configure render system
  bool carryOn = configure(); // OGRE
  if (!carryOn) return false;

  // Load resources
  loadResources(); // OGRE
  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

  chooseSceneManager(); // OGRE

  // Setup Bullet physics
  physicsEngine = new BulletPhysics();
  physicsEngine->initObjects();
  physicsEngine->getDynamicsWorld()->setGravity(btVector3(0.0, -200.0, 0.0));

  setupSound();
  // Create the scene
  createScene(); // Bullet and OGRE: wall, lights, and player creation
  createCamera(); // OGRE + extended camera
  createViewports(); // OGRE

  createFrameListener(); // Set up inputs with OGRE, OIS

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
  ExtendedCamera* exCamera = new ExtendedCamera("ExtendedCamera", mSceneMgr, mCamera);
  setExtendedCamera(exCamera);
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

void GameManager::setupSound()
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
void GameManager::createScene()
{
  // Add ambient light
  mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));
  mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

  Player* player = new Player("Player 1", mSceneMgr, this->physicsEngine);

  setCharacter(player);

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
  groundBody->setRestitution(0.9);

  //add the body to the dynamics world
  this->physicsEngine->getDynamicsWorld()->addRigidBody(groundBody);

  createWalls();
}

//---------------------------------------------------------------------------

void GameManager::createWalls()
{
    /*************
     * Left Wall *
     *************/
    Ogre::Plane leftWallPlane(Ogre::Vector3::UNIT_X, 0);
    Ogre::MeshManager::getSingleton()
        .createPlane("leftWall",
                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                     leftWallPlane,
                     6000, 1500,
                     20, 20,
                     true,
                     1, 5, 5,
                     Ogre::Vector3::UNIT_Z);

    Ogre::Entity *entLeftWall = mSceneMgr->createEntity("LeftWallEntity", "leftWall");
    entLeftWall->setCastShadows(false);
    entLeftWall->setMaterialName("Examples/Rockwall");
    Ogre::SceneNode *LeftWallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("LeftWallNode");
    LeftWallNode->attachObject(entLeftWall);
    LeftWallNode->setPosition(Ogre::Vector3(-750.0, 3000.0, 0.0));

    // create the plane entity to the physics engine, and attach it to the node
    btTransform LeftWallTransform;
    LeftWallTransform.setIdentity();
    LeftWallTransform.setOrigin(btVector3(-750, 3000, 0));

    btScalar LeftWallMass(0.0); // the mass is 0, because the LeftWall is immovable (static)
    btVector3 localLeftWallInertia(0, 0, 0);

    btCollisionShape *LeftWallShape = new btBoxShape(btVector3(0.1, 6000.0, 1500.0));
    btDefaultMotionState *LeftWallMotionState = new btDefaultMotionState(LeftWallTransform);

    LeftWallShape->calculateLocalInertia(LeftWallMass, localLeftWallInertia);

    btRigidBody::btRigidBodyConstructionInfo LeftWallRBInfo(LeftWallMass, LeftWallMotionState, LeftWallShape, localLeftWallInertia);
    btRigidBody *LeftWallBody = new btRigidBody(LeftWallRBInfo);
    LeftWallBody->setRestitution(0.9);

    //add the body to the dynamics world
    this->physicsEngine->getDynamicsWorld()->addRigidBody(LeftWallBody);

    /**************
     * Right Wall *
     **************/
    Ogre::Plane rightWallPlane(Ogre::Vector3::NEGATIVE_UNIT_X, 0);
    Ogre::MeshManager::getSingleton()
        .createPlane("rightWall",
                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                     rightWallPlane,
                     6000, 1500,
                     20, 20,
                     true,
                     1, 5, 5,
                     Ogre::Vector3::UNIT_Z);

    Ogre::Entity *entRightWall = mSceneMgr->createEntity("RightWallEntity", "rightWall");
    entRightWall->setCastShadows(false);
    entRightWall->setMaterialName("Examples/Rockwall");
    Ogre::SceneNode *RightWallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RightWallNode");
    RightWallNode->attachObject(entRightWall);
    RightWallNode->setPosition(Ogre::Vector3(750.0, 3000.0, 0.0));

    // create the plane entity to the physics engine, and attach it to the node
    btTransform RightWallTransform;
    RightWallTransform.setIdentity();
    RightWallTransform.setOrigin(btVector3(750, 3000, 0));

    btScalar RightWallMass(0.0); // the mass is 0, because the RightWall is immovable (static)
    btVector3 localRightWallInertia(0, 0, 0);

    btCollisionShape *RightWallShape = new btBoxShape(btVector3(0.1, 6000.0, 1500.0));
    btDefaultMotionState *RightWallMotionState = new btDefaultMotionState(RightWallTransform);

    RightWallShape->calculateLocalInertia(RightWallMass, localRightWallInertia);

    btRigidBody::btRigidBodyConstructionInfo RightWallRBInfo(RightWallMass, RightWallMotionState, RightWallShape, localRightWallInertia);
    btRigidBody *RightWallBody = new btRigidBody(RightWallRBInfo);
    RightWallBody->setRestitution(0.9);

    //add the body to the dynamics world
    this->physicsEngine->getDynamicsWorld()->addRigidBody(RightWallBody);

    /**************
     * Front Wall *
     **************/
    Ogre::Plane frontWallPlane(Ogre::Vector3::UNIT_Z, 0);
    Ogre::MeshManager::getSingleton()
        .createPlane("frontWall",
                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                     frontWallPlane,
                     6000, 1500,
                     20, 20,
                     true,
                     1, 5, 5,
                     Ogre::Vector3::UNIT_X);

    Ogre::Entity *entFrontWall = mSceneMgr->createEntity("FrontWallEntity", "frontWall");
    entFrontWall->setCastShadows(false);
    entFrontWall->setMaterialName("Examples/Rockwall");
    Ogre::SceneNode *FrontWallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("FrontWallNode");
    FrontWallNode->attachObject(entFrontWall);
    FrontWallNode->setPosition(Ogre::Vector3(0.0, 3000.0, -750.0));

    // create the plane entity to the physics engine, and attach it to the node
    btTransform FrontWallTransform;
    FrontWallTransform.setIdentity();
    FrontWallTransform.setOrigin(btVector3(0, 3000, -750));

    btScalar FrontWallMass(0.0); // the mass is 0, because the FrontWall is immovable (static)
    btVector3 localFrontWallInertia(0, 0, 0);

    btCollisionShape *FrontWallShape = new btBoxShape(btVector3(1500.0, 6000.0, 0.1));
    btDefaultMotionState *FrontWallMotionState = new btDefaultMotionState(FrontWallTransform);

    FrontWallShape->calculateLocalInertia(FrontWallMass, localFrontWallInertia);

    btRigidBody::btRigidBodyConstructionInfo FrontWallRBInfo(FrontWallMass, FrontWallMotionState, FrontWallShape, localFrontWallInertia);
    btRigidBody *FrontWallBody = new btRigidBody(FrontWallRBInfo);
    FrontWallBody->setRestitution(0.9);

    //add the body to the dynamics world
    this->physicsEngine->getDynamicsWorld()->addRigidBody(FrontWallBody);

    /*************
     * Back Wall *
     *************/
    Ogre::Plane backWallPlane(Ogre::Vector3::NEGATIVE_UNIT_Z, 0);
    Ogre::MeshManager::getSingleton()
        .createPlane("backWall",
                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                     backWallPlane,
                     6000, 1500,
                     20, 20,
                     true,
                     1, 5, 5,
                     Ogre::Vector3::UNIT_X);

    Ogre::Entity *entBackWall = mSceneMgr->createEntity("BackWallEntity", "backWall");
    entBackWall->setCastShadows(false);
    entBackWall->setMaterialName("Examples/Rockwall");
    Ogre::SceneNode *BackWallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BackWallNode");
    BackWallNode->attachObject(entBackWall);
    BackWallNode->setPosition(Ogre::Vector3(0.0, 3000.0, 750.0));

    // create the plane entity to the physics engine, and attach it to the node
    btTransform BackWallTransform;
    BackWallTransform.setIdentity();
    BackWallTransform.setOrigin(btVector3(0, 3000, 750));

    btScalar BackWallMass(0.0); // the mass is 0, because the BackWall is immovable (static)
    btVector3 localBackWallInertia(0, 0, 0);

    btCollisionShape *BackWallShape = new btBoxShape(btVector3(1500.0, 6000.0, 0.1));
    btDefaultMotionState *BackWallMotionState = new btDefaultMotionState(BackWallTransform);

    BackWallShape->calculateLocalInertia(BackWallMass, localBackWallInertia);

    btRigidBody::btRigidBodyConstructionInfo BackWallRBInfo(BackWallMass, BackWallMotionState, BackWallShape, localBackWallInertia);
    btRigidBody *BackWallBody = new btRigidBody(BackWallRBInfo);
    BackWallBody->setRestitution(0.9);    

    //add the body to the dynamics world
    this->physicsEngine->getDynamicsWorld()->addRigidBody(BackWallBody);

    /***********
     * Ceiling *
     ***********/
    Ogre::Plane ceilingPlane(Ogre::Vector3::NEGATIVE_UNIT_Y, 0);
    Ogre::MeshManager::getSingleton()
        .createPlane("ceiling",
                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                     ceilingPlane,
                     1500, 1500,
                     20, 20,
                     true,
                     1, 5, 5,
                     Ogre::Vector3::UNIT_Z);

    Ogre::Entity *entCeiling = mSceneMgr->createEntity("CeilingEntity", "ceiling");
    entCeiling->setCastShadows(false);
    entCeiling->setMaterialName("Examples/Rockwall");
    Ogre::SceneNode *CeilingNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CeilingNode");
    CeilingNode->attachObject(entCeiling);
    CeilingNode->setPosition(Ogre::Vector3(0.0, 6000.0, 0.0));

    // create the plane entity to the physics engine, and attach it to the node
    btTransform CeilingTransform;
    CeilingTransform.setIdentity();
    CeilingTransform.setOrigin(btVector3(0, 6000, 0));

    btScalar CeilingMass(0.0); // the mass is 0, because the Ceiling is immovable (static)
    btVector3 localCeilingInertia(0, 0, 0);

    btCollisionShape *CeilingShape = new btBoxShape(btVector3(1500.0, 0.1, 1500.0));
    btDefaultMotionState *CeilingMotionState = new btDefaultMotionState(CeilingTransform);

    CeilingShape->calculateLocalInertia(CeilingMass, localCeilingInertia);

    btRigidBody::btRigidBodyConstructionInfo CeilingRBInfo(CeilingMass, CeilingMotionState, CeilingShape, localCeilingInertia);
    btRigidBody *CeilingBody = new btRigidBody(CeilingRBInfo);
    CeilingBody->setRestitution(0.9);

    //add the body to the dynamics world
    this->physicsEngine->getDynamicsWorld()->addRigidBody(CeilingBody);
}

//---------------------------------------------------------------------------
void GameManager::destroyScene()
{
  Mix_FreeMusic(music);
  for(int i = 0; i < effects.size(); ++i)
    Mix_FreeChunk(effects[i]);
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

  timeSinceLastCat += fe.timeSinceLastFrame;
  if (timeSinceLastCat > 1.0)
  {
      spawnCat();
      timeSinceLastCat -= 1.0;
  }


  return true;
}

bool GameManager::frameStarted(const Ogre::FrameEvent& fe)
{
    timeSinceLastPhysicsStep += fe.timeSinceLastFrame;
    if (timeSinceLastPhysicsStep > 1.0 / 60.0)
        timeSinceLastPhysicsStep -= 1.0 / 60.0;
    else
        return true;
    // mPlayerNode->translate(mDirection * fe.timeSinceLastFrame, Ogre::Node::TS_LOCAL);

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

   if (this->physicsEngine != nullptr)
   {
        physicsEngine->getDynamicsWorld()->stepSimulation(1.0f / 60.0f);

        if (mPlayer != nullptr)
        {
            this->mPlayer->updateAction(this->physicsEngine->getDynamicsWorld(), fe.timeSinceLastFrame);
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

        for (int i = 0; i < this->physicsEngine->getCollisionObjectCount(); i++)
        {
            // Get object from collision array and cast to rigidbody
            btCollisionObject* obj = this->physicsEngine->getDynamicsWorld()->getCollisionObjectArray()[i];
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
                    this->physicsEngine->getDynamicsWorld()->getPairCache()->findPair(
                        pair.m_pProxy0,pair.m_pProxy1);

                if (!collisionPair) continue;

                if (collisionPair->m_algorithm)
                    collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

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

                            // Exclude collisions with walls
                            if (std::abs(ptA.x()) >= WALL_COLLIDE_ERROR || std::abs(ptB.x()) >= WALL_COLLIDE_ERROR)
                                continue;
                            if (std::abs(ptA.z()) >= WALL_COLLIDE_ERROR || std::abs(ptB.z()) >= WALL_COLLIDE_ERROR)
                                continue;
                            if (std::abs(ptA.y()) <= 0.0 || std::abs(ptB.y()) <= 0.0)
                                continue;

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

void GameManager::spawnCat()
{
    // create the plane entity to the physics engine, and attach it to the node
    btTransform CatTransform = mPlayer->getWorldTransform();
    btVector3 vec = CatTransform.getOrigin();

    btScalar CatMass(10.0);
    btVector3 localCatInertia(0, 0, 0);

    btCollisionShape *CatShape = new btSphereShape(20.0);
    this->physicsEngine->getCollisionShapes().push_back(CatShape);
    btDefaultMotionState *CatMotionState = new btDefaultMotionState(CatTransform);

    CatShape->calculateLocalInertia(CatMass, localCatInertia);

    btRigidBody::btRigidBodyConstructionInfo CatRBInfo(CatMass, CatMotionState, CatShape, localCatInertia);
    btRigidBody *CatBody = new btRigidBody(CatRBInfo);

    // Set the velocity of the Cat based on sight and camera nodes attached to the player
    Ogre::Vector3 direction = this->mPlayer->getOgreLookDirection();
    btVector3 lookDirection(direction.x, direction.y, direction.z);
    lookDirection.normalize();
    CatBody->setLinearVelocity(lookDirection * CAT_SPEED); // bullet

    // OGRE stuff
    static bool meshCreated = false;
    if (!meshCreated)
    {
      Ogre::MeshManager::getSingleton()
        .create("Cat.mesh",
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      meshCreated = true;
    }

    Ogre::Entity *entCat =
      mSceneMgr
      ->createEntity(Ogre::MeshManager::getSingleton()
                     .getByName("Cat.mesh",
                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
    entCat->setCastShadows(false);
    Ogre::SceneNode *mainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::SceneNode *CatNode = mainNode->createChildSceneNode();
    CatNode->attachObject(entCat);
    Ogre::Real catScale = 100.0;
    CatNode->scale(Ogre::Vector3(catScale, catScale, catScale));
    CatNode->yaw(Ogre::Radian(Ogre::Degree(180)));
    CatNode->pitch(Ogre::Radian(Ogre::Degree(90)));
    Ogre::Vector3 nodepos = Ogre::Vector3(vec.x(), vec.y(), vec.z());
    Ogre::Vector3 lookdir = Ogre::Vector3(lookDirection.x(), lookDirection.y(), lookDirection.z());
    Ogre::Vector3 up(0, 1, 0);
    Ogre::Vector3 cannonOffset = lookdir.crossProduct(up);
    std::cout << "Cannon Offset: " << cannonOffset << std::endl;
    nodepos += lookdir * CAT_SPAWN_DISTANCE + cannonOffset * 55;

    // Set the position of the Cat
    CatTransform.setOrigin(vec +
                           lookDirection * CAT_SPAWN_DISTANCE +
                           btVector3(cannonOffset.x, cannonOffset.y, cannonOffset.z) * 55);
    CatBody->setWorldTransform(CatTransform);
    mainNode->setPosition(nodepos);

    btQuaternion q = CatTransform.getRotation();
    mainNode->setOrientation(Ogre::Quaternion(q.w(), q.x(), q.y(), q.z()));

    CatBody->setRestitution(1);
    CatBody->setUserPointer(mainNode);

    //add the body to the dynamics world
    this->physicsEngine->getDynamicsWorld()->addRigidBody(CatBody);
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
  case OIS::KC_M:
    Mix_VolumeMusic(Mix_VolumeMusic(-1)-1);
    break;
  case OIS::KC_N:
    //music
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
