#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include "Utility/Math.h"
#include "Utility/FrameTimer.h"
#include "Utility/RhythmTimer.h"
#include "Common.h"

#include "Graphics/Renderer.h"


// #include "Input/InputManager.h"
// Audio, Resouce, UI, Scene


namespace sl
{
	class Renderer;
	class SceneBase;
	class Actor;

	enum GameState
	{
		Playing,
		Paused,
		Quit,
	};

	/// <summary>
	/// Input, Audio, Rendrer, Scene‚È‚Ç‚ğ“Š‡‚·‚éƒNƒ‰ƒXB
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
		void ProcessKeyMouInput(UINT message, WPARAM wParam, LPARAM lParam);
		void Update();
		void GenerateOutput();
		void Shutdown();

		// Scene‚ğŠÇ—
		void AddScene(SceneBase* scene);
		void RemoveScene(SceneBase* scene);
		SceneBase* GetScene() { return mCurrentScene; }
		
		Renderer* GetRenderer() { return mRenderer; }

		GameState GetState() const { return mGameState; }
		void SetState(GameState state) { mGameState = state; }

	private:
		Game();
		Game(const Game&) = delete;
		Game& operator=(const Game&) = delete;

		// InputManager mInputManager; ‚Ü‚¾À‘•‚µ‚Ä‚È‚¢
		// AudioManager mAudioManager; ‚Ü‚¾À‘•‚µ‚Ä‚È‚¢
		Renderer* mRenderer = nullptr;

		GameState mGameState;
		RhythmTimer mRhythmTimer;
		FrameTimer mFrameTimer;
		SceneBase* mCurrentScene = nullptr;
		std::vector<SceneBase*> mScenes;
		int mCurrentSceneIdx = 0;
		
	};

}
