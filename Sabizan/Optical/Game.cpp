#include <iostream>
#include <algorithm>

#include "Game.h"
#include "Graphics/Renderer.h"
#include "UISystem.h"

#include "Stage/MenuStage.h"
#include "Stage//TutorialStage.h"
#include "Stage/StageTest.h"
#include "Stage/Stage1.h"
#include "Stage/Stage2.h"
#include "Stage/Stage3.h"
#include "Stage/Stage4.h"
#include "Stage/EndStage.h"

Game::Game()
: mRenderer(nullptr)
, mGameState(EGameplay)
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
	// Create the renderer
	mRenderer = new Renderer(this);  

    if (!mRenderer->Initialize(1600, 900))  
    {  
        std::cerr << "Failed to initialize renderer." << std::endl;  
        delete mRenderer;  
        mRenderer = nullptr;  
        return false;  
    }  

	// Initalize tickCount
	mTicksCount = SDL_GetTicks();

	// Initialize input key state instance
	for (int key : InputSystem::moveKeyset)
	{
		InputSystem::KeyState ks;
		ks.key = key;
		mKeyStates.push_back(ks);
	}

	for (int key : InputSystem::interactKeyset)
	{
		InputSystem::KeyState ks;
		ks.key = key;
		mKeyStates.push_back(ks);
	}

	// Create Actor instance etc...
	LoadData();

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

		// Compute deltaTime
		// Wait until 16ms has elapsed since last frame (Frame Rate 60)
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

		deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
		if (deltaTime > 0.05f)
		{
			deltaTime = 0.05f;
		}

		mTicksCount = SDL_GetTicks();

		ProcessInput();

		if (mGameState == EQuit)
			break;

		UpdateGame();
		GenerateOutput();
		mStage->ClearCondition();
	}

	return;
}

void Game::Shutdown()
{
	// xiugai
	mStage = nullptr;

	for (Stage* s : mStages) {
		delete s;
	}
	mStages.clear();

	delete mRenderer;
	mRenderer = nullptr;

	// Uninitialize the COM library
	CoUninitialize();
}

void Game::AddStage(Stage* stage)
{
	mStages.emplace_back(stage);
}

void Game::RemoveStage(Stage* stage)
{
	auto iter = std::find(mStages.begin(), mStages.end(), stage);
	if (iter != mStages.end())
	{
		// xiugai
		if (mStage == *iter) mStage = nullptr;
		std::iter_swap(iter, mStages.end() - 1);
		mStages.pop_back();
	}
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

	// Debug feature(clear stage)
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		mStage->SetStageCompleted();
	}

	// Reset(Reload) Stage
	if (GetAsyncKeyState('R') & 0x8000)
	{
		isResetting = true;
	}

	// Detect pressed key and pass to actor
	// ﾏﾈﾈ｡ﾊﾖｱ惞�ﾕﾕ｣ｨﾃｿﾖ｡ 1 ｴﾎ｣ｩ
	auto gp = InputSystem::Instance().ReadGamepadSnapshot();

	// Detect pressed key and pass to actor
	for (auto& ks : mKeyStates)
	{
		bool kbDown = (GetAsyncKeyState(ks.key) & 0x8000) != 0;

		// Gamepad
		bool gpDown = false;
		switch (ks.key) {
		case 'W': gpDown = gp.W; break;
		case 'A': gpDown = gp.A; break;
		case 'S': gpDown = gp.S; break;
		case 'D': gpDown = gp.D; break;
		case 'J': gpDown = gp.J; break;
		case 'K': gpDown = gp.K; break;
		default: break;
		}

		bool isCurrentlyPressed = kbDown || gpDown;

		// If ks.key is pressing
		if (isCurrentlyPressed)
		{
			if (ks.isPressed)
			{
				ks.timeSinceLastMove += deltaTime;
			}
			else
			{
				ks.isPressed = true;
				ks.timeSinceLastMove = 0.0f;
			}
		}
		else
		{
			ks.isPressed = false;
			ks.timeSinceLastMove = 0.0f;
		}
	}

	// Move slimeActor by keyboard input
	if (mGameState == EGameplay && mStage)
	{
		mStage->ProcessInput(deltaTime, mKeyStates);
	}
}

void Game::HandleKeyPress(int key)
{
}

void Game::UpdateGame()
{
	// Reset Stage
	if (isResetting)
	{
		if (mStage->ResetEffect())
		{
			mStage->UnloadData();
			mRenderer->ClearSprites();
			mStage->LoadData();

			isResetting = false;
		}
	}
	
	if (mGameState == EGameplay && mStage)
	{
		// Stage をアップデート
		mStage->Update(deltaTime);
		// Stage が Complete したら次の Stage に行くか、ゲーム終了
		if (mStage->IsStageCompleted())
		{
			// 現在Stageのデータをメモリーから解放
			mStage->UnloadData();
			mRenderer->ClearSprites();
			
			// 次のStageに移行
			++mCurrentStageIndex;
			// 最終ステージをクリアしたら、最初のMenu画面に戻る
			if (mCurrentStageIndex >= mStages.size())
			{
				mCurrentStageIndex = 0;
			}

			mStage = mStages[mCurrentStageIndex];
			mStage->SetStageIncompleted();
			mStage->LoadData();
		}
	}

	return;
}

void Game::GenerateOutput()
{
	// Base pass
	ComPtr<ID3DUserDefinedAnnotation> anno;
	mRenderer->GetDeviceContext()->QueryInterface(IID_PPV_ARGS(&anno));

	if (anno.Get()) anno.Get()->BeginEvent(L"Base Pass");
	mRenderer->BeginFrame();
	mRenderer->Draw();
	if (anno.Get()) anno.Get()->EndEvent();

	mRenderer->PostProcessPass();

	// Swap the buffers (Present(1, 0))
	mRenderer->EndFrame();
}

void Game::LoadData()
{
	// フォントデータをロードする
	HRESULT hr;
	hr = UISystem::Instance().LoadFontImage();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to load font image.", L"Error", MB_OK | MB_ICONERROR);
		//exit(0);
	}

	// ステージのインスタンスを作る。同時にStage1は自分をmStagesにaddするようになっている。
	new MenuStage(this);
	new TutorialStage(this);
	new StageTest(this);
	new Stage1(this);
	new Stage2(this);
	new Stage3(this);
	new Stage4(this);
	new EndStage(this);

	// ステージへのポインターで最初の要素を参照する
	mStage = mStages[mCurrentStageIndex];
	//mStage = mStages[1];

	mStage->LoadData();
}

void Game::UnloadData()
{

}