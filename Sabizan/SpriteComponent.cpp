#include "SpriteComponent.h"
#include "../Actor/Actor.h"
#include "../Game.h"
#include "../Graphics/Renderer.h"

SpriteComponent::SpriteComponent(Actor* owner, const std::string& textureFileName, const std::string& shaderFileName, int drawOrder)
	:Component(owner)
	, mDrawOrder(drawOrder)
	, mTexWidth(0)
	, mTexHeight(0)
	, mVisible(true)
{
	mOwner->GetGame()->GetRenderer()->AddSprite(this);

    // 半透明物体の描画順は不透明物体(100)より後
    if (mOwner->GetType() == "LightRayActor")
    {
        mDrawOrder = 101;
    }

    // Get owner's transform, but do not change or update it in this class
    //m_transform = std::make_unique<Transform>(mOwner->GetTransform());

    m_texture = std::make_unique<Texture>(textureFileName);
    m_CBuffer = std::make_unique<CBuffer>();
    m_shader = std::make_unique<Shader>(shaderFileName);

	auto renderer = mOwner->GetGame()->GetRenderer();
	CreateMesh(*renderer);
	CreateShaders(*renderer);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->GetRenderer()->RemoveSprite(this);
}

void SpriteComponent::Draw(Renderer& renderer)
{
    auto deviceContext = renderer.GetDeviceContext();

    // Update the constant buffer with the world matrices
    CBuffer::CBufferEveryFrame cbEveryFrame;

    // Set worldMatrix
    // mWorldTransformMatrix in actor class will be recalculated(if necessary) every frame in Actor::Update()
    auto mWorldMatrix = mOwner->GetWorldTransform();
    cbEveryFrame.worldMx = Matrix4::Transpose(mWorldMatrix);
    deviceContext->UpdateSubresource(
        m_CBuffer->GetCBufferEveryFrame(),
        0,
        nullptr,
        &cbEveryFrame,
        0,
        0
    );

    ID3D11Buffer* _cbPermanent = m_CBuffer->GetCBufferPermanent();
    ID3D11Buffer* _cbPerFrame = m_CBuffer->GetCBufferEveryFrame();

    // Set the rectangle shader
    deviceContext->VSSetShader(m_shader->GetVertexShader(), nullptr, 0);
    deviceContext->VSSetConstantBuffers(0, 1, &_cbPermanent);
    deviceContext->VSSetConstantBuffers(1, 1, &_cbPerFrame);
    deviceContext->PSSetShader(m_shader->GetPixelShader(), nullptr, 0);

    if (mOwner->GetType() == "LightRayActor")
    {
        CBuffer::CBufferLightRay cbLightRay;
        cbLightRay.lightRayFlag = 1; // Set flag to 1 is horizontal, 2 is vertical
        cbLightRay.padding = Vector3(0.0, 0.0, 0.0);
        deviceContext->UpdateSubresource(
            m_CBuffer->GetCBufferLightRay(),
            0,
            nullptr,
            &cbLightRay,
            0,
            0
        );

        ID3D11Buffer* _cbLightRay = m_CBuffer->GetCBufferLightRay();
        deviceContext->PSSetConstantBuffers(2, 1, &_cbLightRay);
    }

    // Set the texture and sampler state
    ID3D11ShaderResourceView* srv = m_texture->GetShaderResourceView();
    ID3D11SamplerState* sampler = m_texture->GetSampleState();
    deviceContext->PSSetShaderResources(0, 1, &srv);
    deviceContext->PSSetSamplers(0, 1, &sampler);

    // 光線Actorにだけ、BlendStateを設定して、半透明描画を可能にする
    if (mOwner->GetType() == "LightRayActor")
    {
        float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        UINT sampleMask = 0xffffffff;
        renderer.GetDeviceContext()->OMSetBlendState(renderer.GetBlendState(), blendFactor, sampleMask);
    }

    // Draw the rectangle
    deviceContext->DrawIndexed(6, 0, 0); // 6 vertices, starting at index 0
}


void SpriteComponent::CreateMesh(Renderer& renderer)
{
    // ----------------------------- Mesh -----------------------------
    // Create Rectangle Mesh
    Mesh* mesh = new Mesh();
    mesh->CreateMesh(renderer);
    delete mesh;


    // ----------------------------- Constant Buffer -----------------------------
    HRESULT hr = m_CBuffer->CreateConstantBuffer(renderer);
    if (FAILED(hr))
    {
        printf("Fail to create constant buffer.");
        exit(0);
    }

    if (mOwner->GetType() == "LightRayActor")
    {
        hr = m_CBuffer->CreateConstantBufferLightRay(renderer);
        if (FAILED(hr))
        {
            printf("Fail to create lightRay constant buffer.");
            exit(0);
        }
    }

    // ----------------------------- Texture -----------------------------
    // Load & create Texture and Sample state
    hr = m_texture->CreateTextureAndSampleState(renderer);
    if (FAILED(hr))
    {
        printf("Fail to create texture and sample state.");
        exit(0);
    }


    // ----------------------------- Matrix -----------------------------
    // Initialize the world, view, and projection matrices
    // m_transform->SetPosition(Vector3::Zero);   // World Space Position
    // m_transform->SetPosition(
    //     Vector3(-(renderer.GetWidth()) / 2, renderer.GetHeight() / 2, 0.0f)
    // );
    // m_transform->SetRotation(Vector3::Zero);   // Degrees
    // m_transform->SetScale(m_transform->GetScale() * Vector3(1.0f, 1.0f, 1.0f)); // Meet with the screen size, keep scaleZ 1.0
    //mWorldMatrix = m_transform->ComputeWorldMatrix();

    Vector3 Eye = Vector3(0.0f, 0.0f, -1.0f);
    Vector3 At = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 Up = Vector3::UnitY;
    auto mViewMatrix = Matrix4::CreateLookAt(Eye, At, Up);

    auto mProjectionMatrix = Matrix4::CreateOrtho(
        static_cast<float>(renderer.GetWidth()),
        static_cast<float>(renderer.GetHeight()),
        0.1f, 100.0f
    );

    CBuffer::CBufferPermanent cbPermanent;
    cbPermanent.viewMx = Matrix4::Transpose(mViewMatrix);
    cbPermanent.projectionMx = Matrix4::Transpose(mProjectionMatrix);
    renderer.GetDeviceContext()->UpdateSubresource(m_CBuffer->GetCBufferPermanent(), 0, nullptr, &cbPermanent, 0, 0);
}

void SpriteComponent::CreateShaders(Renderer& renderer)
{
    // ----------------------------- Input Layout -----------------------------
    // Define the input layout
    // { LPCSTR SemanticName, 
    //	 UINT SemanticIndex,	(TEXCOORD0, TEXCOORD1...)
    //	 DXGI_FORMAT Format, 
    //	 UINT InputSlot,		(Which vertex buffer?)
    //	 UINT AlignedByteOffset, (D3D11_APPEND_ALIGNED_ELEMENT will do anything for me, woohoo)
    //	 D3D11_INPUT_CLASSIFICATION InputSlotClass, 
    //	 UINT InstanceDataStepRate (Only valid when using instancing) }
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    m_shader->CreatShaderAndInputLayout(renderer, layout, ARRAYSIZE(layout));
}