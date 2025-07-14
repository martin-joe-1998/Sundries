#include "Component.h"
#include "../Actor.h"

#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"

namespace sl
{
	Component::Component(Actor* owner)
		: mOwner(owner)
	{
		mOwner->AddComponent(this);
	}

	Component::~Component()
	{
		mOwner->RemoveComponent(this);
	}
	void Component::Update(float deltaTime)
	{
	}
}
