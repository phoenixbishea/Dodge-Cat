#ifndef InputComponent_hpp
#define InputComponent_hpp

class InputComponent
{
public:
	virtual ~InputComponent() {}
	virtual void update(GameObject& obj, World& world) = 0;
};

#endif
