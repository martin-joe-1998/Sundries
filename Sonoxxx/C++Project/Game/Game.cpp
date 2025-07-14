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
	/// Application����Ă΂��
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
	/// Application������̓C�x���g���󂯎��AInputManager�ɓn���B
	/// </summary>
	/// <param name="message"></param>
	/// <param name="wParam"></param>
	/// <param name="lParam"></param>
	void Game::ProcessKeyMouInput(UINT message, WPARAM wParam, LPARAM lParam)
	{
	}

	/// <summary>
	/// �Q�[���̏�Ԃ��X�V����B
	/// ��̓I�ȃQ�[�����W�b�N�͊e�V�[���ɈϏ�����B
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
		// GameDesc�Ƃ��ŏ������ʂ�Ԃ��B�Ⴆ�΁ASE�̍Đ��ʒu�A�����ǂ��ɕ`�悷�邩�AScene�̏�ԂȂǁA
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
		// title �Ƃ��@Result����������Ă��H

		// �e�V�[����Game�������Ăق����Ȃ��̂ŁAGameDesc���Q�Ƃ��ăV�[���؂�ւ����s��
		//if (mGameDesc.NextScen != ENone)
		//{
		//	mCurrentScene = mGameDesc.NextScene; // �V�[����؂�ւ���
		// // �V�[����؂�ւ������Ȃ��āAGame���ꎞ��~�������Ƃ���SceneState�ɏ�Ԃ���������
		//}

		// se, bgm, sfx�̍Đ�
		// renderer�ɕ`�悷����̂�n��
	}

	/// <summary>
	/// �`�揈�����s���B
	/// RenderDesc���Q�Ƃ��āARenderer�ɕ`�悷����̂�n���B
	/// </summary>
	void Game::GenerateOutput()
	{
		mRenderer->Draw();
	}

	/// <summary>
	/// Audio, Renderer, InputManager�Ȃǂ̃��\�[�X��������A�Q�[�����I������B
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
