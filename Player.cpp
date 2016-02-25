#include "Player.hpp"

Player::Player (Ogre::String name, Ogre::SceneManager *sceneMgr) 
{
    // Setup basic member references
    mName = name;
    mSceneMgr = sceneMgr;

    // Setup basic node structure to handle 3rd person cameras
    mMainNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode (mName);
    mSightNode = mMainNode->createChildSceneNode (mName + "_sight", Ogre::Vector3 (0, 0, -100));
    mCameraNode = mMainNode->createChildSceneNode (mName + "_camera", Ogre::Vector3 (0, 300, 500));

    // Give this character a shape :)
    mEntity = mSceneMgr->createEntity (mName, "ninja.mesh");
    mMainNode->attachObject (mEntity);
}

Player::~Player () 
{
    mMainNode->detachAllObjects ();
    delete mEntity;
    mMainNode->removeAndDestroyAllChildren ();
    mSceneMgr->destroySceneNode (mName);
}

// Updates the Player (movement...)
void Player::update (Ogre::Real elapsedTime, OIS::Keyboard *input) 
{
    // Handle movement
    if (input->isKeyDown (OIS::KC_W)) 
    {
        mMainNode->translate (mMainNode->getOrientation () * Ogre::Vector3 (0, 0, -100 * elapsedTime));
    }
    if (input->isKeyDown (OIS::KC_S)) 
    {
        mMainNode->translate (mMainNode->getOrientation () * Ogre::Vector3 (0, 0, 50 * elapsedTime));
    }
    if (input->isKeyDown (OIS::KC_A)) 
    {
        mMainNode->yaw (Ogre::Radian (2 * elapsedTime));
    }
    if (input->isKeyDown (OIS::KC_D)) 
    {
        mMainNode->yaw (Ogre::Radian (-2 * elapsedTime));
    }
}

// Change visibility - Useful for 1st person view ;)
void Player::setVisible (bool visible) 
{
    mMainNode->setVisible (visible);
}

// The three methods below returns the two camera-related nodes, 
// and the current position of the Player (for the 1st person camera)
Ogre::SceneNode* Player::getSightNode () 
{
    return mSightNode;
}

Ogre::SceneNode* Player::getCameraNode () 
{
    return mCameraNode;
}

Ogre::Vector3 Player::getWorldPosition () 
{
    return mMainNode->_getDerivedPosition ();
}
