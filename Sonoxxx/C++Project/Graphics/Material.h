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
  /// 描画に必要なリソース（シェーダー、テクスチャ、色など）をまとめて管理するクラス
	/// RenderComponentなどで使用される。
  /// </summary>
  class Material
  {
  public:
    Material() = default;
    ~Material() = default;

    // シェーダーの設定・取得
    void SetShader(Shader* shader) { mShader = shader; }
    Shader* GetShader() const { return mShader; }

    // テクスチャの設定・取得（複数できるよ）
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

    // 色
    void SetBaseColor(const Vector3& color) { mBaseColor = color; }
    const Vector3& GetBaseColor() const { return mBaseColor; }

  private:
    Shader* mShader = nullptr;
    std::vector<Texture*> mTextures;
    Vector3 mBaseColor{ 1.0f, 1.0f, 1.0f }; // デフォは白
  };
}
