#include "SceneBase.h"

#include "Utility/Logger.h"

#include "Game/Game.h"
#include "Game/Actor.h"


namespace sl
{
	SceneBase::SceneBase(Game* game)
		: mGame(game)
	{
		mGame->AddScene(this);
	}

	SceneBase::~SceneBase()
	{
		mGame->RemoveScene(this);
	}

	void SceneBase::Update(float deltaTime)
	{
		mUpdatingActors = true;
		for (auto actor : mActors)
		{
			actor->Update(deltaTime);
		}
		mUpdatingActors = false;
	}

	void SceneBase::LoadData()
	{
		mActor = std::make_shared<Actor>(mGame);
		mActor->Initialize();
		// Todo: position偲偐愝掕
		//printf("Actor num: %d\n", mActors.size());
		LOG_INFO("SceneBase: LoadData()");
	}

	void SceneBase::Finalize()
	{

	}

	void SceneBase::AddActor(std::shared_ptr<Actor> actor)
	{
		if (!mUpdatingActors)
		{
			mActors.emplace_back(actor);
		}
	}

	void SceneBase::RemoveActor(std::shared_ptr<Actor> actor)
	{
		auto iter = std::find(mActors.begin(), mActors.end(), actor);
		if (iter != mActors.end())
		{
			// Swap to end of vector and pop off (avoid erase copies)
			std::iter_swap(iter, mActors.end() - 1);
			mActors.pop_back();
		}
	}



}