#pragma once
#include "IScene.h"
#include "Graphics/Renderers/IGameRenderer.h"

namespace sl
{
  class RhythmScene : public IScene {
  public:
    RhythmScene(IGameRenderer* renderer) : mRenderer(renderer) {}

    void Initialize() override { /* ���W�b�N������ */ }
    void Update() override { /* ����Ȃ� */ }
    void Draw() override { mRenderer->Render(); }
    void Finalize() override { /* �I������ */ }

  private:
    IGameRenderer* mRenderer;
  };

}

