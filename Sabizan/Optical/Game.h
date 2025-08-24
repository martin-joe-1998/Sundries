#pragma once
#include "InputSystem.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <SDL.h>
#include <typeinfo>
#include <memory>

class Stage;
class Renderer;

class Game
{
public:
	Game();
	//~Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddStage(Stage* stage);
	void RemoveStage(Stage* stage);
	Stage* GetStage() { return mStage; }

	Renderer* GetRenderer() { return mRenderer; }

	enum GameState
	{
		EMainMenu,
		EGameplay,
		EPaused,
		EQuit
	};

	GameState GetState() const { return mGameState; }
	void SetState(GameState state) { mGameState = state; }
	void ResetKeyTimer(InputSystem::KeyState& keystate) { keystate.timeSinceLastMove = 0.0f; }

private:
	void ProcessInput();
	void HandleKeyPress(int key);
	void UpdateGame();
	void GenerateOutput();

	// Create Stage in here
	void LoadData();
	void UnloadData();

	Uint32 mTicksCount;
	GameState mGameState;
	std::vector<InputSystem::KeyState> mKeyStates;

	Renderer* mRenderer;

	float deltaTime = 0.0f;

	// すべてのステージ
	std::vector<Stage*> mStages;
	// 現在実行中のステージ
	Stage* mStage;
	size_t mCurrentStageIndex = 0;
	bool isResetting = false;
};

