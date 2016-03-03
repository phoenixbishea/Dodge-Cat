#include "Listener.hpp"

//---------------------------------------------------------------------------
Listener::Listener(Ogre::RenderWindow* rw, BulletPhysics* physEng, 
    ExtendedCamera* exCam)

    : mInputMgr(0),
    mKeyboard(0),
    mMouse(0),
    mWindow(rw), 
    mPhysicsEngine(physEng), 
    mExCamera(exCam),

    mShutDown(false)
{
}

//---------------------------------------------------------------------------
Listener::~Listener()
{
}

//---------------------------------------------------------------------------
void Listener::initInput()
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

    // Register Listener as source of CALLBACK methods
    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
}

// ---------------------Adjust mouse clipping area---------------------------
void Listener::windowResized(Ogre::RenderWindow* rw)
{
    int left, top;
    unsigned int width, height, depth;

    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState& ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//-----------------Unattach OIS before window shutdown-----------------------
void Listener::windowClosed(Ogre::RenderWindow* rw)
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
bool Listener::keyPressed(const OIS::KeyEvent& ke)
{
    if (ke.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
    return true;
}

//---------------------------------------------------------------------------
bool Listener::keyReleased(const OIS::KeyEvent& ke)
{
    return true;
}

//---------------------------------------------------------------------------
bool Listener::mouseMoved(const OIS::MouseEvent& me)
{
    return true;
}

//---------------------------------------------------------------------------
bool Listener::mousePressed(
    const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
    return true;
}

//---------------------------------------------------------------------------
bool Listener::mouseReleased(
    const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
    return true;
}

//---------------------------------------------------------------------------
bool Listener::RenderingQueued(const Ogre::Event& fe)
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

    // timeSinceLastCat += fe.timeSinceLast;
    // if (timeSinceLastCat > 1.0)
    // {
    //     spawnCat();
    //     timeSinceLastCat -= 1.0;
    // }

    return true;
}

//---------------------------------------------------------------------------
bool Listener::Started(const Ogre::Event& fe)
{
//     timeSinceLastPhysicsStep += fe.timeSinceLast;
//     if (timeSinceLastPhysicsStep > 1.0 / 60.0)
//         timeSinceLastPhysicsStep -= 1.0 / 60.0;
//     else
//         return true;
//     // mPlayerNode->translate(mDirection * fe.timeSinceLast, Ogre::Node::TS_LOCAL);

//     if (mPlayer != NULL)
//     {
//         mPlayer->update (fe.timeSinceLast, mKeyboard, mMouse);

//         if (mExCamera)
//         {
//             mExCamera->update (fe.timeSinceLast,
//                                mPlayer->getCameraNode ()->_getDerivedPosition(),
//                                mPlayer->getSightNode ()->_getDerivedPosition());
//         }
//     }

//    if (this->mPhysicsEngine != nullptr)
//    {
//         mPhysicsEngine->getDynamicsWorld()->stepSimulation(1.0f / 60.0f);

//         if (mPlayer != nullptr)
//         {
//             this->mPlayer->updateAction(this->mPhysicsEngine->getDynamicsWorld(), fe.timeSinceLast);
//             btTransform& trans = this->mPlayer->getWorldTransform();

//             // Update player rendering position
//             this->mPlayer->setOgrePosition(Ogre::Vector3(trans.getOrigin().getX(),
//                                                        trans.getOrigin().getY() - this->mPlayer->getCollisionObjectHalfHeight(),
//                                                        trans.getOrigin().getZ()));

//             this->mPlayer->setOgreOrientation(Ogre::Quaternion(trans.getRotation().getW(),
//                                                              trans.getRotation().getX(),
//                                                              trans.getRotation().getY(),
//                                                              trans.getRotation().getZ()));

//             // btVector3 t2 = mPlayer->getWorldTransform().getOrigin();
//             // std::cout << "player position: " << t2.x() << " " << t2.y() << " " << t2.z() << std::endl;
//         }

//         for (int i = 0; i < this->mPhysicsEngine->getCollisionObjectCount(); i++)
//         {
//             // Get object from collision array and cast to rigidbody
//             btCollisionObject* obj = this->mPhysicsEngine->getDynamicsWorld()->getCollisionObjectArray()[i];
//             btRigidBody* body = btRigidBody::upcast(obj);

//             // Check collisions that are not with the player?
//             if (body && body->getMotionState() && obj->getCollisionFlags() != btCollisionObject::CF_CHARACTER_OBJECT)
//             {
//                 btTransform trans;
//                 body->getMotionState()->getWorldTransform(trans);
//                 void *userPointer = body->getUserPointer();

//                 // Play cat sound on collision
//                 Mix_PlayChannel(-1, effects.at(rand()%effects.size()) ,0);

//                 // Convert rigidbody to OGRE scenenode and update position and orientation
//                 if (userPointer)
//                 {
//                     btQuaternion orientation = trans.getRotation();
//                     Ogre::SceneNode *sceneNode = static_cast<Ogre::SceneNode *>(userPointer);

//                     sceneNode->setPosition(Ogre::Vector3(trans.getOrigin().getX(),
//                                                          trans.getOrigin().getY(),
//                                                          trans.getOrigin().getZ()));

//                     sceneNode->setOrientation(Ogre::Quaternion(orientation.getW(),
//                                                                orientation.getX(),
//                                                                orientation.getY(),
//                                                                orientation.getZ()));
//                 }
//             }
//         }

//         // Check to see if the player was hit by a ball
//         if (mPlayer != nullptr)
//         {
//             btManifoldArray manifoldArray;
//             btPairCachingGhostObject* ghostObject = this->mPlayer->getGhostObject();
//             btBroadphasePairArray& pairArray =
//                 ghostObject->getOverlappingPairCache()->getOverlappingPairArray();

//             int numPairs = pairArray.size();

//             for (int i = 0; i < numPairs; ++i)
//             {
//                 manifoldArray.clear();

//                 const btBroadphasePair& pair = pairArray[i];

//                 btBroadphasePair* collisionPair =
//                     this->mPhysicsEngine->getDynamicsWorld()->getPairCache()->findPair(
//                         pair.m_pProxy0,pair.m_pProxy1);

//                 if (!collisionPair) continue;

//                 if (collisionPair->m_algorithm)
//                     collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

//                 for (int j=0;j<manifoldArray.size();j++)
//                 {
//                     btPersistentManifold* manifold = manifoldArray[j];

//                     bool isFirstBody = manifold->getBody0() == ghostObject;

//                     btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);

//                     for (int p = 0; p < manifold->getNumContacts(); ++p)
//                     {
//                         const btManifoldPoint& pt = manifold->getContactPoint(p);

//                         if (pt.getDistance() < 0.f)
//                         {
//                             const btVector3& ptA = pt.getPositionWorldOnA();
//                             const btVector3& ptB = pt.getPositionWorldOnB();
//                             const btVector3& normalOnB = pt.m_normalWorldOnB;

//                             if (numPairs > 1)
//                             {
//                                 std::cout << std::endl << "********* MANIFOLD COLLISION *********" << std::endl;
//                                 std::cout << ptA << std::endl;
//                                 std::cout << ptB << std::endl;
//                                 std::cout << normalOnB << std::endl;
//                                 std::cout << "**************************************" << std::endl;
//                             }

//                             // Exclude collisions with walls
//                             if (std::abs(ptA.x()) >= WALL_COLLIDE_ERROR || std::abs(ptB.x()) >= WALL_COLLIDE_ERROR)
//                                 continue;
//                             if (std::abs(ptA.z()) >= WALL_COLLIDE_ERROR || std::abs(ptB.z()) >= WALL_COLLIDE_ERROR)
//                                 continue;
//                             if (std::abs(ptA.y()) <= 0.0 || std::abs(ptB.y()) <= 0.0)
//                                 continue;

//                             return false;
//                         }
//                     }
//                 }
//             }
//         }
//    }
//     return true;
}
