#include "Game.h"

#include "Utility/Logger.h"

#include "Actor.h"
#include "Scenes/SceneBase.h"

namespace sl
{
	Game::Game()
		: mGameState(GameState::Playing)
	{

	}



	/// <summary>
	/// Applicationから呼ばれる
	/// </summary>
	void Game::Initialize()
	{
		mRenderer = new Renderer(this);
		mRenderer->Initialize();

		mFrameTimer.Reset();

		mCurrentScene = new SceneBase(this);
		mCurrentScene = mScenes[mCurrentSceneIdx];

		mCurrentScene->LoadData();
	}

	/// <summary>
	/// Applicationから入力イベントを受け取り、InputManagerに渡す。
	/// </summary>
	/// <param name="message"></param>
	/// <param name="wParam"></param>
	/// <param name="lParam"></param>
	void Game::ProcessKeyMouInput(UINT message, WPARAM wParam, LPARAM lParam)
	{
	}

	/// <summary>
	/// ゲームの状態を更新する。
	/// 具体的なゲームロジックは各シーンに委譲する。
	/// </summary>
	void Game::Update()
	{
		if (mGameState == GameState::Playing)
		{
			float deltaTime = mFrameTimer.UpdateDeltaTime();
			mCurrentScene->Update(deltaTime);

			// if (mCurrentScene->IsCompleted())
			{
				// next scene or end
			}

		}

		// Scene Update
		// GameDescとかで処理結果を返す。例えば、SEの再生位置、何をどこに描画するか、Sceneの状態など、
		//if (mCurrentScene == ERhyThm)
		//{
		//	RhythmScene->ProcessInput();
		//	mGameDesc = RhythmScene->Update(deltaTime);
		//}
		//else if (mCurrentScene == E2DAction)
		//{
		// 2DActionScene->ProcessInput();
		//	mGameDesc = 2DActionScene->Update(deltaTime);
		//}
		// title とか　Resultもこうやってやる？

		// 各シーンにGameを持ってほしくないので、GameDescを参照してシーン切り替えを行う
		//if (mGameDesc.NextScen != ENone)
		//{
		//	mCurrentScene = mGameDesc.NextScene; // シーンを切り替える
		// // シーンを切り替えたくなくて、Gameを一時停止したいとかもSceneStateに状態を持たせる
		//}

		// se, bgm, sfxの再生
		// rendererに描画するものを渡す
	}

	/// <summary>
	/// 描画処理を行う。
	/// RenderDescを参照して、Rendererに描画するものを渡す。
	/// </summary>
	void Game::GenerateOutput()
	{
		mRenderer->Draw();
	}

	/// <summary>
	/// Audio, Renderer, InputManagerなどのリソースを解放し、ゲームを終了する。
	/// </summary>
	void Game::Shutdown()
	{

	}

	void Game::AddScene(SceneBase* scene)
	{
		mScenes.emplace_back(scene);
	}

	void Game::RemoveScene(SceneBase* scene)
	{
		auto iter = std::find(mScenes.begin(), mScenes.end(), scene);
		if (iter != mScenes.end())
		{
			std::iter_swap(iter, mScenes.end() - 1);
			mScenes.pop_back();
		}
	}

}
