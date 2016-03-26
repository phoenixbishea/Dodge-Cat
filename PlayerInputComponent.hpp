#ifndef PlayerInputComponent_hpp
#define PlayerInputComponent_hpp

#include <cmath>
#include "PlayerData.hpp"

float clampf (float val, float min, float max)
{
	return std::max(min, std::min(val, max));
}

class PlayerInputComponent
{
public:
	~PlayerInputComponent()
	{
	}
	void update(PlayerData& obj, OIS::Keyboard* keyboard, OIS::Mouse* mouse, float elapsedTime)
	{
		printf("Update input\n");
		// KEYBOARD
		bool forward = keyboard->isKeyDown(OIS::KC_W) || 
					   keyboard->isKeyDown(OIS::KC_COMMA) || 
					   keyboard->isKeyDown(OIS::KC_UP);

		bool down = keyboard->isKeyDown (OIS::KC_S) || 
					keyboard->isKeyDown(OIS::KC_O) || 
					keyboard->isKeyDown(OIS::KC_DOWN);

		bool left = keyboard->isKeyDown (OIS::KC_A) || 
					keyboard->isKeyDown(OIS::KC_LEFT);

		bool right = keyboard->isKeyDown(OIS::KC_D) || 
					 keyboard->isKeyDown(OIS::KC_E) || 
					 keyboard->isKeyDown(OIS::KC_RIGHT);

		if (forward)
		{
			printf("forward input\n");
			// velocity should be a btVector3 that can be handled by bullet
			obj.velocity = obj.orientation * Vector(0.0f, 0.0f, -WALK_SPEED);
			printf("up velocity: %f, %f, %f\n", obj.velocity.x(), obj.velocity.y(), obj.velocity.z());
		}
		else if (down)
		{
			printf("down input\n");
			obj.velocity = obj.orientation * Vector(0.0f, 0.0f, WALK_SPEED);
			printf("down velocity: %f, %f, %f\n", obj.velocity.x(), obj.velocity.y(), obj.velocity.z());
		}
		else
		{
			obj.velocity = Vector(0.0f, 0.0f, 0.0f);
		}

		if (left)
		{
			printf("left input\n");
			Quaternion rotation = Quaternion(0.0f, ROTATION_SPEED * elapsedTime, 0.0f, 1.0f);
			obj.orientation *= rotation;
		}
		else if (right)
		{
			printf("right input\n");
			Quaternion rotation = Quaternion(0.0f, -ROTATION_SPEED * elapsedTime, 0.0f, 1.0f);
			obj.orientation *= rotation;
		}

		// MOUSE
		const OIS::MouseState& me = mouse->getMouseState();
		bool mouseMovement = me.Y.rel < -DEAD_ZONE || me.Y.rel > DEAD_ZONE;

		if (mouseMovement)
		{
			obj.cannonPitch = me.Y.rel * 0.03f;

			float moveAmount = me.Y.rel * DAMPING_FACTOR;

			float sightYPos = obj.sightPosition.y();
			float cameraYPos = obj.cameraPosition.y();
			
			sightYPos = clampf(sightYPos - moveAmount, LOWER_BOUND, UPPER_SIGHT);
			cameraYPos = clampf(cameraYPos + moveAmount, LOWER_BOUND, UPPER_CAM);

			// Update sight's y if camera's y is below 300
			if (cameraYPos <= MIDDLE_BOUND)
			{
				obj.sightPosition.setY(sightYPos);
				obj.sightPosition.setZ(-200.0f);
			}	

			// Update camera's y if sight's y is below 300
			if (sightYPos <= MIDDLE_BOUND)
			{
				obj.cameraPosition.setY(cameraYPos);
				obj.cameraPosition.setZ(500.0f);
			}
			// Sight's y is above 300 and camera's y = 0
			// Move camera along the player's z axis
			else
			{
				float cameraZPos = obj.cameraPosition.z();
				cameraZPos = clampf(cameraZPos + (moveAmount * DAMPING_FACTOR), 100, 500);
				obj.cameraPosition.setY(0.0f);
				obj.cameraPosition.setZ(cameraZPos);
			}
		}
		else
		{
			obj.cannonPitch = 0.0f;
		}
	}

private:
	const float DEAD_ZONE = 0.1f;
	const float ROTATION_SPEED = 2.0f;
	const float WALK_SPEED = 500.0f;

	const float UPPER_CAM = 600.0f;
	const float UPPER_SIGHT = 500.0f;
	const float MIDDLE_BOUND = 300.0f;
	const float LOWER_BOUND = 0.0f;
	const float DAMPING_FACTOR = 0.5f;
};

#endif
