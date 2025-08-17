#include "Game.h"
#include "Graphics/Renderer.h"

#include "Utility/Logger.h"

#include "Actor.h"

#include "Scenes/TitleScene.h"
#include "Scenes/RhythmScene.h"
#include "Scenes//ResultScene.h"

#include "Components/SpriteComponent.h"
#include "Components/MeshComponent.h"

namespace sl
{
	Game::Game()
		: mGameState(GameState::EPlaying)
	{

	}

	/// <summary>
	/// Application‚©‚çŒÄ‚Î‚ê‚é
	/// </summary>
	void Game::Initialize()
	{
		mRenderer = new Renderer(this);
		mRenderer->Initialize();

		//mAudioManager = std::make_unique<AudioManager>();
		//if (!mAudioManager->Initialize())
		if(!AudioManager::GetInstance().Initialize())
		{
			LOG_ERROR("AudioManager‚Ì‰Šú‰»‚ÉŽ¸”s");
		}

		mFrameTimer.Reset();

		ChangeScene(SceneType::ERhythm);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="message"></param>
	/// <param name="wParam"></param>
	/// <param name="lParam"></param>
	void Game::ProcessInput()
	{
		
	}

	/// <summary>
	/// ƒQ[ƒ€‚Ìó‘Ô‚ðXV‚·‚éB
	/// ‹ï‘Ì“I‚ÈƒQ[ƒ€ƒƒWƒbƒN‚ÍŠeƒV[ƒ“‚ÉˆÏ÷‚·‚éB
	/// </summary>
	void Game::Update()
	{
		
		if (mGameState == GameState::EPlaying)
		{
			float deltaTime = mFrameTimer.UpdateDeltaTime();
			InputManager::GetInstance().ProcessInput(deltaTime);
			mCurrentScene->EaryUpdate(deltaTime);
			mCurrentScene->Update(deltaTime);
			mCurrentScene->LateUpdate(deltaTime);


		}
	}

	/// <summary>
	/// •`‰æˆ—‚ðs‚¤B
	/// RenderDesc‚ðŽQÆ‚µ‚ÄARenderer‚É•`‰æ‚·‚é‚à‚Ì‚ð“n‚·B
	/// </summary>
	void Game::GenerateOutput()
	{
		// Pass¤ËÃûÇ°¤Ç¥Þ©`¥¯¤·¤Æ¡¢render doc¤Ç×R„e¤Ç¤­¤ë¤è¤¦¤Ë¤¹¤ë¤¿¤á¤Î¤ä¤Ä¤Ç¤¹
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> anno;
		mRenderer->GetDeviceContext()->QueryInterface(IID_PPV_ARGS(&anno));

		// »ù±¾Pass
		if (anno.Get()) anno.Get()->BeginEvent(L"Base Pass");
		mRenderer->BeginFrame();
		mRenderer->Draw();
		if (anno.Get()) anno.Get()->EndEvent();

		// Post-processing pass
		mRenderer->PostProcessPass();

		mRenderer->EndFrame();
	}

	/// <summary>
	/// Audio, Renderer, InputManager‚È‚Ç‚ÌƒŠƒ\[ƒX‚ð‰ð•ú‚µAƒQ[ƒ€‚ðI—¹‚·‚éB
	/// </summary>
	void Game::Shutdown()
	{
		mCurrentScene.reset();

		delete mRenderer;
		mRenderer = nullptr;
	}

	void Game::ChangeScene(std::unique_ptr<SceneBase> nextScene)
	{
		LOG_INFO("Scene‚ðØ‚è‘Ö‚¦‚Ü‚·");
		int stopAudioNum = AudioManager::GetInstance().DestroyAllVoice();
		LOG_INFO(stopAudioNum, "ŒÂ‚Ì‰¹‚ðDestroy");
		
		if (mCurrentScene)
		{
			mCurrentScene->Finalize();
		}

		mCurrentScene = std::move(nextScene);
		if (mCurrentScene)
		{
			mCurrentScene->LoadData();
			//mCurrentScene->Initialize();
		}
		else
		{
			LOG_ERROR("ŽŸ‚ÌƒV[ƒ“‚ªnull‚Å‚·B");
		}
	}

	void Game::ChangeScene(SceneType nextSceneType)
	{
		switch (nextSceneType)
		{
		case SceneType::ETitle:
			ChangeScene(std::make_unique<TitleScene>(this));
			break;
		case SceneType::ERhythm:
			ChangeScene(std::make_unique<RhythmScene>(this));
			break;
		case SceneType::EAction:
			// ChangeScene(std::make_unique<Action2DScene>(this));
			break;
		case SceneType::EResult:
			 ChangeScene(std::make_unique<ResultScene>(this));
			break;
		default:
			LOG_ERROR("Unknown scene type");
			break;
		}
	}


	void Game::TestEvent()
	{
		RhythmScene* scene = dynamic_cast<RhythmScene*>(mCurrentScene.get());
		if (scene)
		{
			scene->Judgment();
		}
	}

	void Game::TestShowScore()
	{
		RhythmScene* scene = dynamic_cast<RhythmScene*>(mCurrentScene.get());
		if (scene)
		{
			scene->ShowScore();
		}
	}
}
