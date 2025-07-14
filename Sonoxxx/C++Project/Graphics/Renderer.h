#pragma once
#include <Windows.h>

#include <wrl/client.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "Common.h"
#include "Shader.h"
#include "Mesh.h"

using Microsoft::WRL::ComPtr;

namespace sl
{
	class Game;
	class SpriteComponent;
	// class MeshComponent;

	/// <summary>
	/// DirectX11偺弶婜壔丄昤夋張棟傪娗棟
	/// Todo: GraphicsDevice 傪傗傔傞
	/// Renderer偑SpriteComponentorMeshComponent傪帩偭偰堦婥偵Draw偡傞
	/// </summary>
	class Renderer
	{
	public:
		Renderer(Game* game);
		~Renderer();

		void Initialize(/*HWND hwnd, int width, int height*/);
		void Draw(/*const DrawDesc& desc*/);


		ID3D11Device* GetDevice() { return mDevice.Get(); }
		ID3D11DeviceContext* GetDeviceContext() { return mDeviceContext.Get(); }
		IDXGISwapChain* GetSwapChain() { return mSwapChain.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return mRenderTargetView.Get(); }

		void AddSprite(SpriteComponent* sc);
		void RemoveSprite(SpriteComponent* sc);

	private:
		Game* mGame;
		std::vector<SpriteComponent*> mSpriteComponents;

		ComPtr<ID3D11Device> mDevice;
		ComPtr<ID3D11DeviceContext> mDeviceContext;
		ComPtr<IDXGISwapChain> mSwapChain;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView;

		bool CreateDevice(HWND& hwnd, int width, int height);
		bool CreateRenderTarget();

		int mWindowWidth = 0;
		int mWindowHeight = 0;

		void BeginFrame();
		void EndFrame();
	};
}

