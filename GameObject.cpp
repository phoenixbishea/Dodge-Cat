#include GameObject.hpp

GameObject::GameObject(InputComponent* input,
         PhysicsComponent* physics,
         GraphicsComponent* graphics,
         SoundComponent* sound,
         GUIComponent* gui)
: mInput(input),
  mPhysics(physics),
  mGraphics(graphics),
  mSound(sound),
  mGUI(gui)
{}

GameObject* GameObject::createPlayer()
{
    return new GameObject(new PlayerInputComponent(), new PlayerPhysicsComponent(), new PlayerGraphicsComponent(), new PlayerSoundComponent());
}

void GameObject::send(int message)
{
	for (int i = 0; i < MAX_COMPONENTS; i++)
	{
		if (mComponents[i] != NULL)
		{
			mComponents[i]->receive(message);
		}
	}
}
