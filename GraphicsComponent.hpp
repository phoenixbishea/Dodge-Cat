#ifndef GraphicsComponent_hpp
#define GraphicsComponent_hpp

class GraphicsComponent
{
public:
	virtual ~GraphicsComponent() {}
	virtual void update(GameObject& obj, Graphics& graphics) = 0;
};

#endif
