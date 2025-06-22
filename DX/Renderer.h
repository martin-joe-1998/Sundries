#pragma once
#include "Window.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

class Renderer 
{
public:
	Renderer(Window& window);

private:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	void Initialize(Window& window);
	void CreateSwapChain(Window& window);
	void CreateRenderTargetView();
	void ClearScreen(float r, float g, float b, float a);
	void Present();
};