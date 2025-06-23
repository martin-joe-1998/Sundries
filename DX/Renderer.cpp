#include "Renderer.h"

Renderer::Renderer(Window& window)
{
	CreateDevice(window);
	CreateRenderTarget();
}

void Renderer::CreateDevice(Window& window)
{
	// define swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1; // double buffering
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // color format
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // usage
	swapChainDesc.OutputWindow = window.GetHandle(); // window handle
	swapChainDesc.SampleDesc.Count = 1; // no multisampling
	swapChainDesc.Windowed = TRUE; // windowed mode

	// Create the swap chain device and context
	auto result = D3D11CreateDeviceAndSwapChain(
		nullptr, // default adapter
		D3D_DRIVER_TYPE_HARDWARE, // hardware driver
		nullptr, // no software device
		0, // flags
		nullptr, // feature levels
		0, // number of feature levels
		D3D11_SDK_VERSION, // SDK version
		&swapChainDesc, // swap chain description
		&m_swapChain, // swap chain pointer
		&m_device, // device pointer
		nullptr, // feature level (not used)
		&m_deviceContext // context pointer
	);

	// Check for errors
	if (result != S_OK)
	{
		MessageBox(nullptr, "Problem Creating DX11", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}
}

void Renderer::CreateRenderTarget()
{
	ID3D11Texture2D* backBuffer = nullptr;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
	backBuffer->Release(); // release the back buffer texture
}

void Renderer::BeginFrame()
{
	// Set the render target to the back buffer
	// m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	// Set the viewport
	// D3D11_VIEWPORT viewport = {};
	// viewport.TopLeftX = 0;
	// viewport.TopLeftY = 0;
	// viewport.Width = 800;
	// viewport.Height = 600;
	// viewport.MinDepth = 0.0f;
	// viewport.MaxDepth = 1.0f;
	// m_deviceContext->RSSetViewports(1, &viewport);

	// Set the background color
	// float red = (float)(rand() % 256) / 255.0f; // random red value
	float clearColor[4] = { 0.3f, 0.4f, 0.5f, 1.0f };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
}

void Renderer::EndFrame()
{
	// Swap the buffers
	m_swapChain->Present(1, 0); // present the frame with vsync
}