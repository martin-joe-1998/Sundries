#include "Rectangle.h"
#include "Math.h"
#include "Mesh.h"
#include "Shader.h"
#include "MoveComponent.h"
#include <fstream>
#include <vector>
#include <Windows.h>

Rectangle::Rectangle(Renderer& renderer)
{
    m_transform = std::make_unique<Transform>();
    //mMoveComp = new MoveComponent(this);

    CreateMesh(renderer);
	CreateShaders(renderer);
}

Rectangle::~Rectangle()
{
	if (m_vertexBuffer) m_vertexBuffer->Release();
	if (m_indexBuffer) m_indexBuffer->Release();
	if (m_cBufferPermanent) m_cBufferPermanent->Release();
    if (m_cBufferEveryFrame) m_cBufferEveryFrame->Release();
	if (m_vertexShader) m_vertexShader->Release();
	if (m_pixelShader) m_pixelShader->Release();
	if (m_inputLayout) m_inputLayout->Release();
    if (m_shaderResourceView) m_shaderResourceView->Release();
    if (m_samplerLinear) m_samplerLinear->Release();
}

void Rectangle::Draw(Renderer& renderer)
{
    auto deviceContext = renderer.GetDeviceContext();

	// Update the constant buffer with the world matrices
	Mesh::CBufferEveryFrame cbEveryFrame;

    m_worldMatrix = m_transform->ComputeWorldMatrix();
    cbEveryFrame.worldMx = Matrix4::Transpose(m_worldMatrix);
    deviceContext->UpdateSubresource(
        m_cBufferEveryFrame,
        0,
        nullptr,
        &cbEveryFrame,
        0,
        0
	);

    // Set the rectangle shader
    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    deviceContext->VSSetConstantBuffers(0, 1, &m_cBufferPermanent);
	deviceContext->VSSetConstantBuffers(1, 1, &m_cBufferEveryFrame);
    deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

    // Set the texture and sampler state
    deviceContext->PSSetShaderResources(0, 1, &m_shaderResourceView);
    deviceContext->PSSetSamplers(0, 1, &m_samplerLinear);

    // Draw the rectangle
    deviceContext->DrawIndexed(6, 0, 0); // 6 vertices, starting at index 0
}

void Rectangle::Update(float deltaTime)
{

}

void Rectangle::ProcessInput(float deltaTime, std::vector<KeyState>& keyStates)
{
    //int keys[] = { 'W', 'A', 'S', 'D' };
    // The threshold time between one move and the next
    float moveDelay = 0.8f;

    for (auto& ks : keyStates)
    {
        if (ks.key != 'W' && ks.key != 'A' && ks.key != 'S' && ks.key != 'D')
            continue;

        if (activeKey == 0 || activeKey == ks.key)
        {
            // First been pressed
            if (ks.isPressed && ks.timeSinceLastMove == 0.0f)
            {
                MoveInDirection(ks.key);
                activeKey = ks.key;
            }
            else if (ks.isPressed && ks.timeSinceLastMove >= moveDelay)
            {
                MoveInDirection(ks.key);
                ks.timeSinceLastMove = 0.0f;
            }
            // Key released
            else if (!ks.isPressed && activeKey == ks.key)
            {
                activeKey = 0;
            }
        }
    }
}

void Rectangle::MoveInDirection(int key)
{
    switch (key)
    {
    case 'W':
        m_transform->SetPosition(
            Vector3(
                m_transform->GetPosition().x, 
                m_transform->GetPosition().y + gridSize,  
                m_transform->GetPosition().z
            )
        );
        break;
    case 'S':
        m_transform->SetPosition(
            Vector3(
                m_transform->GetPosition().x,
                m_transform->GetPosition().y - gridSize,
                m_transform->GetPosition().z
            )
        );
        break;
    case 'A':
        m_transform->SetPosition(
            Vector3(
                m_transform->GetPosition().x - gridSize,
                m_transform->GetPosition().y,
                m_transform->GetPosition().z
            )
        );
        break;
    case 'D':
        m_transform->SetPosition(
            Vector3(
                m_transform->GetPosition().x + gridSize,
                m_transform->GetPosition().y,
                m_transform->GetPosition().z
            )
        );
        break;
    default:
        break;
    }
}

void Rectangle::CreateMesh(Renderer& renderer)
{
    auto deviceContext = renderer.GetDeviceContext();

    // ----------------------------- Vertex -----------------------------
    // Define vertices for a rectangle
    Mesh::Vertex vertices[] = {
        { Vector3( 0.0f,  0.0f, 0.0f), Vector2(0.0f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f) }, // Left Up
        { Vector3( 1.0f,  0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f) }, // Right Up
        { Vector3( 1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) }, // Right Down
        { Vector3( 0.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector4(0.3f, 0.4f, 0.5f, 1.0f) }, // Left Down
    };

    // Create vertex buffer
    CD3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0; // No CPU access needed

    D3D11_SUBRESOURCE_DATA vertexData = { };
    vertexData.pSysMem = vertices;

    auto hr = renderer.GetDevice()->CreateBuffer(
        &bufferDesc,
        &vertexData,
        &m_vertexBuffer
    );

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create vertex buffer for rectangle.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Set the Vertex buffer
    UINT stride = sizeof(Mesh::Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);


    // ----------------------------- Indices -----------------------------
    // Define indices for the rectangle (two triangles)
    WORD indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    // Create index buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(WORD) * 6;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    vertexData.pSysMem = indices;

    hr = renderer.GetDevice()->CreateBuffer(
        &bufferDesc,
        &vertexData,
        &m_indexBuffer
    );
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create index buffer for rectangle.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Set the index buffer
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    // Set the primitive topology to triangle list
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// ----------------------------- Constant Buffer -----------------------------
    // Create constant buffer for transformation matrices
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(Mesh::CBufferPermanent);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    hr = renderer.GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_cBufferPermanent);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create cbufferPermanent for rectangle.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    bufferDesc.ByteWidth = sizeof(Mesh::CBufferEveryFrame);
    hr = renderer.GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_cBufferEveryFrame);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create cbufferEveryFrame for rectangle.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }


    // Load the texture
    hr = Shader::CreateTextureFromBMP(renderer.GetDevice(), L"Asset/Textures/Slime.bmp", &m_shaderResourceView);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create texture from bmp file.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Create the sample state
    D3D11_SAMPLER_DESC sampleDesc = {};
    sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampleDesc.MinLOD = 0;
    sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = renderer.GetDevice()->CreateSamplerState(&sampleDesc, &m_samplerLinear);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create samplerState.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Initialize the world, view, and projection matrices
    m_transform->SetRotation(Vector3::Zero);    // Degrees
    m_transform->SetPosition(Vector3::Zero);   // World Space Position
    m_transform->SetScale(m_transform->GetScale() * Vector3(1.0f, 1.0f, 1.0f)); // Meet with the screen size, keep scaleZ 1.0
    m_worldMatrix = m_transform->ComputeWorldMatrix();

    Vector3 Eye = Vector3(0.0f, 0.0f, -1.0f);
    Vector3 At = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 Up = Vector3::UnitY;
    m_viewMatrix = Matrix4::CreateLookAt(Eye, At, Up);

    m_projectionMatrix = Matrix4::CreateOrtho(
        static_cast<float>(renderer.GetWidth()),
        static_cast<float>(renderer.GetHeight()),
        0.1f, 100.0f
    );

    Mesh::CBufferPermanent cbPermanent;
    cbPermanent.viewMx = Matrix4::Transpose(m_viewMatrix);
    cbPermanent.projectionMx = Matrix4::Transpose(m_projectionMatrix);
    renderer.GetDeviceContext()->UpdateSubresource(m_cBufferPermanent, 0, nullptr, &cbPermanent, 0, 0);
}

void Rectangle::CreateShaders(Renderer& renderer)
{
	HRESULT hr = S_OK;

	// ----------------------------- Vertex Shader -----------------------------
    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
	D3D_FEATURE_LEVEL featureLevel = renderer.GetFeatureLevel();
    const char* vshaderModel = Shader::GetShaderModelForFeatureLevel(featureLevel, "vs");
    
	hr = Shader::CompileShaderFromFile(
        L"RectangleShader.fxh",
        "VSMain", vshaderModel, &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to compile vertex shader.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
	}

	// Create the vertex shader
    hr = renderer.GetDevice()->CreateVertexShader(
        pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(),
        nullptr,
        &m_vertexShader
	);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        MessageBox(nullptr, L"Failed to create vertex shader.", L"Error", MB_OK);
		exit(0);
    }

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

	// Create the input layout
    hr = renderer.GetDevice()->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(),
        &m_inputLayout
	);
	// Release the vertex shader blob as it's no longer needed
    pVSBlob->Release();

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create input layout.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
	}

	// Set the input layout
	renderer.GetDeviceContext()->IASetInputLayout(m_inputLayout);


    // ----------------------------- Pixel Shader -----------------------------
	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    const char* pshaderModel = Shader::GetShaderModelForFeatureLevel(featureLevel, "ps");

    hr = Shader::CompileShaderFromFile(
        L"RectangleShader.fxh",
		"PSMain", pshaderModel, &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to compile pixel shader.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

	// Create the pixel shader
    hr = renderer.GetDevice()->CreatePixelShader(
        pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(),
        nullptr,
        &m_pixelShader
    );
    pPSBlob->Release();
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create pixel shader.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
	}
}
