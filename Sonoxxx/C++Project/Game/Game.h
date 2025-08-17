#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include "Utility/Math.h"
#include "Utility/FrameTimer.h"
#include "Utility/RhythmTimer.h"

//#include "Graphics/Renderer.h"
#include "Input/InputManager.h"
// Audio, Resouce, UI, Scene
#include "Audio/AudioManager.h"

#include "Scenes/SceneBase.h"


namespace sl
{
	class Renderer;
	class InputManager;
	class Actor;

	enum GameState
	{
		EPlaying,
		EPaused,
		EQuit,
	};

	enum SceneType
	{
		ENone,
		ETitle,
		ERhythm,
		EAction,
		EResult,
	};

	/// <summary>
	/// Input, Audio, Rendrer, SceneÇ»Ç«ÇìùäáÇ∑ÇÈÉNÉâÉXÅB
	/// </summary>
	class Game
	{
	public:
		static Game& GetInstance()
		{
			static Game instance;
			return instance;
		}

		void Initialize();
		void ProcessInput();
		void Update();
		void GenerateOutput();
		void Shutdown();


		const std::unique_ptr<SceneBase>& GetScene() const { return mCurrentScene; }
		void ChangeScene(std::unique_ptr<SceneBase> nextScene);
		void ChangeScene(SceneType nextSceneType);
		
		Renderer* GetRenderer() { return mRenderer; }
		

		GameState GetState() const { return mGameState; }
		void SetState(GameState state) { mGameState = state; }

		// Todo: Ç‚ÇŒÇ∑Ç¨
		void TestEvent();
		void TestShowScore();
		

	private:
		Game();
		Game(const Game&) = delete;
		Game& operator=(const Game&) = delete;

		Renderer* mRenderer = nullptr;
		//InputManager* mInputManager = nullptr;
		//std::unique_ptr<AudioManager> mAudioManager;

		GameState mGameState;
		RhythmTimer mRhythmTimer;
		FrameTimer mFrameTimer;
		std::unique_ptr<SceneBase> mCurrentScene;
		
	};

}
