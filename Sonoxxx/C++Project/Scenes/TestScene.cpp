//#include "TestScene.h"
//
//#include "Utility/Logger.h"
//
//#include "Game/GameObjects/TestActor.h"
//#include "Game/Note.h"
//#include "Game/Components/MeshComponent.h"
//namespace sl
//{
//	void TestScene::Initialize()
//	{
//		// �K�v�ɉ����Đe�N���X�̏�����
//		// SceneBase::Initialize();
//
//		// TestActor�𐶐����V�[���ɒǉ�
//		auto actor = new TestActor(this);
//		if (!actor)
//		{
//			LOG_ERROR("TestActor�̐����Ɏ��s���܂���");
//			return;
//		}
//		actor->Initialize();
//		LOG_DEBUG("TestScene Initialize!!!!!!!!!!!!!!!!!!!!!!!!!!");
//	}
//
//	void TestScene::Update(float deltaTime)
//	{
//		UpdateActors(deltaTime);
//	}
//
//	void TestScene::Finalize()
//	{
//		mActors.clear();
//	}
//
//	void TestScene::AddActor(Actor* actor)
//	{
//		mActors.emplace_back(actor);
//	}
//
//	void TestScene::RemoveActor(Actor* actor)
//	{
//		auto iter = std::find(mActors.begin(), mActors.end(), actor);
//		if (iter != mActors.end())
//		{
//			// Swap to end of vector and pop off (avoid erase copies)
//			std::iter_swap(iter, mActors.end() - 1);
//			mActors.pop_back();
//		}
//	}
//
//
//	void TestScene::UpdateActors(float deltaTime)
//	{
//		for (auto& actor : mActors)
//		{
//			actor->Update(deltaTime);
//		}
//	}
//}