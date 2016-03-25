#ifndef Player_hpp
#define Player_hpp

class Player : GameObject
{
public:
    Vector velocity;
    Quaternion orientation;

    Vector sightPosition;
    Vector cameraPositon;

    float cannonPitch;
    Quaternion cannonOrientation;

    Player(InputComponent* input, PhysicsComponent* physics,
           GraphicsComponent* graphics, CameraComponent* camera,
           SoundComponent* sound, GUIComponent* gui)
    : mInput(input),
    mPhysics(physics),
    mGraphics(graphics),
    mCamera(camera),
    mSound(sound),
    mGUI(gui)
    {}

    void update(BulletPhysics* physics, OIS::Keyboard* keyboard, OIS::Mouse* mouse, World& world)
    {
        mInput->update(*this, keyboard, mouse, world);
        mPhysics->update(*this, physics, world);
        mGraphics->update(*this);
        mCamera->update(*this);
    }
private:
    InputComponent* mInput;
    PhysicsComponent* mPhysics;
    GraphicsComponent* mGraphics;
    CameraComponent* mCamera;
    SoundComponent* mSound;
    GUIComponent* mGUI;

    static const int MAX_COMPONENTS = 6;
    Component* mComponents[MAX_COMPONENTS];
};

#endif
