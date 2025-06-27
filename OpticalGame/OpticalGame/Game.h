#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"


class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	enum GameState
	{
		EMainMenu,
		EGameplay,
		EPaused,
		EQuit
	};

	GameState GetState() const { return mGameState; }
	void SetState(GameState state) { mGameState = state; }

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void SetScreenSize(int width, int height) { mWindowWidth = width; mWindowHeight = height; }

	GameState mGameState;

	int mWindowWidth;
	int mWindowHeight;
	class Triangle* mTriangle;
	class Renderer* mRenderer;
};

