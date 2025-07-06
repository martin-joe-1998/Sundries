#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <SDL.h>

class Game
{
public:
	Game();
	//~Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor) {}
	void RemoveActor(class Actor* actor) {}

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
	void HandleKeyPress(int key);
	void UpdateGame();
	void GenerateOutput();

	Uint32 mTicksCount;
	GameState mGameState;

	//class Triangle* mTriangle;
	class Rectangle* mRectangle;
	class Renderer* mRenderer;

	float deltaTime = 0.0f;
};

