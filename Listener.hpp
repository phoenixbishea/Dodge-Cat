#ifndef Listener_hpp
#define Listener_hpp

#include <OgreRoot.h>

#include <OgreWindowEventUtilities.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include "BulletPhysics.hpp"
#include "ExtendedCamera.hpp"

class Listener
	: public Ogre::WindowEventListener,
	public Ogre::FrameListener,
	public OIS::KeyListener,
	public OIS::MouseListener
{
public:
	Listener(Ogre::RenderWindow* rw, BulletPhysics* physEng, ExtendedCamera* exCam);
	~Listener();

	void initInput();

	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);
private:
	bool keyPressed(const OIS::KeyEvent& ke);
	bool keyReleased(const OIS::KeyEvent& ke);

	bool mouseMoved(const OIS::MouseEvent& me);
	bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id);

	OIS::InputManager* mInputMgr;
    OIS::Keyboard* mKeyboard;
    OIS::Mouse* mMouse;

	Ogre::RenderWindow* mWindow;
    BulletPhysics* mPhysicsEngine;
    ExtendedCamera* mExCamera;  

    bool mShutDown;  
};

#endif