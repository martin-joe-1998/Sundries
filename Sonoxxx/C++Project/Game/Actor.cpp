#include "Actor.h"
#include <algorithm> // shared_from

#include "Game/Game.h"
#include "Scenes/SceneBase.h"

#include "Components/Component.h"

#include "Game/Components/SpriteComponent.h"

namespace sl
{
	Actor::Actor(Game* game)
		: mGame(game)
		, mState(State::Active)
	{
		mSprite = std::make_shared<SpriteComponent>(this);
	}

	Actor::~Actor()
	{
		// mGame->GetScene()->RemoveActor(shared_from_this());

		while (!mComponents.empty())
		{
			delete mComponents.back();
		}
	}

	// StageÇ©ÇÁåƒÇŒÇÍÇÈ
	void Actor::Initialize()
	{
		mGame->GetScene()->AddActor(shared_from_this());
	}


	/// <summary>
	/// ÉVÅ[ÉìÇ©ÇÁÇÊÇŒÇÍÇÈ
	/// </summary>
	/// <param name="deltaTime"></param>
	void Actor::Update(float deltaTime)
	{
		if (mState == Active)
		{
			UpdateComponents(deltaTime);
			UpdateActor(deltaTime);
		}
	}

	void Actor::UpdateComponents(float deltaTime)
	{
		for (auto comp : mComponents)
		{
			comp->Update(deltaTime);
		}
	}


	void Actor::SetPosition(const Vector3& pos)
	{
		mPosition = pos;
	}
	void Actor::SetRotation(const Vector3& rot)
	{
		mRotation = rot;
	}
	void Actor::SetScale(const Vector3& scl)
	{
		mScale = scl;
	}
	const Vector3& Actor::GetPosition() const
	{
		return mPosition;
	}
	const Vector3& Actor::GetRotation() const
	{
		return mRotation;
	}
	const Vector3& Actor::GetScale() const
	{
		return mScale;
	}

	void Actor::AddComponent(Component* component)
	{
		if (component && std::find(mComponents.begin(), mComponents.end(), component) == mComponents.end())
		{
			mComponents.push_back(component);
		}
	}

	void Actor::RemoveComponent(Component* component)
	{
		auto itr = std::find(mComponents.begin(), mComponents.end(), component);
		if (itr != mComponents.end())
		{
			delete* itr;
			mComponents.erase(itr, mComponents.end());
		}
	}

	void Actor::UpdateActor(float deltaTime)
	{

	}
}
