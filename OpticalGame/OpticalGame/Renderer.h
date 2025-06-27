#pragma once
#include "Window.h"  
#include <d3d11_1.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class Renderer
{
public:
	Renderer(class Game* mGame);
	~Renderer();
	bool Initialize(int screenWidth, int screenHeight);
	void BeginFrame();
	void EndFrame();
	ID3D11Device1* GetDevice() const { return m_device1; }
	ID3D11DeviceContext1* GetDeviceContext() const { return m_deviceContext1; }

private:
	void CreateDevice(Window& window);
	void CreateRenderTarget();

	// Device stuff
	IDXGISwapChain* m_swapChain = nullptr;
	//ID3D11Device* m_device = nullptr;
	//ID3D11DeviceContext* m_deviceContext = nullptr;

	ID3D11Device1* m_device1 = nullptr;
	ID3D11DeviceContext1* m_deviceContext1 = nullptr;

	// Render target view
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	D3D11_TEXTURE2D_DESC m_backBufferDesc{};

	class Game* mGame{};
	class Window* mWindow{};
};