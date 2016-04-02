#ifndef PlayerGraphicsComponent_hpp
#define PlayerGraphicsComponent_hpp

#include "PlayerData.hpp"

class PlayerGraphicsComponent
{
public:
    Ogre::SceneNode* cannonNode;
	
    PlayerGraphicsComponent(PlayerData& data, Ogre::SceneManager* graphics, const Vector& initialPosition, const Quaternion& initialRotation)
    	: mSceneMgr(graphics)
    {
        // Create scene nodes
        playerNode = graphics->getRootSceneNode()->createChildSceneNode();
        playerNode->setPosition(initialPosition.toOgre());
        playerNode->setOrientation(initialRotation.toOgre());
        sightNode = playerNode->createChildSceneNode(Ogre::Vector3(0, 0, -200));
        cameraNode = playerNode->createChildSceneNode(Ogre::Vector3(0, 300, 500));

        Ogre::SceneNode* baseNode = playerNode->createChildSceneNode();
        cannonNode = playerNode->createChildSceneNode();

        // Retrieve the meshes for the cannon and spray bottle
        Ogre::MeshManager::getSingleton().create("cannon/CannonBase.mesh",
                                                 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        Ogre::MeshManager::getSingleton().create("cannon/CannonSpray.mesh",
                                                 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        Ogre::Entity* baseEntity = graphics->createEntity(Ogre::MeshManager::getSingleton()
                                                          .getByName("cannon/CannonBase.mesh", 
                                                                     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
        Ogre::Entity* sprayEntity = graphics->createEntity(Ogre::MeshManager::getSingleton()
                                                           .getByName("cannon/CannonSpray.mesh",
                                                                      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));

        // Position the cannon base
        baseNode->attachObject(baseEntity);
        baseNode->roll(Ogre::Radian(Ogre::Degree(180)));
        baseNode->yaw(Ogre::Radian(Ogre::Degree(90)));

        // Positon the cannon and spray bottle
        Ogre::SceneNode* topNode = cannonNode->createChildSceneNode();
        topNode->attachObject(sprayEntity);
        topNode->roll(Ogre::Radian(Ogre::Degree(180)));
        topNode->yaw(Ogre::Radian(Ogre::Degree(180)));
        topNode->translate(Ogre::Vector3(0.0, 100.0, 0.0));
        topNode->scale(Ogre::Vector3(0.77, 0.77, 0.77));

        // Scale the cannon
        playerNode->scale(Ogre::Vector3(0.6, 0.6, 0.6));

        data.orientation = playerNode->getOrientation();
    }

    ~PlayerGraphicsComponent()
    {
	    // mSceneMgr->destroySceneNode(cameraNode);
	    // mSceneMgr->destroySceneNode(sightNode);
	    mSceneMgr->destroySceneNode(playerNode);
    }

    void update(PlayerData& obj)
        {
            // Camera and sight position
            sightNode->setPosition(obj.sightPosition.toOgre());
            cameraNode->setPosition(obj.cameraPosition.toOgre());

            // Update sight and camera node positions derived from the player node
            // for camera update
            obj.derivedSight.setValue(sightNode->_getDerivedPosition());
            obj.derivedCamera.setValue(cameraNode->_getDerivedPosition()); 

            // Player position/orientation
            obj.position.setY(obj.position.y() - PLAYER_OFFSET);

            playerNode->translate(obj.position.toOgre() - playerNode->_getDerivedPosition());
            playerNode->setOrientation(obj.orientation.toOgre());

            // Rotate the cannon and spray bottle
            Ogre::Degree pitchChange = Ogre::Degree(obj.cannonPitch);
            Ogre::Degree pitch(cannonNode->getOrientation().getPitch());
            pitch -= pitchChange;

            if (pitch > Ogre::Degree(-10.0f) && pitch < Ogre::Degree(85.0f))
            {
                cannonNode->pitch(-Ogre::Radian(pitchChange));
            }
            // Need to add a setValue() to Quaternion
        obj.cannonOrientation = cannonNode->_getDerivedOrientation();
    }
private:
    Ogre::SceneNode* playerNode;
    Ogre::SceneNode* sightNode;
    Ogre::SceneNode* cameraNode;

    Ogre::SceneManager* mSceneMgr;

    const float PLAYER_OFFSET = 70.0f;
};

#endif
