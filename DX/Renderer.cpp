#include "Renderer.h"

Renderer::Renderer(Window& window)
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
	}
}