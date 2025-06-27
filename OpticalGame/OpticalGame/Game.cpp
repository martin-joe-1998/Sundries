#include "Game.h"
#include "Renderer.h"
#include "Triangle.h"
#include <iostream>

Game::Game()
:mRenderer(nullptr)
,mGameState(EMainMenu)
{
}

bool Game::Initialize()
{
	mRenderer = new Renderer(this);	
	if (!mRenderer)
	{
		std::cerr << "Failed to create renderer." << std::endl;
		return false;
	}

	SetScreenSize(1600, 900);

	if (!mRenderer->Initialize(mWindowWidth, mWindowHeight))
	{
		std::cerr << "Failed to initialize renderer." << std::endl;
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}

	mTriangle = new Triangle(*mRenderer);

	return true;
}

void Game::RunLoop()
{
	MSG msg = {};
	while (mGameState != EQuit)
	{
		// Process messages in the message queue
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// Check for quit message, if Window is closed, switch GameState to EQuit
			if (msg.message == WM_QUIT) 
			{
				mGameState = EQuit;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (mGameState == EQuit)
			break;

		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}

	return;
}

void Game::Shutdown()
{
	delete mRenderer;
	delete mTriangle;

	return;
}

void Game::ProcessInput()
{
	return;
}

void Game::UpdateGame()
{
	return;
}

void Game::GenerateOutput()
{
	// Draw
	mRenderer->BeginFrame();
	// Rendering you know aocnuewrgnoivne....orz
	// renderer.Draw();  
	mTriangle->Draw(*mRenderer);
	mRenderer->EndFrame();
}
