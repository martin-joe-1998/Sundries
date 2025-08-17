#include "Renderer.h"
#include "../Component/SpriteComponent.h"
#include "../Graphics/CBuffer.h"
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
	m_CBuffer = std::make_shared<CBuffer>();
}

Renderer::~Renderer()
{
	if (m_swapChain) m_swapChain->Release();				m_swapChain = nullptr;
	if (m_device1) m_device1->Release();					m_device1 = nullptr;
	if (m_deviceContext1) m_deviceContext1->Release();		m_deviceContext1 = nullptr;
	if (m_renderTargetView) m_renderTargetView->Release();	m_renderTargetView = nullptr;
	if (m_depthStencil) m_depthStencil->Release();			m_depthStencil = nullptr;
	if (m_depthStencilView) m_depthStencilView->Release();	m_depthStencilView = nullptr;
	if (m_depthStencilState) m_depthStencilState->Release(); m_depthStencilState = nullptr;
	if (m_blendState) m_blendState->Release();				m_blendState = nullptr;

	if (m_postProcessTexture) m_postProcessTexture->Release(); m_postProcessTexture = nullptr;
	if (m_postProcessRTV) m_postProcessRTV->Release(); m_postProcessRTV = nullptr;
	if (m_postProcessSRV) m_postProcessSRV->Release(); m_postProcessSRV = nullptr;
	if (m_fullscreenQuadVB) m_fullscreenQuadVB->Release(); m_fullscreenQuadVB = nullptr;
	if (m_fullscreenQuadIB) m_fullscreenQuadIB->Release(); m_fullscreenQuadIB = nullptr;
	if (m_postProcessVS) m_postProcessVS->Release(); m_postProcessVS = nullptr;
	if (m_postProcessPS) m_postProcessPS->Release(); m_postProcessPS = nullptr;
	if (m_postProcessInputLayout) m_postProcessInputLayout->Release(); m_postProcessInputLayout = nullptr;
	if (m_postProcessSampler) m_postProcessSampler->Release(); m_postProcessSampler = nullptr;
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
	CreateCBuffer();

	// Post-processing
	CreatePostProcessResources();

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

void Renderer::CreateCBuffer()
{
	HRESULT hr;
#define CREATE_CBUFFER(Type) \
    hr = m_CBuffer->CreateConstantBuffer<Type>(*this); \
    if (FAILED(hr)) { \
        printf("Fail to create " #Type " constant buffer.\n"); \
        exit(0); \
    }

	CREATE_CBUFFER(CBuffer::CBufferPermanent);
	CREATE_CBUFFER(CBuffer::CBufferEveryFrame);
	CREATE_CBUFFER(CBuffer::CBufferLightRay);
	CREATE_CBUFFER(CBuffer::CBufferMirror);
	CREATE_CBUFFER(CBuffer::CBufferColor);
	CREATE_CBUFFER(CBuffer::CBufferScale);
	CREATE_CBUFFER(CBuffer::CBufferTileIllumination);

	// 変わらないCBufferの初期設定を行う
	Vector3 Eye = Vector3(0.0f, 0.0f, -10.0f);
	Vector3 At = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 Up = Vector3::UnitY;
	auto mViewMatrix = Matrix4::CreateLookAt(Eye, At, Up);

	auto mProjectionMatrix = Matrix4::CreateOrtho(
		static_cast<float>(m_backBufferDesc.Width),
		static_cast<float>(m_backBufferDesc.Height),
		0.1f, 100.0f
	);

	CBuffer::CBufferPermanent cbPermanent;
	cbPermanent.viewMx = Matrix4::Transpose(mViewMatrix);
	cbPermanent.projectionMx = Matrix4::Transpose(mProjectionMatrix);
	m_deviceContext1->UpdateSubresource(m_CBuffer->GetConstantBufferr<CBuffer::CBufferPermanent>(), 0, nullptr, &cbPermanent, 0, 0);
}

void Renderer::CreatePostProcessResources()
{
	// 1. 创建后处理用的Texture2D
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = m_backBufferDesc.Width;
	desc.Height = m_backBufferDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = m_device1->CreateTexture2D(&desc, nullptr, &m_postProcessTexture);
	if (FAILED(hr)) { 
		MessageBox(nullptr, L"Failed to create Texture2D for Post-processing.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// 2. 创建RTV
	hr = m_device1->CreateRenderTargetView(m_postProcessTexture, nullptr, &m_postProcessRTV);
	if (FAILED(hr)) { 
		MessageBox(nullptr, L"Failed to create RTV for Post-processing.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// 3. 创建SRV
	hr = m_device1->CreateShaderResourceView(m_postProcessTexture, nullptr, &m_postProcessSRV);
	if (FAILED(hr)) { 
		MessageBox(nullptr, L"Failed to create SRV for Post-processing.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// 4. 创建全屏Quad的VB/IB
	// ...（此处可用标准的全屏quad顶点和索引数据，后续可详细展开）
	// 顶点数据（左下、左上、右上、右下）
	Mesh::FullscreenQuadVertex quadVertices[4] = {
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} }, // 左下
		{ {-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f} }, // 左上
		{ { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f} }, // 右上
		{ { 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f} }  // 右下
	};

	// 索引数据（两个三角形）
	UINT quadIndices[6] = { 0, 1, 2, 0, 2, 3 };

	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(quadVertices);
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = quadVertices;

	hr = m_device1->CreateBuffer(&vbDesc, &vbData, &m_fullscreenQuadVB);
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create fullscreen quad VB.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// 创建索引缓冲区
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(quadIndices);
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = quadIndices;

	hr = m_device1->CreateBuffer(&ibDesc, &ibData, &m_fullscreenQuadIB);
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create fullscreen quad IB.", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// 5. 加载/编译后处理用的VS/PS
	// ...（此处可用D3DCompileFromFile等加载编译shader，后续可详细展开）
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// 编译VS
	hr = D3DCompileFromFile(
		L"Shaders/PostProcessShader.fxh", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vsBlob, &errorBlob);

	if (FAILED(hr)) {
		if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); }
		MessageBox(nullptr, L"Failed to compile PostProcessVS.hlsl", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	hr = m_device1->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_postProcessVS);
	if (FAILED(hr)) { vsBlob->Release(); MessageBox(nullptr, L"Failed to create PostProcess VS", L"Error", MB_OK | MB_ICONERROR); exit(0); }

	// 编译PS
	hr = D3DCompileFromFile(
		L"Shaders/PostProcessShader.fxh", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &psBlob, &errorBlob);

	if (FAILED(hr)) {
		if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); }
		MessageBox(nullptr, L"Failed to compile PostProcessPS.hlsl", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	hr = m_device1->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_postProcessPS);
	if (FAILED(hr)) { psBlob->Release(); MessageBox(nullptr, L"Failed to create PostProcess PS", L"Error", MB_OK | MB_ICONERROR); exit(0); }

	// 6. 创建InputLayout（与Mesh::FullscreenQuadVertex一致）
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = m_device1->CreateInputLayout(
		layout, ARRAYSIZE(layout),
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
		&m_postProcessInputLayout);

	vsBlob->Release();
	psBlob->Release();

	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create PostProcess InputLayout", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// 7. Sampler
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_device1->CreateSamplerState(&sampDesc, &m_postProcessSampler);
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create PostProcess SamplerState", L"Error", MB_OK | MB_ICONERROR);
		exit(0);
	}
}

void Renderer::BeginFrame()
{
	// Bind render target
	//m_deviceContext1->OMSetRenderTargets(1, &m_renderTargetView, nullptr); // no depth stencil view
	m_deviceContext1->OMSetRenderTargets(1, &m_postProcessRTV, m_depthStencilView);
	m_deviceContext1->OMSetDepthStencilState(m_depthStencilState, 0);

	// Set the viewport
	//CD3D11_VIEWPORT viewport = {};
	//viewport.TopLeftX = 0.f;
	//viewport.TopLeftY = 0.f;
	//viewport.Width = (float)m_backBufferDesc.Width;
	//viewport.Height = (float)m_backBufferDesc.Height;
	//viewport.MinDepth = 0.0f;
	//viewport.MaxDepth = 1.0f;
	//m_deviceContext1->RSSetViewports(1, &viewport);

	// Set the background color
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_deviceContext1->ClearRenderTargetView(m_postProcessRTV, clearColor);

	// Clear the depth buffer to 1.0 (max depth)
	m_deviceContext1->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::EndFrame()
{
	// 1. 切换回backBuffer作为渲染目标
	m_deviceContext1->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	CD3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.Width = (float)m_backBufferDesc.Width;
	viewport.Height = (float)m_backBufferDesc.Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_deviceContext1->RSSetViewports(1, &viewport);

	// 2. 设置后处理shader和输入布局
	m_deviceContext1->IASetInputLayout(m_postProcessInputLayout);
	m_deviceContext1->VSSetShader(m_postProcessVS, nullptr, 0);
	m_deviceContext1->PSSetShader(m_postProcessPS, nullptr, 0);

	// 3. 设置全屏quad的VB/IB
	UINT stride = sizeof(Mesh::FullscreenQuadVertex);
	UINT offset = 0;
	m_deviceContext1->IASetVertexBuffers(0, 1, &m_fullscreenQuadVB, &stride, &offset);
	m_deviceContext1->IASetIndexBuffer(m_fullscreenQuadIB, DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext1->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 4. 设置后处理SRV为PS输入
	m_deviceContext1->PSSetShaderResources(0, 1, &m_postProcessSRV);
	// 设置后处理 Sampler
	m_deviceContext1->PSSetSamplers(0, 1, &m_postProcessSampler);

	// 5. 绘制全屏quad
	m_deviceContext1->DrawIndexed(6, 0, 0);

	// 6. 清理SRV绑定，防止Present报错
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	m_deviceContext1->PSSetShaderResources(0, 1, nullSRV);

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

void Renderer::ClearSprites() 
{ 
	for (auto sprite : mSprites) {
		delete sprite;
		sprite = nullptr;
	}

	mSprites.clear(); 
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

//void Renderer::RemoveSprite(SpriteComponent* sprite)
//{
//	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
//	
//	mSprites.erase(iter);
//}