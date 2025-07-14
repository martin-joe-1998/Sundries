#pragma once

#include "Common.h"

namespace sl
{
	class Game;
	class Actor;

	//class TestActor;

	// ��U���̂܂�Game�Ŏg�p����
	// �{���͒��ۃN���X
	class SceneBase
	{
	public:
		SceneBase(Game* game);
		virtual ~SceneBase();
		//virtual ProcessInput(); Todo: InputSystem�\�z��
		virtual void Update(float deltaTime);
		virtual void LoadData();

		// virtual const bool IsStageCompleted() const { return mIsCompleted; }
		virtual void Finalize();

		virtual void AddActor(std::shared_ptr<Actor> actor);
		virtual void RemoveActor(std::shared_ptr<Actor> actor);
		//virtual const std::vector<Actor*>& GetActors() const = 0;

	protected:
		Game* mGame;
		std::vector<std::shared_ptr<Actor>> mActors; // �ł���Δh���N���X�Ɏ���������

		bool mUpdatingActors = false;		

		std::shared_ptr<Actor> mActor;

	};
}

