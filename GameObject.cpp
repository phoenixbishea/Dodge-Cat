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
