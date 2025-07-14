#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

using Microsoft::WRL::ComPtr;

namespace sl
{
	class GraphicsDevice
	{
	public:
		void Initialize(HWND hwnd, int width, int height);
		void Cleanup();

		ID3D11Device* GetDevice() { return mDevice.Get(); }
		ID3D11DeviceContext* GetDeviceContext() { return mDeviceContext.Get(); }
		IDXGISwapChain* GetSwapChain() { return mSwapChain.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return mRenderTargetView.Get(); }
		ID3D11RenderTargetView** GetAddressRenderTargetView() {	return mRenderTargetView.GetAddressOf(); } // Todo: ‚±‚ê‚â‚è‚½‚­‚È‚¢


	private:
		ComPtr<ID3D11Device> mDevice;
		ComPtr<ID3D11DeviceContext> mDeviceContext;
		ComPtr<IDXGISwapChain> mSwapChain;
		ComPtr<ID3D11RenderTargetView> mRenderTargetView;

		int mWindowWidth = 0;
		int mWindowHeight = 0;

		bool CreateDevice(HWND& hwnd, int width, int height);
		bool CreateRenderTarget();
	};
}

