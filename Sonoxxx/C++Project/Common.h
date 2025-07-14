#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <Utility/Math.h>

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"

namespace sl
{
	struct BaseDesc
	{
	};

	// SEの再生位置、何をどこに描画するか、Sceneの状態など、
	struct GameDesc : public BaseDesc
	{
		// SceneState {遷移するか、ポーズかとか}
		// AudioPlayState {SEの再生位置、BGMの再生状態など}
		// RendererDesc Renderer; // 描画に関する設定
		// InputManagerDesc Input; // 入力に関する設定
		// ResourceManagerDesc Resource; // リソースに関する設定
		// UIManagerDesc UI; // UIに関する設定

		bool isQuit = false; // ゲーム終了フラグ
	};

	/// <summary>
	/// １つのゲームオブジェクトが描画に関して必要な情報をまとめた構造体。
	/// </summary>
	struct DrawDesc : public BaseDesc
	{
		Matrix4 worldMatrix;
		const Material* material;
		const Mesh* mesh;
	};
}