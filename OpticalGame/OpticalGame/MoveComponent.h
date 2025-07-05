#pragma once
#include "Component.h"

class MoveComponent : public Component
{
public:
	MoveComponent(class Actor* owner, int updateOrder = 200);
	~MoveComponent();

	void Update(float deltaTime) override;
	virtual void ProcessInput(float deltaTime) override;

private:
};

