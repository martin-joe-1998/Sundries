#include "Renderer.h"
#include "../Component/SpriteComponent.h"
#include <iostream>

Renderer::Renderer(Game* game)
	: mGame(game)
	, m_swapChain(nullptr)
	, m_device1(nullptr)
	, m_deviceContext1(nullptr)
	, m_renderTargetView(nullptr)
	, m_depthStencil(nullptr)
	, m_depthStencilView(nullptr)
	, m_depthStencilState(nullptr)
	, m_blendState(nullptr)
	, mWindow(nullptr)
{
}

Renderer::~Renderer()
{
	if (m_swapChain) m_swapChain->Release(); 
	if (m_device1) m_device1->Release();
	if (m_deviceContext1) m_deviceContext1->Release();
	if (m_renderTargetView) m_renderTargetView->Release();
	if (m_depthStencil) m_depthStencil->Release();
	if (m_depthStencilView) m_depthStencilView->Release();
	if (m_depthStencilState) m_depthStencilState->Release();
	if (m_blendState) m_blendState->Release();
}

bool Renderer::Initialize(int screenWidth, int screenHeight)
{
	mWindowWidth = screenWidth;
	mWindowHeight = screenHeight;
	
	// Create the window
	mWindow = new Window(mWindowWidth, mWindowHeight);
	
	if (!mWindow)
	{
		std::cerr << "Failed to create window." << std::endl;
		return false;
	}

	// Create the device and swap chain
	CreateDevice(*mWindow);
	CreateRenderTarget();
	CreateDepthStencil();
	CreateBlendState();

	return true;
}

void Renderer::CreateDevice(Window& window)
{
	// define swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;								  // double buffering
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // color format
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // usage
	swapChainDesc.OutputWindow = window.GetHandle();			  // window handle
	swapChainDesc.SampleDesc.Count = 1;							  // no multisampling
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;							      // windowed mode

	// create the base version of device and swap chain
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;

	// Create the swap chain device and context
	auto result = D3D11CreateDeviceAndSwapChain(
		nullptr,					// default adapter
		D3D_DRIVER_TYPE_HARDWARE,	// hardware driver
		nullptr,					// no software device
		0,							// flags
		nullptr,					// feature levels
		0,							// number of feature levels
		D3D11_SDK_VERSION,			// SDK version
		&swapChainDesc,				// swap chain description
		&m_swapChain,				// swap chain pointer
		&m_device,					// device pointer
		&mSelectedFeatureLevel,		// feature level (not used)
		&m_deviceContext			// context pointer
	);

	// Currently, Feaature Level 11.0(shader model 5.1) is selected
	switch (mSelectedFeatureLevel)
	{
	case D3D_FEATURE_LEVEL_11_1: printf("Using D3D_FEATURE_LEVEL_11_1\n"); break;
	case D3D_FEATURE_LEVEL_11_0: printf("Using D3D_FEATURE_LEVEL_11_0\n"); break;
	case D3D_FEATURE_LEVEL_10_1: printf("Using D3D_FEATURE_LEVEL_10_1\n"); break;
	case D3D_FEATURE_LEVEL_10_0: printf("Using D3D_FEATURE_LEVEL_10_0\n"); break;
	default: printf("Using a feature level under 10_0\n"); break;
	}

	// Check for errors
	if (result != S_OK)
	{
		MessageBox(nullptr, L"Problem Creating DX11", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// Upgrade to dx11.1
	m_device->QueryInterface(__uuidof(ID3D11Device1), (void**)&m_device1);
	m_deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&m_deviceContext1);

	// release the original device
	m_device->Release();
	m_deviceContext->Release();
}

void Renderer::CreateRenderTarget()
{
	ID3D11Texture2D* backBuffer = nullptr;

	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr) || backBuffer == nullptr)
	{
		MessageBox(nullptr, L"Failed to get back buffer from swap chain.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	hr = m_device1->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create render target view.", L"Error", MB_OK | MB_ICONERROR);
		backBuffer->Release();
		exit(0);
	}

	backBuffer->GetDesc(&m_backBufferDesc); // get the back buffer description
	backBuffer->Release(); // release the back buffer texture
}

void Renderer::CreateDepthStencil()
{
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = m_backBufferDesc.Width;
	depthDesc.Height = m_backBufferDesc.Height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	HRESULT hr = m_device1->CreateTexture2D(&depthDesc, nullptr, &m_depthStencil);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create depth stencil texture.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = depthDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = m_device1->CreateDepthStencilView(m_depthStencil, &dsvDesc, &m_depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create depth stencil view.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	hr = m_device1->CreateDepthStencilState(&dsDesc, &m_depthStencilState);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create depth stencil state.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}
}

void Renderer::CreateBlendState()
{
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_device1->CreateBlendState(&blendDesc, &m_blendState);
}

void Renderer::BeginFrame()
{
	// Bind render target
	//m_deviceContext1->OMSetRenderTargets(1, &m_renderTargetView, nullptr); // no depth stencil view
	m_deviceContext1->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	m_deviceContext1->OMSetDepthStencilState(m_depthStencilState, 0);

	// Set the viewport
	CD3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.Width = (float)m_backBufferDesc.Width;
	viewport.Height = (float)m_backBufferDesc.Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_deviceContext1->RSSetViewports(1, &viewport);

	// Set the background color
	// float red = (float)(rand() % 256) / 255.0f; // random red value
	float clearColor[4] = { 0.3f, 0.4f, 0.5f, 1.0f };
	m_deviceContext1->ClearRenderTargetView(m_renderTargetView, clearColor);

	// Clear the depth buffer to 1.0 (max depth)
	m_deviceContext1->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::EndFrame()
{
	// Swap the buffers
	m_swapChain->Present(1, 0); // present the frame with vsync
}

void Renderer::Draw()
{
	for (auto sprite : mSprites)
	{
		if (sprite->GetVisible())
		{
			sprite->Draw(*this);
		}
	}
}

void Renderer::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}