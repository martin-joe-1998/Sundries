#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Utility/Math.h"

#include "Shader.h"
#include "Texture.h"

namespace sl
{
  /// <summary>
  /// �`��ɕK�v�ȃ��\�[�X�i�V�F�[�_�[�A�e�N�X�`���A�F�Ȃǁj���܂Ƃ߂ĊǗ�����N���X
	/// RenderComponent�ȂǂŎg�p�����B
  /// </summary>
  class Material
  {
  public:
    Material() = default;
    ~Material() = default;

    // �V�F�[�_�[�̐ݒ�E�擾
    void SetShader(Shader* shader) { mShader = shader; }
    Shader* GetShader() const { return mShader; }

    // �e�N�X�`���̐ݒ�E�擾�i�����ł����j
    void AddTexture(Texture* texture) { mTextures.push_back(texture); }
    void SetTexture(size_t index, Texture* texture)
    {
      if (index < mTextures.size()) mTextures[index] = texture;
    }
    Texture* GetTexture(size_t index = 0) const
    {
      if (index < mTextures.size()) return mTextures[index];
      return nullptr;
    }
    size_t GetTextureCount() const { return mTextures.size(); }

    // �F
    void SetBaseColor(const Vector3& color) { mBaseColor = color; }
    const Vector3& GetBaseColor() const { return mBaseColor; }

  private:
    Shader* mShader = nullptr;
    std::vector<Texture*> mTextures;
    Vector3 mBaseColor{ 1.0f, 1.0f, 1.0f }; // �f�t�H�͔�
  };
}
