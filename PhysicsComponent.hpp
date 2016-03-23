#ifndef PhysicsComponent_hpp
#define PhysicsComponent_hpp

class PhysicsComponent
{
public:
	virtual ~PhysicsComponent() {}
	virtual void update(GameObject& obj, World& world) = 0;
};

#endif
