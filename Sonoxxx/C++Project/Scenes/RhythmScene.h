#pragma once
#include "IScene.h"
#include "Graphics/Renderers/IGameRenderer.h"

namespace sl
{
  class RhythmScene : public IScene {
  public:
    RhythmScene(IGameRenderer* renderer) : mRenderer(renderer) {}

    void Initialize() override { /* ロジック初期化 */ }
    void Update() override { /* 判定など */ }
    void Draw() override { mRenderer->Render(); }
    void Finalize() override { /* 終了処理 */ }

  private:
    IGameRenderer* mRenderer;
  };

}

