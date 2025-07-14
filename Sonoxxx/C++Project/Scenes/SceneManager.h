#pragma once
#include <iostream>

#include "IScene.h"

namespace sl
{
  class SceneManager {
  public:
    void ChangeScene(std::unique_ptr<IScene> newScene); // Todo:uniqueポインタを引数で渡すのよくない。これ所有権を移譲してる
    void Update();
    void Draw();

  private:
    std::unique_ptr<IScene> mCurrentScene;
    // Todo:シーンを2つ保持しておく
  };


  // シーン切り替え想定
  //SceneManager sceneManager;
  //std::unique_ptr<IGameRenderer> rhythmRenderer = std::make_unique<RhythmRenderer>();
  //sceneManager.ChangeScene(std::make_unique<RhythmScene>(rhythmRenderer.get()));

}