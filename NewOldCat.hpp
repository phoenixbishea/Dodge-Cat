#ifndef Cat_hpp
#define Cat_hpp

class Cat : public GameObject
{
public:
    Cat(InputComponent* input, PhysicsComponent* physics,
           GraphicsComponent* graphics, CameraComponent* camera,
           SoundComponent* sound, GUIComponent* gui)
        : mInput(input),
        mPhysics(physics),
        mGraphics(graphics),
        mCamera(camera),
        mSound(sound),
        mGUI(gui)
    {}

    void update(BulletPhysics* physics, World& world)
    {
        mPhysics->update(*this, physics, world);
    }
private:
    InputComponent* mInput;
    PhysicsComponent* mPhysics;
    GraphicsComponent* mGraphics;
    CameraComponent* mCamera;
    SoundComponent* mSound;
    GUIComponent* mGUI;
};

#endif
