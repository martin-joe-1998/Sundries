#pragma once
#include <iostream>
#include "Component.h"

#include "Graphics/Material.h"
namespace sl
{
  class Actor;

  class Renderer;

  /// <summary>
	/// テクスチャベースの描画コンポーネント。
	/// RenderComponentのMaterialにスプライト用のテクスチャを設定する
  /// コンポーネント生成時にRendererに自信を登録し、RendererのDrawコールで描画する ←依存関係ごちゃごちゃなるのでやめました
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
