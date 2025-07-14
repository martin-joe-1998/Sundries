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
//		// 必要に応じて親クラスの初期化
//		// SceneBase::Initialize();
//
//		// TestActorを生成しシーンに追加
//		auto actor = new TestActor(this);
//		if (!actor)
//		{
//			LOG_ERROR("TestActorの生成に失敗しました");
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