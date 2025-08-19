#include "MeshComponent.h"
#include "Game/Actor.h"
#include "Game/Game.h"
#include "Graphics/Renderer.h"

//#include "Graphics/Mesh.h"

#include "Utility/Logger.h"
#include "Application/Application.h"

namespace sl
{
	MeshComponent::MeshComponent(Actor* owner, MeshType type)
		: Component(owner)
		, mMeshType(type)
	{
		auto renderer = mOwner->GetGame()->GetRenderer();
		mShader = std::make_unique<Shader>();
		mTexture = std::make_unique<Texture>();
		mCBuffer = std::make_unique<ConstantBuffer>(renderer->GetDevice());
		// Cache mesh
		mMesh = std::make_unique<Mesh>();

		CreateMesh(*renderer);
		CreateShader(*renderer);
		CreateTexture(*renderer);
	}

	MeshComponent::~MeshComponent()
	{
		
	}

	void MeshComponent::Update(float deltaTime)
	{
		auto dc = mOwner->GetGame()->GetRenderer()->GetDeviceContext();
		
		switch (mMeshType)
		{
		case ECircle:
			TfCBufferMapping(dc);
			// 1廃暘偺Note偲偄偆偙偲偵偡傞
			ColorCBufferMapping(dc, Vector4(0.f, 2.f, 0.f, 1.f));
			NoteParamCBufferMapping(dc, 360.f);
			break;
		case EPizza:
			TfCBufferMapping(dc);
			PizzaParamCBufferMapping(dc, mSectorNum);
			break;
		case EBaumkuchen:
			TfCBufferMapping(dc);
			ColorCBufferMapping(dc, Vector4(mColor.x /** 20.0f*/, mColor.y /** 20.0f*/, mColor.z /** 20.0f*/, mColor.w)); // Bloom は luminance値が 1.0 以上のピクセルにのみ有効 
			NodeParamCBufferMapping(dc, mAngleOffset, mCurrentRadius);
			break;
		case ELine:
			TfCBufferMapping(dc);
			ColorCBufferMapping(dc, Vector4(mColor.x * 2.0f, mColor.y * 2.0f, mColor.z * 2.0f, mColor.w));
			LineParamCBufferMapping(dc, mLineDir);
			break;
		case EImage:
			TfCBufferMapping(dc);
			ColorCBufferMapping(dc, Vector4(1.f, 1.f, 1.f, 1.f));
			break;
		case EText:
			TfCBufferMapping(dc);
			ColorCBufferMapping(dc, Vector4(0.f, 1.f, 1.f, 1.f));
			break;
			// Spectrum
		case ESpectrum:
			TfCBufferMapping(dc);
			break;
		default:
			
			break;
		}
	
	}

	// Buffer偼Renderer偱帩偭偰偄偰丄Buffer傊偺僙僢僩傪偙偙偱峴偆
	void MeshComponent::Draw(Renderer& renderer)
	{
		auto deviceContext = renderer.GetDeviceContext();

		// Reset vertex & index buffer every frame per actor
		mMesh->SetMeshBuffer(renderer);

		ID3D11Buffer* cbTf = mCBuffer->GetTransFormCBuffer();
		ID3D11Buffer* cbCo = mCBuffer->GetColorCBuffer();
		ID3D11Buffer* cbCi = mCBuffer->GetCircleParamCBuffer();
		ID3D11Buffer* cbNo = mCBuffer->GetNoteParamCBuffer();
		ID3D11Buffer* cbLi = mCBuffer->GetLineParamCBuffer();

		deviceContext->VSSetShader(mShader->GetVertexShader(), nullptr, 0);
		deviceContext->PSSetShader(mShader->GetPixelShader(), nullptr, 0);
		deviceContext->PSSetShaderResources(0, 1, mTexture->GetShaderResourceView());
		deviceContext->PSSetSamplers(0, 1, mTexture->GetSamperState());
		deviceContext->VSSetConstantBuffers(0, 1, &cbTf);
		deviceContext->PSSetConstantBuffers(1, 1, &cbCo);

		switch (mMeshType)
		{
		case ECircle:
			// 1廃暘偺Note偲偄偆偙偲偵偡傞
			deviceContext->PSSetConstantBuffers(2, 1, &cbCi);
			break;
		case EPizza:
			deviceContext->PSSetConstantBuffers(4, 1, &cbCi);
			break;
		case EBaumkuchen:
			deviceContext->PSSetConstantBuffers(5, 1, &cbCi);
			break;
		case ELine:
			deviceContext->PSSetConstantBuffers(3, 1, &cbLi);
			break;
			// Spectrum
		case ESpectrum:
			// Set cbuffer here
			break;
		}

		// Blend State
		// もしオーナー様がピザなら、半透明物体として扱う
		// TODO : CreateBlendState() で Opaque(return nullptr), Trans(半透明), Add(加算) の三種類の BlendState を作り、オーナーに応じて切り替える
		float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		UINT sampleMask = 0xffffffff;
		if (mOwner->GetRenderType() == "Transparent")
		{
			renderer.GetDeviceContext()->OMSetBlendState(renderer.GetBlendState(), blendFactor, sampleMask);
		}

		// Spectrum
		if (mMeshType == ESpectrum)
		{
			deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr); // 不绑定VB
			//deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
			deviceContext->IASetInputLayout(nullptr); // 不绑定InputLayout
			deviceContext->Draw(6 * 256, 0);
		}
		else {
			deviceContext->DrawIndexed(6, 0, 0);
		}

		// Draw が完了したら、Blend stateを元に戻し、不透明物体の DrawCall への汚染を防止
		if (mOwner->GetRenderType() == "Transparent")
		{
			renderer.GetDeviceContext()->OMSetBlendState(nullptr, blendFactor, sampleMask);
		}
	}

	void MeshComponent::CreateMesh(Renderer& renderer)
	{
		//Mesh* mesh = new Mesh();
		//mesh->CreateMesh(renderer);
		//delete mesh;
		mMesh->CreateMesh(renderer);

		// Todo: calc view proj
	}

	void MeshComponent::CreateShader(Renderer& renderer)
	{
		switch (mMeshType)
		{
		case ECircle:
			mShader->Load(renderer.GetDevice(), renderer.GetDeviceContext(), L"Assets/Shaders/Test.shader", "VShader", "PShader");
			break;
		case EPizza:
			mShader->Load(renderer.GetDevice(), renderer.GetDeviceContext(), L"Assets/Shaders/LaneShader.hlsl", "VShader", "PShader");
			break;
		case EBaumkuchen:
			mShader->Load(renderer.GetDevice(), renderer.GetDeviceContext(), L"Assets/Shaders/NodeShader.hlsl", "VShader", "PShader");
			break;
		case ELine:
			mShader->Load(renderer.GetDevice(), renderer.GetDeviceContext(), L"Assets/Shaders/LineShader.hlsl", "VShader", "PShader");
			break;
		case EImage:
			mShader->Load(renderer.GetDevice(), renderer.GetDeviceContext(), L"Assets/Shaders/ImageShader.hlsl", "VShader", "PShader");
			break;
		case EText:
			mShader->Load(renderer.GetDevice(), renderer.GetDeviceContext(), L"Assets/Shaders/ImageShader.hlsl", "VShader", "PShader");
			break;
			// Spectrum
		case ESpectrum:
			mShader->Load(renderer.GetDevice(), renderer.GetDeviceContext(), L"Assets/Shaders/SpectrumShader.hlsl", "VShader", "PShader");
			break;
		default:
			break;
		}
		
	}

	void MeshComponent::SetChar(wchar_t text_)
	{
		mChar = text_;
		CreateTexture(*Game::GetInstance().GetRenderer());
	}

	void MeshComponent::SetImageFileName(std::wstring value_)
	{
		mImageFileName = value_;
		CreateTexture(*Game::GetInstance().GetRenderer());
	}

	void MeshComponent::CreateTexture(Renderer& renderer)
	{
		switch (mMeshType)
		{
		case EImage:
			mTexture->LoadAndCreateSRV(renderer.GetDevice(), L"Assets/Textures/" + mImageFileName + L".png");
			mTexture->CreateSamplarState(renderer.GetDevice());
			break;
		case EText:
			mTexture->LoadAndCreateSRV(renderer.GetDevice(), std::wstring(L"Assets/Textures/Fonts/") + mChar + L".png");
			mTexture->CreateSamplarState(renderer.GetDevice());
			break;
		default:
			break;
		}

	}

	void MeshComponent::TfCBufferMapping(ID3D11DeviceContext* deviceContext)
	{
		// MVP;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(mCBuffer->GetTransFormCBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			LOG_ERROR("TransformCBuffer偺儅僢僺儞僌偵幐攕 hr: {}", hr);
			return;
		}
		ConstantBuffer::TransformCBuffer* tfCBuffer = reinterpret_cast<ConstantBuffer::TransformCBuffer*>(mappedResource.pData);
		float width = Application::GetInstance().GetWindowWidth();
		float height = Application::GetInstance().GetWindowHeight();
		float aspect = width / height;

		Vector3 myeye(0.0f, 0.0f, -1.0f);
		Vector3 myfocus(0.0f, 0.0f, 0.0f);
		Vector3 myup(0.0f, 1.0f, 0.0f);
		Matrix4 worldMt, viewMt, projMt;
		worldMt = mOwner->GetWorldTransform();
		viewMt.LookAt(myeye, myfocus, myup);
		//projMt.Perspective(Math::PiDiv4, aspect, 0.1f, 100.0f);
		
		float viewSize = 1.0f;
		float u = viewSize * 0.5f * aspect;
		float v = viewSize * 0.5f;
		projMt.Orthographic(-u, u, -v, v, 0.1f, 100.0f); // fix:

		tfCBuffer->world = worldMt;
		tfCBuffer->view = viewMt;
		tfCBuffer->projection = projMt;

		deviceContext->Unmap(mCBuffer->GetTransFormCBuffer(), 0);
	}

	void MeshComponent::ColorCBufferMapping(ID3D11DeviceContext* deviceContext, Vector4 color_)
	{
		// color 
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(mCBuffer->GetColorCBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			LOG_ERROR("ColorCBuffer偺儅僢僺儞僌偵幐攕 hr: {}", hr);
			return;
		}

		ConstantBuffer::ColorCBuffer* colorCBuffer = reinterpret_cast<ConstantBuffer::ColorCBuffer*>(mappedResource.pData);
		colorCBuffer->colorChange = color_;
		deviceContext->Unmap(mCBuffer->GetColorCBuffer(), 0);
	}


	void MeshComponent::PizzaParamCBufferMapping(ID3D11DeviceContext* deviceContext, int num_)
	{
		// Theta 
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(mCBuffer->GetCircleParamCBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			LOG_ERROR("CircleCBuffer偺儅僢僺儞僌偵幐攕 hr: {}", hr);
			return;
		}

		ConstantBuffer::PizzaParam* circleCBuffer = reinterpret_cast<ConstantBuffer::PizzaParam*>(mappedResource.pData);
		circleCBuffer->edgeWidth = 0.005f;
		circleCBuffer->innerRadius = 0.3f;// 0.45f;
		circleCBuffer->outerRadius = 0.305f;
		circleCBuffer->laneNumber = (float)num_;
		circleCBuffer->edgeColor = Vector4(1.f, 1.f, 1.f, 1.f);
		circleCBuffer->bgColor = Vector4(0.2f, 0.2f, 0.2f, 0.4f);
		deviceContext->Unmap(mCBuffer->GetCircleParamCBuffer(), 0);
	}

	void MeshComponent::NoteParamCBufferMapping(ID3D11DeviceContext* deviceContext, float theta_)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(mCBuffer->GetCircleParamCBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			LOG_ERROR("CircleCBuffer偺儅僢僺儞僌偵幐攕 hr: {}", hr);
			return;
		}

		ConstantBuffer::NoteParam* noteCBuffer = reinterpret_cast<ConstantBuffer::NoteParam*>(mappedResource.pData);
		noteCBuffer->theta = theta_;
		deviceContext->Unmap(mCBuffer->GetCircleParamCBuffer(), 0);
	}

	void MeshComponent::NodeParamCBufferMapping(ID3D11DeviceContext* deviceContext, float angleOffset_, float radius_)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(mCBuffer->GetCircleParamCBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			LOG_ERROR("CircleCBuffer偺儅僢僺儞僌偵幐攕 hr: {}", hr);
			return;
		}

		ConstantBuffer::NodeParam* nodeCBuffer = reinterpret_cast<ConstantBuffer::NodeParam*>(mappedResource.pData);
		nodeCBuffer->width = 0.01f;
		nodeCBuffer->radius = radius_;
		nodeCBuffer->angleOffset = angleOffset_;
		nodeCBuffer->Separation = 8;
		deviceContext->Unmap(mCBuffer->GetCircleParamCBuffer(), 0);
	}

	// lineCB
	void MeshComponent::LineParamCBufferMapping(ID3D11DeviceContext* deviceContext, Vector2 dir_)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(mCBuffer->GetLineParamCBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			LOG_ERROR("LineParamCBuffer偺儅僢僺儞僌偵幐攕 hr: {}", hr);
			return;
		}

		ConstantBuffer::LineParam* lineCBuffer = reinterpret_cast<ConstantBuffer::LineParam*>(mappedResource.pData);
		lineCBuffer->circleCenter = Vector2(0.5f, 0.5f); // uv偱
		lineCBuffer->radius = 0.45f;
		lineCBuffer->direction = dir_;
		lineCBuffer->lineThickness = 0.01f;
		lineCBuffer->padding = Vector2(0.f, 0.f);
		deviceContext->Unmap(mCBuffer->GetLineParamCBuffer(), 0);
	}
}