#pragma once

#include "Common.h"

namespace sl
{
	class Game;
	class Actor;

	//class TestActor;

	// 一旦そのままGameで使用する
	// 本来は抽象クラス
	class SceneBase
	{
	public:
		SceneBase(Game* game);
		virtual ~SceneBase();
		//virtual ProcessInput(); Todo: InputSystem構築後
		virtual void Update(float deltaTime);
		virtual void LoadData();

		// virtual const bool IsStageCompleted() const { return mIsCompleted; }
		virtual void Finalize();

		virtual void AddActor(std::shared_ptr<Actor> actor);
		virtual void RemoveActor(std::shared_ptr<Actor> actor);
		//virtual const std::vector<Actor*>& GetActors() const = 0;

	protected:
		Game* mGame;
		std::vector<std::shared_ptr<Actor>> mActors; // できれば派生クラスに持たせたい

		bool mUpdatingActors = false;		

		std::shared_ptr<Actor> mActor;

	};
}

