#pragma once
#include <iostream>
#include "Component.h"

#include "Graphics/Material.h"
namespace sl
{
  class Actor;

  class Renderer;

  /// <summary>
	/// �e�N�X�`���x�[�X�̕`��R���|�[�l���g�B
	/// RenderComponent��Material�ɃX�v���C�g�p�̃e�N�X�`����ݒ肷��
  /// �R���|�[�l���g��������Renderer�Ɏ��M��o�^���ARenderer��Draw�R�[���ŕ`�悷�� ���ˑ��֌W�����Ⴒ����Ȃ�̂ł�߂܂���
  /// </summary>
  class SpriteComponent : public Component
  {
  public:

    SpriteComponent(Actor* owner/*, const Material& material*/);

    ~SpriteComponent();

    void Draw(Renderer& renderer);


  private:
    // Material
    std::unique_ptr<Shader> mShader = nullptr;
    
    void CreateMesh(Renderer& renderer);
    void CreateShader(Renderer& renderer);

  };
}
