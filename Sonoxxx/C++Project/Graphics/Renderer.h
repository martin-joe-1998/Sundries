#pragma once
#include <Windows.h>
#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <d3d11_1.h> // pass をmarkして、render docでDebugしやすくするためのincludeです
#pragma comment(lib, "d3d11.lib")

#include "Shader.h"
#include "Mesh.h"
#include "PostProcessing.h"

using Microsoft::WRL::ComPtr;

namespace sl
{
	class Game;

	class Renderer
	{
	public:
		Renderer(Game* game);
		~Renderer();

		void Initialize();
		//void Draw(/*const DrawDesc& desc*/);
		void BeginFrame();
		void Draw(); // 管理しやすくするため、全 MeshComponent の DrawCall をここに移した
		void EndFrame();
		void PostProcessPass();


		ID3D11Device* GetDevice() { return mDevice.Get(); }
		ID3D11DeviceContext* GetDeviceContext() { return mDeviceContext.Get(); }
		IDXGISwapChain* GetSwapChain() { return mSwapChain.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return mRenderTargetView.Get(); }
		// Blend State
		ID3D11BlendState* GetBlendState() { return mBlendState.Get(); }
		// Back Buffer
		const D3D11_TEXTURE2D_DESC GetBackBufferDesc() const { return mBackBufferDesc; }

	private:
		Game* mGame;

		ComPtr<ID3D11Device> mDevice;
		ComPtr<ID3D11DeviceContext> mDeviceContext;
		ComPtr<IDXGISwapChain> mSwapChain;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView;

		bool CreateDevice(HWND& hwnd, int width, int height);
		bool CreateRenderTarget();
		// Blend State Trans(src, one minus src)
		bool CreateBlendState();

		// Blend State
		ComPtr<ID3D11BlendState> mBlendState;

		int mWindowWidth = 0;
		int mWindowHeight = 0;

		// Post processing
		std::unique_ptr<PostProcessing> mPostProcess;
		bool mIsActivePostProcess;
		// 初回で mRenderTargetView 作成するときに、その設定をここに保存
		// 現段階では、window sizeや最終パスのcolor formatの取得にしか使わない
		D3D11_TEXTURE2D_DESC mBackBufferDesc{};
	};
}