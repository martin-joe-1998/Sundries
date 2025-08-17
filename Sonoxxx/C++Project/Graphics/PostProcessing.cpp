#include "PostProcessing.h"
#include "Graphics/Renderer.h"
#include "Graphics/Mesh.h"
#include "Data/Config.h"
#include "Utility/Logger.h"

#include <vector>
#include <iostream>

#include <d3d11_1.h> // pass をmarkして、render docでDebugしやすくするためのincludeです
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")

namespace sl
{
	
	PostProcessing::PostProcessing(Renderer* renderer)
        : mRenderer(renderer)
	{
		CalcWeightsTableFromGaussian();
	}

	PostProcessing::~PostProcessing()
	{
        // 各Pass共通
		mFullscreenQuadVB = nullptr;
        mFullscreenQuadIB = nullptr;
        mPostProcessInputLayout = nullptr;
        mPostProcessSampler = nullptr;

		// Bloom 最後の加算用
		mBlendStateAdd = nullptr;

		// Pass 1
        mSceneTexture = nullptr;
        mSceneRTV = nullptr;
        mSceneSRV = nullptr;
        mLuminanceVS = nullptr;
        mLuminancePS = nullptr;

		// Pass 2
		mLuminanceTexture = nullptr;
		mLuminanceRTV = nullptr;
		mLuminanceSRV = nullptr;
		mBlurVS = nullptr;
		mBlurPS = nullptr;

		// Pass 3
		mBlurTexture = nullptr;
		mBlurRTV = nullptr;
		mBlurSRV = nullptr;
		mAddVS = nullptr;
		mAddPS = nullptr;

		// Blur用のcbuffer
		mBlurCbuffer = nullptr;
	}

	void PostProcessing::CreateBlurConstantBuffer()
	{
		auto device = mRenderer->GetDevice();

		CD3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(BlurParam);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;

		HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, mBlurCbuffer.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create blur cbuffer, hr: {}", hr);
		}
	}

    void PostProcessing::CreatePostProcessResources()
    {
		auto& backBufferDesc = mRenderer->GetBackBufferDesc();
		auto device = mRenderer->GetDevice();

		// 1. Post-process 用の Texture2D を作成
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = backBufferDesc.Width;
		desc.Height = backBufferDesc.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		// Pass 1
		HRESULT hr = device->CreateTexture2D(&desc, nullptr, mSceneTexture.GetAddressOf());
		if (FAILED(hr)) {
			LOG_ERROR("Failed to create Texture2D for Luminance Pass, hr: {}", hr);
		}
		// Pass 2
		hr = device->CreateTexture2D(&desc, nullptr, mLuminanceTexture.GetAddressOf());
		if (FAILED(hr)) {
			LOG_ERROR("Failed to create Texture2D for Blur Pass, hr: {}", hr);
		}
		// Pass 3
		hr = device->CreateTexture2D(&desc, nullptr, mBlurTexture.GetAddressOf());
		if (FAILED(hr)) {
			LOG_ERROR("Failed to create Texture2D for Add-Blend Pass, hr: {}", hr);
		}


		// 2. RTV を作成
		// Pass 1
		CreateRTV(mSceneTexture.Get(), nullptr, mSceneRTV.GetAddressOf());
		// Pass 2
		CreateRTV(mLuminanceTexture.Get(), nullptr, mLuminanceRTV.GetAddressOf());
		// Pass 3
		CreateRTV(mBlurTexture.Get(), nullptr, mBlurRTV.GetAddressOf());

		// 3. SRV を作成
		// Pass 1
		CreateSRV(mSceneTexture.Get(), nullptr, mSceneSRV.GetAddressOf());
		// Pass 2
		CreateSRV(mLuminanceTexture.Get(), nullptr, mLuminanceSRV.GetAddressOf());
		// Pass 3
		CreateSRV(mBlurTexture.Get(), nullptr, mBlurSRV.GetAddressOf());

		// 4. フルスクリーン用の Quad の Vertex Buffer と Index Buffer を作成する
		CreateVertexAndIndexBuffer();

		// 5. Load & compile shaders & 6. Create InputLayout
		// Pass 1
		CompileShaderAndInputLayout(L"Assets/Shaders/LuminanceShader.hlsl", "VSMain", "PSMain", mLuminanceVS.GetAddressOf(), mLuminancePS.GetAddressOf());
		// Pass 2
		CompileShaderAndInputLayout(L"Assets/Shaders/GaussianBlurShader.hlsl", "VSMain", "PSMain", mBlurVS.GetAddressOf(), mBlurPS.GetAddressOf());
		// Pass 3 & present
		CompileShaderAndInputLayout(L"Assets/Shaders/Sample2D.hlsl", "VSMain", "PSMain", mAddVS.GetAddressOf(), mAddPS.GetAddressOf());

		// 7. Create Sampler
		// Pass 共通
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampDesc, mPostProcessSampler.GetAddressOf());
		if (FAILED(hr)) {
			LOG_ERROR("Failed to create PostProcess SamplerState, hr: {}", hr);
		}

		// 8. Create blend state
		CreateBlendStateAdd();

		// 9. Create Constant Buffer
		CreateBlurConstantBuffer();
    }

	void PostProcessing::RunPass()
	{
		auto context = mRenderer->GetDeviceContext();
		
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> anno;
		context->QueryInterface(IID_PPV_ARGS(&anno));

		if (anno.Get()) anno.Get()->BeginEvent(L"Bloom - Luminance");
		BeginFrame_Luminance(context);
		Draw_Luminance(context);
		if (anno.Get()) anno.Get()->EndEvent();

		if (anno.Get()) anno.Get()->BeginEvent(L"Bloom - Blur");
		BeginFrame_Blur(context);
		Draw_Blur(context);
		if (anno.Get()) anno.Get()->EndEvent();

		if (anno.Get()) anno.Get()->BeginEvent(L"Bloom - BlendAdd");
		BeginFrame_Add(context);
		Draw_Add(context);
		if (anno.Get()) anno.Get()->EndEvent();

		if (anno.Get()) anno.Get()->BeginEvent(L"Bloom - Present");
		BeginFrame_Present(context);
		Draw_Present(context);
		if (anno.Get()) anno.Get()->EndEvent();
	}

	void PostProcessing::BeginFrame_Luminance(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->OMSetRenderTargets(1, mLuminanceRTV.GetAddressOf(), NULL);

		// Clear RTV
		deviceContext->ClearRenderTargetView(mLuminanceRTV.Get(), Config::Common::BlackBackGround);
	}

	void PostProcessing::Draw_Luminance(ID3D11DeviceContext* deviceContext)
	{
		// 1. Set viewport
		D3D11_VIEWPORT viewport = {};
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<FLOAT>(mRenderer->GetBackBufferDesc().Width);
		viewport.Height = static_cast<FLOAT>(mRenderer->GetBackBufferDesc().Height);
		deviceContext->RSSetViewports(1, &viewport);

		// 2. Set shader & inputLayout buffer
		deviceContext->IASetInputLayout(mPostProcessInputLayout.Get());
		deviceContext->VSSetShader(mLuminanceVS.Get(), nullptr, 0);
		deviceContext->PSSetShader(mLuminancePS.Get(), nullptr, 0);

		// 3. Set Fullscreen quad buffer
		UINT stride = sizeof(Quad);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, mFullscreenQuadVB.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mFullscreenQuadIB.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 4. Set Post-process SRV
		deviceContext->PSSetShaderResources(0, 1, mSceneSRV.GetAddressOf());
		// Set sampler
		deviceContext->PSSetSamplers(0, 1, mPostProcessSampler.GetAddressOf());

		// 5. Draw to the fullscreen quad, with Post-process shader's effect
		deviceContext->DrawIndexed(6, 0, 0);

		// 6. Reset SRV binding
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		deviceContext->PSSetShaderResources(0, 1, nullSRV);
	}

	void PostProcessing::BeginFrame_Blur(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->OMSetRenderTargets(1, mBlurRTV.GetAddressOf(), NULL);
		
		deviceContext->ClearRenderTargetView(mBlurRTV.Get(), Config::Common::BlackBackGround);
	}

	void PostProcessing::Draw_Blur(ID3D11DeviceContext* deviceContext)
	{
		// 1. Set viewport
		D3D11_VIEWPORT viewport = {};
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<FLOAT>(mRenderer->GetBackBufferDesc().Width);
		viewport.Height = static_cast<FLOAT>(mRenderer->GetBackBufferDesc().Height);
		deviceContext->RSSetViewports(1, &viewport);

		// 2. Set shader & inputLayout buffer
		deviceContext->IASetInputLayout(mPostProcessInputLayout.Get());
		deviceContext->VSSetShader(mBlurVS.Get(), nullptr, 0);
		deviceContext->PSSetShader(mBlurPS.Get(), nullptr, 0);

		// 3. Set Fullscreen quad buffer
		UINT stride = sizeof(Quad);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, mFullscreenQuadVB.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mFullscreenQuadIB.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 4. Set Post-process SRV
		deviceContext->PSSetShaderResources(0, 1, mLuminanceSRV.GetAddressOf());
		// Set sampler
		deviceContext->PSSetSamplers(0, 1, mPostProcessSampler.GetAddressOf());
		// Set cbuffer
		deviceContext->UpdateSubresource(
			mBlurCbuffer.Get(),
			0,
			nullptr,
			&mBlurParam,
			0,
			0
		);
		deviceContext->PSSetConstantBuffers(0, 1, mBlurCbuffer.GetAddressOf());

		// 5. Draw to the fullscreen quad, with Post-process shader's effect
		deviceContext->DrawIndexed(6, 0, 0);

		// 6. Reset SRV binding
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		deviceContext->PSSetShaderResources(0, 1, nullSRV);
	}


	void PostProcessing::BeginFrame_Add(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->OMSetRenderTargets(1, mSceneRTV.GetAddressOf(), NULL);

		// mPostProcessRTV には、本来のゲーム画面 texture が保存されているので、加算のためここでクリアしない
	}

	void PostProcessing::Draw_Add(ID3D11DeviceContext* deviceContext)
	{
		// 1. Set viewport
		D3D11_VIEWPORT viewport = {};
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<FLOAT>(mRenderer->GetBackBufferDesc().Width);
		viewport.Height = static_cast<FLOAT>(mRenderer->GetBackBufferDesc().Height);
		deviceContext->RSSetViewports(1, &viewport);

		// 2. Set shader & inputLayout buffer
		deviceContext->IASetInputLayout(mPostProcessInputLayout.Get());
		deviceContext->VSSetShader(mAddVS.Get(), nullptr, 0);
		deviceContext->PSSetShader(mAddPS.Get(), nullptr, 0);

		// 3. Set Fullscreen quad buffer
		UINT stride = sizeof(Quad);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, mFullscreenQuadVB.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mFullscreenQuadIB.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 4. Set Post-process SRV
		deviceContext->PSSetShaderResources(0, 1, mBlurSRV.GetAddressOf());
		// Set sampler
		deviceContext->PSSetSamplers(0, 1, mPostProcessSampler.GetAddressOf());

		// 5. Set blend state add On
		float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		UINT sampleMask = 0xffffffff;
		deviceContext->OMSetBlendState(mBlendStateAdd.Get(), blendFactor, sampleMask);

		// 6. Draw to the fullscreen quad, with Post-process shader's effect
		deviceContext->DrawIndexed(6, 0, 0);

		// 7. Set blend state add Off
		deviceContext->OMSetBlendState(nullptr, blendFactor, sampleMask);

		// 8. Reset SRV binding
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		deviceContext->PSSetShaderResources(0, 1, nullSRV);
	}


	void PostProcessing::BeginFrame_Present(ID3D11DeviceContext* deviceContext)
	{
		auto mainRTV = mRenderer->GetRenderTargetView();
		deviceContext->OMSetRenderTargets(1, &mainRTV, NULL);

		deviceContext->ClearRenderTargetView(mainRTV, Config::Common::BlackBackGround);
	}

	void PostProcessing::Draw_Present(ID3D11DeviceContext* deviceContext)
	{
		// 1. Set viewport
		D3D11_VIEWPORT viewport = {};
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<FLOAT>(mRenderer->GetBackBufferDesc().Width);
		viewport.Height = static_cast<FLOAT>(mRenderer->GetBackBufferDesc().Height);
		deviceContext->RSSetViewports(1, &viewport);

		// 2. Set shader & inputLayout buffer
		deviceContext->IASetInputLayout(mPostProcessInputLayout.Get());
		deviceContext->VSSetShader(mAddVS.Get(), nullptr, 0);
		deviceContext->PSSetShader(mAddPS.Get(), nullptr, 0);

		// 3. Set Fullscreen quad buffer
		UINT stride = sizeof(Quad);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, mFullscreenQuadVB.GetAddressOf(), &stride, &offset);
		deviceContext->IASetIndexBuffer(mFullscreenQuadIB.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 4. Set Post-process SRV
		deviceContext->PSSetShaderResources(0, 1, mSceneSRV.GetAddressOf());
		// Set sampler
		deviceContext->PSSetSamplers(0, 1, mPostProcessSampler.GetAddressOf());

		// 5. Draw to the fullscreen quad, with Post-process shader's effect
		deviceContext->DrawIndexed(6, 0, 0);

		// 6. Reset SRV binding
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		deviceContext->PSSetShaderResources(0, 1, nullSRV);
	}


	void PostProcessing::CreateBlendStateAdd()
	{
		auto device = mRenderer->GetDevice();

		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&blendDesc, &mBlendStateAdd);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create blend state add (HRESULT = 0x" + std::to_string(hr) + ")");
		}
	}

	void PostProcessing::CreateRTV(ID3D11Resource* texture, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc, ID3D11RenderTargetView** rtv)
	{
		auto device = mRenderer->GetDevice();

		HRESULT hr = device->CreateRenderTargetView(texture, rtvDesc, rtv);
		if (FAILED(hr)) {
			LOG_ERROR("Failed to create RTV for Post-processing, hr: {}", hr);
		}
	}

	void PostProcessing::CreateSRV(ID3D11Resource* texture, const D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc, ID3D11ShaderResourceView** srv)
	{
		auto device = mRenderer->GetDevice();

		HRESULT hr = device->CreateShaderResourceView(texture, srvDesc, srv);
		if (FAILED(hr)) {
			LOG_ERROR("Failed to create SRV for Post-processing, hr: {}", hr);
		}
	}

	void PostProcessing::CreateVertexAndIndexBuffer()
	{
		auto device = mRenderer->GetDevice();

		// Vertex
		Quad quadVertices[4] = {
		{ Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f) }, // 左下
		{ Vector3(-1.0f,  1.0f, 0.0f), Vector2(0.0f, 0.0f) }, // 左上
		{ Vector3( 1.0f,  1.0f, 0.0f), Vector2(1.0f, 0.0f) }, // 右上
		{ Vector3( 1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) }  // 右下
		};

		// Index
		UINT quadIndices[6] = { 0, 1, 2, 0, 2, 3 };

		// Create vertex buffer
		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.ByteWidth = sizeof(quadVertices);
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA vbData = {};
		vbData.pSysMem = quadVertices;

		HRESULT hr = device->CreateBuffer(&vbDesc, &vbData, mFullscreenQuadVB.GetAddressOf());
		if (FAILED(hr)) {
			LOG_ERROR("Failed to create fullscreen quad VB, hr: {}", hr);
		}

		// Create index buffer
		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		ibDesc.ByteWidth = sizeof(quadIndices);
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA ibData = {};
		ibData.pSysMem = quadIndices;

		hr = device->CreateBuffer(&ibDesc, &ibData, mFullscreenQuadIB.GetAddressOf());
		if (FAILED(hr)) {
			LOG_ERROR("Failed to create fullscreen quad IB, hr: {}", hr);
		}
	}


	void PostProcessing::CompileShaderAndInputLayout(
		const std::wstring& path, 
		const std::string& vsEntryPoint, 
		const std::string& psEntryPoint, 
		ID3D11VertexShader** vertexShader,
		ID3D11PixelShader** pixelShader)
	{
		auto device = mRenderer->GetDevice();
		ID3DBlob* vsBlob = nullptr;
		ID3DBlob* psBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		// Compile VS
		HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, nullptr, vsEntryPoint.c_str(), "vs_5_0", 0, 0, &vsBlob, &errorBlob);

		if (FAILED(hr)) {
			if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); }
            LOG_ERROR("Failed to compile vs from " + std::string(path.begin(), path.end()) + " (HRESULT = 0x" + std::to_string(hr) + ")");
		}

		hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, vertexShader);
		if (FAILED(hr)) { 
			vsBlob->Release(); 
			LOG_ERROR("Failed to create vs from " + std::string(path.begin(), path.end()) + " (HRESULT = 0x" + std::to_string(hr) + ")");
		}

		// Compile PS
		hr = D3DCompileFromFile(
			path.c_str(), nullptr, nullptr, psEntryPoint.c_str(), "ps_5_0", 0, 0, &psBlob, &errorBlob);

		if (FAILED(hr)) {
			if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); }
			LOG_ERROR("Failed to compile ps from " + std::string(path.begin(), path.end()) + " (HRESULT = 0x" + std::to_string(hr) + ")");
		}

		hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pixelShader);
		if (FAILED(hr)) { 
			psBlob->Release();
			LOG_ERROR("Failed to create ps from " + std::string(path.begin(), path.end()) + " (HRESULT = 0x" + std::to_string(hr) + ")");
		}

		// Create InputLayout
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		hr = device->CreateInputLayout(
			layout, ARRAYSIZE(layout),
			vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
			mPostProcessInputLayout.GetAddressOf());

		vsBlob->Release();
		psBlob->Release();

		if (FAILED(hr)) {
			LOG_ERROR("Failed to create LnputLayout from " + std::string(path.begin(), path.end()) + " (HRESULT = 0x" + std::to_string(hr) + ")");
		}

		return;
	}

	void PostProcessing::CalcWeightsTableFromGaussian()
	{
		// 重みを保存する変数を定義する
		const int sizeOfWeightsTbl = mGaussianRadius + 1;
		//std::vector<float> weightsTbl(sizeOfWeightsTbl, 0.0f);
		float weightsTbl[sizeOfWeightsTbl] = { 0 };
		const int r = sizeOfWeightsTbl - 1;
		const float twoSigma2 = 2.0f * mSigma * mSigma;

		// 重みを計算
		for (int k = 0; k <= r; ++k)
		{
			float d = k * mSampleStep; // 実際の物理距離
			weightsTbl[k] = std::exp(-(d * d) / twoSigma2); // exp(-d^2 / (2σ^2))
		}
		
		float total = weightsTbl[0];
		for (int k = 1; k <= r; ++k) total += 2.0f * weightsTbl[k];

		// Normalize
		if (total > 0.0f)
			for (int k = 0; k <= r; ++k) weightsTbl[k] /= total;

		// 重みデータをCBufferに送る用の変数に入れる
		mBlurParam.weight_0 = Vector4(weightsTbl[0], weightsTbl[1], weightsTbl[2], weightsTbl[3]);
		mBlurParam.weight_1 = Vector4(weightsTbl[4], weightsTbl[5], weightsTbl[6], weightsTbl[7]);
	}
}