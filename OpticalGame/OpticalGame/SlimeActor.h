#pragma once
#include "Actor.h"

class SlimeActor : public Actor
{
public:
	SlimeActor(class Game* game);
	void UpdateActor(float deltaTime) override;
private:

};

