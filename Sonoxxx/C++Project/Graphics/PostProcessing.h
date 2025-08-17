#pragma once
#include <string>
#include <Utility/Math.h>

#include <wrl/client.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

using Microsoft::WRL::ComPtr;

namespace sl
{
	class Renderer;
	
	class PostProcessing
	{
	public:
		PostProcessing(Renderer* renderer);
		~PostProcessing();
		void CreatePostProcessResources();
		
		ID3D11RenderTargetView* GetRTV() const { return mSceneRTV.Get(); }
		void RunPass();

	private:
		void BeginFrame_Luminance(ID3D11DeviceContext* deviceContext);
		void Draw_Luminance(ID3D11DeviceContext* deviceContext);

		void BeginFrame_Blur(ID3D11DeviceContext* deviceContext);
		void Draw_Blur(ID3D11DeviceContext* deviceContext);

		void BeginFrame_Add(ID3D11DeviceContext* deviceContext);
		void Draw_Add(ID3D11DeviceContext* deviceContext);

		void BeginFrame_Present(ID3D11DeviceContext* deviceContext);
		void Draw_Present(ID3D11DeviceContext* deviceContext);


		void CreateBlurConstantBuffer();
		void CreateBlendStateAdd();
		void CreateRTV(ID3D11Resource* texture, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, ID3D11RenderTargetView** rtv);
		void CreateSRV(ID3D11Resource* texture, const D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc, ID3D11ShaderResourceView** srv);
		void CreateVertexAndIndexBuffer();
		void CompileShaderAndInputLayout(
			const std::wstring& path, 
			const std::string& vsEntryPoint, 
			const std::string& psEntryPoint, 
			ID3D11VertexShader** vertexShader,
			ID3D11PixelShader** pixelShader
		);

		void CalcWeightsTableFromGaussian();
		// パラメーターの調整テクニック：まずは Radius と mSampleStep を決めてから
		// sigma = (r * sampleStep) / 3.0 で sigma の初期値を決める。そしてsigmaを微調整
		// sigma が小：コントラスト強く、ボケが弱い。 sigma が大：その逆
		static constexpr int mGaussianRadius = 5;
		static constexpr float mSigma = 1.8f;
		static constexpr float mSampleStep = 1.5f;

		// Renderer への参照
		Renderer* mRenderer;
		// 九割自分のせいだけど、朝五時に MeshCompoenent が ConstantBuffer を持っているのを見た時は涙目になった（迫真）
		// 一応ここにも一個持たせることにした
		// Blur処理用のcbuffer
		ComPtr<ID3D11Buffer> mBlurCbuffer = nullptr;

		// Blend State
		ComPtr<ID3D11BlendState> mBlendStateAdd = nullptr;

		// フルスクリーンの Quad の vertex buffer と index buffer
		ComPtr<ID3D11Buffer> mFullscreenQuadVB = nullptr;
		ComPtr<ID3D11Buffer> mFullscreenQuadIB = nullptr;
		// 現在では、共通の InputLayout と Sampler を使用する
		ComPtr<ID3D11InputLayout> mPostProcessInputLayout = nullptr;
		ComPtr<ID3D11SamplerState> mPostProcessSampler = nullptr;

		// 最初のパスでは、各MeshComponentのDrawでここに情報を書き込まれる。
		// LuminanceShader の入力Texture
		ComPtr<ID3D11Texture2D> mSceneTexture = nullptr;
		ComPtr<ID3D11RenderTargetView> mSceneRTV = nullptr;
		// 輝度抽出パスの shader(LuminanceShader) が使用するもの
		ComPtr<ID3D11ShaderResourceView> mSceneSRV = nullptr;
		ComPtr<ID3D11VertexShader> mLuminanceVS = nullptr;
		ComPtr<ID3D11PixelShader> mLuminancePS = nullptr;


		// ブラー処理パス用の変数、LuminanceShader の出力先、MeanBlurShader の入力Texture
		ComPtr<ID3D11Texture2D> mLuminanceTexture = nullptr;
		ComPtr<ID3D11RenderTargetView> mLuminanceRTV = nullptr;
		// ブラー処理パスの shader(MeanBlurShader) が使用するもの
		ComPtr<ID3D11ShaderResourceView> mLuminanceSRV = nullptr;
		ComPtr<ID3D11VertexShader> mBlurVS = nullptr;
		ComPtr<ID3D11PixelShader> mBlurPS = nullptr;

		// ブラー処理後の texture と 元のゲーム画面 texture を加算するパス用の変数
		// mBlurTexture を使って、mAddSRV を作り、shader にセットする
		ComPtr<ID3D11Texture2D> mBlurTexture = nullptr;
		ComPtr<ID3D11RenderTargetView> mBlurRTV = nullptr;
		ComPtr<ID3D11ShaderResourceView> mBlurSRV = nullptr;
		ComPtr<ID3D11VertexShader> mAddVS = nullptr;
		ComPtr<ID3D11PixelShader> mAddPS = nullptr;

		// Pass 1: 輝度抽出
		// Pass 2: Blur
		// Pass 3: Blend-add
		// Pass 4: final present

		public:
			struct BlurParam // 48 bytes
			{
				int radius = mGaussianRadius;
				float sampleStep = mSampleStep;
				Vector2 pad0 = Vector2(0.0f, 0.0f);
				Vector4 weight_0 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
				Vector4 weight_1 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			};
		private:
			BlurParam mBlurParam = {};
	};
}