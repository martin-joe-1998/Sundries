#include "Game.h"
#include "Triangle.h"
#include "Rectangle.h"
#include <iostream>

Game::Game()
:mRenderer(nullptr)
,mGameState(EGameplay)
{
	// initializes the COM library on the current thread and sets the concurrency model to Single-Threaded Apartment (STA)
	HRESULT hr = CoInitialize(nullptr);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Initialize WICFactory.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}
}

bool Game::Initialize()  
{  
    mRenderer = new Renderer(this);  

    if (!mRenderer->Initialize(1600, 900))  
    {  
        std::cerr << "Failed to initialize renderer." << std::endl;  
        delete mRenderer;  
        mRenderer = nullptr;  
        return false;  
    }  

    mTriangle = new class Triangle(*mRenderer);  
    mRectangle = new class Rectangle(*mRenderer);

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
	// Uninitialize the COM library
	CoUninitialize();

	delete mRenderer;
	delete mTriangle;
	delete mRectangle;

	return;
}

void Game::ProcessInput()
{
	// Detect if the user pressed the escape key (for closing window)
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		mGameState = EQuit;

		if (mRenderer && mRenderer->GetGame() && mRenderer->GetGame()->mRenderer->GetWindow())
		{
			HWND hwnd = mRenderer->GetWindow()->GetHandle();
			if (hwnd)
				PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		return;
	}
}

void Game::UpdateGame()
{
	if (mGameState == EGameplay)
	{

	}

	return;
}

void Game::GenerateOutput()
{
	// Draw
	mRenderer->BeginFrame();
	// Rendering you know aocnuewrgnoivne....orz
	// renderer.Draw();  
	//mTriangle->Draw(*mRenderer);
	mRectangle->Draw(*mRenderer);
	mRenderer->EndFrame();
}
