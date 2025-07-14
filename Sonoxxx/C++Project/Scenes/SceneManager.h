#pragma once
#include <iostream>

#include "IScene.h"

namespace sl
{
  class SceneManager {
  public:
    void ChangeScene(std::unique_ptr<IScene> newScene); // Todo:unique�|�C���^�������œn���̂悭�Ȃ��B���ꏊ�L�����ڏ����Ă�
    void Update();
    void Draw();

  private:
    std::unique_ptr<IScene> mCurrentScene;
    // Todo:�V�[����2�ێ����Ă���
  };


  // �V�[���؂�ւ��z��
  //SceneManager sceneManager;
  //std::unique_ptr<IGameRenderer> rhythmRenderer = std::make_unique<RhythmRenderer>();
  //sceneManager.ChangeScene(std::make_unique<RhythmScene>(rhythmRenderer.get()));

}