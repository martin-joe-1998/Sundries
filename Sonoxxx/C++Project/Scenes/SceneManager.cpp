#include "SceneManager.h"

namespace sl
{
  // Todo:uniqueƒ|ƒCƒ“ƒ^‚ğˆø”‚Å“n‚·‚Ì‚æ‚­‚È‚¢
  void SceneManager::ChangeScene(std::unique_ptr<IScene> newScene) {
    if (mCurrentScene) mCurrentScene->Finalize();
    mCurrentScene = std::move(newScene);
    mCurrentScene->Initialize();
  }

  void SceneManager::Update() {
    if (mCurrentScene) mCurrentScene->Update();
  }

  void SceneManager::Draw() {
    if (mCurrentScene) mCurrentScene->Draw();
  }
}