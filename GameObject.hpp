
class GameObject
{
public:
	GameObject(InputComponent* input,
			   PhysicsComponent* physics,
			   GraphicsComponent* graphics,
			   SoundComponent* sound,
			   GUIComponent* gui);
	void send(int message);

	Vector3 velocity;
	Quaternion orientation;
private:
	InputComponent* mInput;
	PhysicsComponent* mPhysics;
	GraphicsComponent* mGraphics;
	SoundComponent* mSound;
	GUIComponent* mGUI;

	static const int MAX_COMPONENTS = 5;
	Component* mComponents[MAX_COMPONENTS];
};
