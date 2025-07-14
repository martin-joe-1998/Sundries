#pragma once

namespace sl
{
	class Actor;

	class Component
	{
	public:
		Component(Actor* owner);
		virtual ~Component();

		virtual void Update(float deltaTime);

		Actor* GetOwner() { return mOwner; }

	protected:
		Actor* mOwner;

	};
}

