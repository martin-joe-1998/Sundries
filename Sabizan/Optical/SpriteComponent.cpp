#include "SpriteComponent.h"
#include "../Actor/Actor.h"
#include "../Actor/LightRayActor.h"
#include "../Actor/MirrorActor.h"
#include "../Actor/UIActor.h"
#include "../Actor/FloorActor.h"
#include "../Game.h"
#include "../Stage/Stage.h"
#include "../LightRaySystem.h"
#include "../Graphics/Renderer.h"

SpriteComponent::SpriteComponent(Actor* owner, const std::vector<std::string>& textureFileNames, const std::string& shaderFileName, int drawOrder)
	: Component(owner)
	, mDrawOrder(drawOrder)
	, mTexWidth(0)
	, mTexHeight(0)
	, mVisible(true)
    , mShaderName(shaderFileName)
{
	mOwner->GetGame()->GetRenderer()->AddSprite(this);

    // îºìßñæï®ëÃÇÃï`âÊèáÇÕïsìßñæï®ëÃ(100)ÇÊÇËå„ÇÎ

    for (const auto& fileName : textureFileNames)
    {
        auto texture = std::make_unique<Texture>(fileName);
        m_textures.emplace_back(std::move(texture));
    }

	auto renderer = mOwner->GetGame()->GetRenderer();

    m_shader = std::make_unique<Shader>(shaderFileName);
    mCbufferRef = renderer->GetCBufferRef();

	CreateMesh(*renderer);
	CreateShaders(*renderer);
}

SpriteComponent::~SpriteComponent()
{
	//mOwner->GetGame()->GetRenderer()->RemoveSprite(this);
    // SpriteComponentÇÃéQè∆ÇÕÅAActor ÇÃ mComponents Ç∆ Renderer ÇÃ mSprites ÇÃóºï˚Ç≈éùÇ¡ÇƒÇ¢ÇÈ
    // ÉVÅ[ÉìëJà⁄ÇÃéûÇÕÅARenderer ÇÃ ClearSprites() Ç≈ÉÅÉÇÉäÅ[ä«óùÇçsÇ§
}

void SpriteComponent::Draw(Renderer& renderer)
{
    auto deviceContext = renderer.GetDeviceContext();

    mMesh->SetMeshBuffer(renderer);

    // CbufferÇÃê›íË
    SetCBuffer(deviceContext);

    // Set the texture and sampler state
    for (size_t i = 0; i < m_textures.size(); ++i)
    {
        ID3D11ShaderResourceView* srv = m_textures[i]->GetShaderResourceView();
        ID3D11SamplerState* sampler = m_textures[i]->GetSampleState();
        deviceContext->PSSetShaderResources(static_cast<UINT>(i), 1, &srv);
        deviceContext->PSSetSamplers(static_cast<UINT>(i), 1, &sampler);
    }

    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    UINT sampleMask = 0xffffffff;
    // åıê¸ActorÇ…ÇæÇØÅABlendStateÇê›íËÇµÇƒÅAîºìßñæï`âÊÇâ¬î\Ç…Ç∑ÇÈ
    if (mOwner->GetRenderType() == "Transparent")
    {    
        renderer.GetDeviceContext()->OMSetBlendState(renderer.GetBlendState(), blendFactor, sampleMask);
    }

    // Draw call
    deviceContext->DrawIndexed(6, 0, 0); // 6 vertices, starting at index 0

    // ï`âÊå„Ç…ÅAìßñæï®ëÃÇÃÉuÉåÉìÉhê›íËÇå≥Ç…ñﬂÇ∑
    if (mOwner->GetRenderType() == "Transparent")
    {
        renderer.GetDeviceContext()->OMSetBlendState(nullptr, blendFactor, sampleMask);
    }
}

void SpriteComponent::CreateMesh(Renderer& renderer)
{
    HRESULT hr;
    // ----------------------------- Mesh -----------------------------
    // Create Rectangle Mesh
    //Mesh* mesh = new Mesh();
    //mesh->CreateMesh(renderer);
    //delete mesh;
    mMesh = std::make_unique<Mesh>();
    mMesh->CreateMesh(renderer);


    // ----------------------------- Texture -----------------------------
    // Load & create Texture and Sample state
    for (auto& texture : m_textures)
    {
        hr = texture->CreateTextureAndSampleState(renderer, mOwner->GetType());
        if (FAILED(hr))
        {
            printf("Fail to create texture and sample state.");
            exit(0);
        }
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

    // TODO : §≥§≥§ŒÑI¿Ì§Ú Renderer §À“∆§π!!!
    //Vector3 Eye = Vector3(0.0f, 0.0f, -10.0f);
    //Vector3 At = Vector3(0.0f, 0.0f, 0.0f);
    //Vector3 Up = Vector3::UnitY;
    //auto mViewMatrix = Matrix4::CreateLookAt(Eye, At, Up);
    //
    //auto mProjectionMatrix = Matrix4::CreateOrtho(
    //    static_cast<float>(renderer.GetBackBufferWidth()),
    //    static_cast<float>(renderer.GetBackBufferHeight()),
    //    0.1f, 100.0f
    //);
    //
    //CBuffer::CBufferPermanent cbPermanent;
    //cbPermanent.viewMx = Matrix4::Transpose(mViewMatrix);
    //cbPermanent.projectionMx = Matrix4::Transpose(mProjectionMatrix);
    //renderer.GetDeviceContext()->UpdateSubresource(mCbufferRef->GetConstantBufferr<CBuffer::CBufferPermanent>(), 0, nullptr, &cbPermanent, 0, 0);
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

void SpriteComponent::SetCBuffer(ID3D11DeviceContext1* deviceContext)
{
    // Update the constant buffer with the world matrices
    CBuffer::CBufferEveryFrame cbEveryFrame;

    // Set worldMatrix
    // mWorldTransformMatrix in actor class will be recalculated(if necessary) every frame in Actor::Update()
    auto mWorldMatrix = mOwner->GetWorldTransform();
    cbEveryFrame.worldMx = Matrix4::Transpose(mWorldMatrix);
    deviceContext->UpdateSubresource(
        mCbufferRef->GetConstantBufferr<CBuffer::CBufferEveryFrame>(),
        0,
        nullptr,
        &cbEveryFrame,
        0,
        0
    );

    ID3D11Buffer* _cbPermanent = mCbufferRef->GetConstantBufferr<CBuffer::CBufferPermanent>();
    ID3D11Buffer* _cbPerFrame = mCbufferRef->GetConstantBufferr<CBuffer::CBufferEveryFrame>();

    // Set the Shader
    deviceContext->VSSetShader(m_shader->GetVertexShader(), nullptr, 0);
    deviceContext->VSSetConstantBuffers(0, 1, &_cbPermanent);
    deviceContext->VSSetConstantBuffers(1, 1, &_cbPerFrame);
    deviceContext->PSSetShader(m_shader->GetPixelShader(), nullptr, 0);

    if (mShaderName == "LightRayShader.fxh"/*mOwner->GetType() == "LightRayActor"*/)
    {
        CBuffer::CBufferLightRay cbLightRay;
        LightRayActor* lightRayActor = static_cast<LightRayActor*>(mOwner);

        cbLightRay.lightRayFlag = lightRayActor->GetDirFlag(); // Set flag to 1 is horizontal, 2 is vertical
        cbLightRay.lightColor = lightRayActor->GetLightColor();
        deviceContext->UpdateSubresource(
            mCbufferRef->GetConstantBufferr<CBuffer::CBufferLightRay>(),
            0,
            nullptr,
            &cbLightRay,
            0,
            0
        );

        ID3D11Buffer* _cbLightRay = mCbufferRef->GetConstantBufferr<CBuffer::CBufferLightRay>();
        deviceContext->PSSetConstantBuffers(2, 1, &_cbLightRay);
    }
    else if (mShaderName == "MirrorShader.fxh"/*mOwner->GetType() == "MirrorActor"*/)
    {
        CBuffer::CBufferMirror cbMirror;
        MirrorActor* mirrorActor = static_cast<MirrorActor*>(mOwner);

        cbMirror.rotFlag = mirrorActor->GetRotFlag();
        cbMirror.padding = Vector3(0.0f, 0.0f, 0.0f);
        deviceContext->UpdateSubresource(
            mCbufferRef->GetConstantBufferr<CBuffer::CBufferMirror>(),
            0,
            nullptr,
            &cbMirror,
            0,
            0
        );

        ID3D11Buffer* _cbMirror = mCbufferRef->GetConstantBufferr<CBuffer::CBufferMirror>();
        deviceContext->PSSetConstantBuffers(2, 1, &_cbMirror);
    }
    else if (mShaderName == "RectangleShader.fxh")
    {
        CBuffer::CBufferColor cbColor = CBuffer::CBufferColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        FloorActor* floorActor = static_cast<FloorActor*>(mOwner);

        // è∞ÇîºìßñæÇ…Ç∑ÇÈ
        if (mOwner->GetType() == "FloorActor")
        {
            cbColor.Color.a = floorActor->GetAlpha();
        }

        deviceContext->UpdateSubresource(
            mCbufferRef->GetConstantBufferr<CBuffer::CBufferColor>(),
            0,
            nullptr,
            &cbColor,
            0,
            0
        );

        ID3D11Buffer* _cbColor = mCbufferRef->GetConstantBufferr<CBuffer::CBufferColor>();
        deviceContext->PSSetConstantBuffers(2, 1, &_cbColor);

        // è∆ñæèÓïÒÇÃê›íË
        CBuffer::CBufferTileIllumination cbTI;
        // è∆ñæÉ}ÉbÉvÇíºê⁄éQè∆ÇµÇƒÅAmOwnerÇÃÉ^ÉCÉãç¿ïWÇ©ÇÁé©êgÇÃè∆ñæèÓïÒÇéÊìæ
        auto& IMapRef = mOwner->GetGame()->GetStage()->GetLightRaySystem()->GetIlluminationMapRef();
        auto& coord = mOwner->GetTileCoord();

        cbTI.intensity = IMapRef[coord.h][coord.w].intensity;
        cbTI.color = IMapRef[coord.h][coord.w].lightColor;

        deviceContext->UpdateSubresource(
            mCbufferRef->GetConstantBufferr<CBuffer::CBufferTileIllumination>(),
            0,
            nullptr,
            &cbTI,
            0,
            0
        );
        ID3D11Buffer* _cbTI = mCbufferRef->GetConstantBufferr<CBuffer::CBufferTileIllumination>();
        deviceContext->PSSetConstantBuffers(3, 1, &_cbTI);
    }
    else if (mOwner->GetType() == "UIActor") // UIActor using UIShader or ArrowShader
    {
        CBuffer::CBufferColor cbColor = CBuffer::CBufferColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
        UIActor* uiActor = static_cast<UIActor*>(mOwner);

        cbColor.Color = uiActor->GetFontColor();
        deviceContext->UpdateSubresource(
            mCbufferRef->GetConstantBufferr<CBuffer::CBufferColor>(),
            0,
            nullptr,
            &cbColor,
            0,
            0
        );

        ID3D11Buffer* _cbColor = mCbufferRef->GetConstantBufferr<CBuffer::CBufferColor>();
        deviceContext->PSSetConstantBuffers(2, 1, &_cbColor);
    }
    else if (mShaderName == "TransBoxShader.fxh")
    {
        CBuffer::CBufferScale cbScale;
        cbScale.scale = mOwner->GetScale();
        cbScale.padding = 0.0;

        deviceContext->UpdateSubresource(
            mCbufferRef->GetConstantBufferr<CBuffer::CBufferScale>(),
            0,
            nullptr,
            &cbScale,
            0,
            0
        );

        ID3D11Buffer* _cbScale = mCbufferRef->GetConstantBufferr<CBuffer::CBufferScale>();
        deviceContext->PSSetConstantBuffers(2, 1, &_cbScale);
    }
}