
class GameObject
{
public:
    GameObject(InputComponent* input,
               PhysicsComponent* physics,
               GraphicsComponent* graphics,
               CameraComponent* camera,
               SoundComponent* sound,
               GUIComponent* gui);
    void send(int message);
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
