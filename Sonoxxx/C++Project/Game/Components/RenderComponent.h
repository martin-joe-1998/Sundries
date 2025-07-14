#pragma once
#include "Component.h"

#include "Common.h"
#include "Graphics/Material.h"

namespace sl
{
	/// <summary>
	/// 抽象クラスです。ゲームオブジェクトの描画に必要な情報をまとめました。継承して使う
	/// シェーダベースの場合はMeshComponentを継承し、テクスチャベースの場合はSpriteComponentを継承する。
	/// Materialがシェーダやテクスチャを持っているので、派生クラスでいい感じにセットして
	/// 
	/// こいつはいなくなる予定
	/// 
	/// </summary>
	class RenderComponent :	public Component
	{
	public:
		RenderComponent(Actor* owner, const Material& material)
			: Component(owner), mMaterial(material) {	}
		virtual ~RenderComponent() = default;

		/// <summary>
		/// 描画に必要な情報を取得する。
		/// Sceneなどで描画する際に、各RenderComponentからこの情報を取得
		/// 派生先でちゃんとワールド行列を計算して入れてね！！！！！！
		/// </summary>
		virtual DrawDesc GetDrawDesc() const = 0;
		const Material& GetMaterial() const { return mMaterial; }

	private:
		Material mMaterial; // 描画に必要なマテリアル情報（シェーダ、テクスチャなど）
		// MeshやTextureなどのリソースをここで管理することもできる
		// 例えば、MeshComponentならMeshを持つ、SpriteComponentならTextureを持つなど
		// ただし、RenderComponentは抽象クラスなので、具体的な実装は派生クラスで行う
	};
}

