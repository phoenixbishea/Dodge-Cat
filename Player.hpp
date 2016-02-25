#ifndef Player_hpp
#define Player_hpp

#include <OgreEntity.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OgreSceneManager.h>

class Player 
{
public:
    Player (Ogre::String name, Ogre::SceneManager* sceneMgr);

    ~Player ();

    // Updates the Player (movement...)
    void update (Ogre::Real elapsedTime, OIS::Keyboard* input);

    // Change visibility - Useful for 1st person view ;)
    void setVisible (bool visible);

    // The three methods below returns the two camera-related nodes, 
    // and the current position of the Player (for the 1st person camera)
    Ogre::SceneNode* getSightNode ();

    Ogre::SceneNode* getCameraNode ();

    Ogre::Vector3 getWorldPosition ();

protected:
    Ogre::String mName;
    Ogre::SceneNode* mMainNode; // Main Player node
    Ogre::SceneNode* mSightNode; // "Sight" node - The Player is supposed to be looking here
    Ogre::SceneNode* mCameraNode; // Node for the chase camera
    Ogre::Entity* mEntity; // Player entity
    Ogre::SceneManager* mSceneMgr;
};

#endif