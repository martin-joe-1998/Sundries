#pragma once
#include "Window.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

class Renderer 
{
public:
	Renderer(Window& window);
	void BeginFrame();
	void EndFrame();
	ID3D11Device* GetDevice() const { return m_device; }
	ID3D11DeviceContext* GetDeviceContext() const { return m_deviceContext; }

private:
	void CreateDevice(Window& window);
	void CreateRenderTarget();

	// Device stuff
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	
	// Render target view
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
};