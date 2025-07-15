#include "Mesh.h"
#include <cstring>

#include "Utility/Logger.h"

#include "Graphics/Renderer.h"

namespace sl
{



    void Mesh::CreateMesh(Renderer& renderer)
    {
        auto device = renderer.GetDevice();
        auto context = renderer.GetDeviceContext();

        //std::vector<Vertex> vertices(4);
        Vertex vertices[] = {
            { Vector3(-0.5f,  0.5f, 0.0f), Vector2(0.0, 0.0), Vector4(1.0f, 0.0f, 0.0f, 1.0f) }, // Left Up
            { Vector3( 0.5f,  0.5f, 0.0f), Vector2(1.0, 0.0), Vector4(1.0f, 0.0f, 0.0f, 1.0f) }, // Right Up
            { Vector3( 0.5f, -0.5f, 0.0f), Vector2(1.0, 1.0), Vector4(1.0f, 0.0f, 0.0f, 1.0f) }, // Right Down
            { Vector3(-0.5f, -0.5f, 0.0f), Vector2(0.0, 1.0), Vector4(1.0f, 0.0f, 0.0f, 1.0f) }, // Left Down
        };

        // Revise 1
        //vertices[0] = { {-0.5f,  0.5f, 0.0f}, {1, 0, 0, 1} };
        //vertices[1] = { { 0.5f,  0.5f, 0.0f}, {1, 0, 0, 1} };
        //vertices[2] = { { 0.5f, -0.5f, 0.0f}, {1, 0, 0, 1} };
        //vertices[3] = { {-0.5f, -0.5f, 0.0f}, {1, 0, 0, 1} };
        
        //SetVertices(vertices);
        
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(vertices);//UINT(sizeof(Vertex) * mVertices.size());
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices;//mVertices.data();
        
        HRESULT hr = device->CreateBuffer(
            &desc, 
            &initData, 
            mVertexBuffer.GetAddressOf()
        );
        if (FAILED(hr)) {
            LOG_ERROR("Mesh::CreateVertexBuffer - 捀揰僶僢僼傽偺嶌惉偵幐攕偟傑偟偨 (HRESULT = 0x" + std::to_string(hr) + ")");
        }
        
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);

        WORD indices[] = {
            0, 1, 2, // 戞1嶰妏宍
            0, 2, 3  // 戞2嶰妏宍
        };
        
        // Create index buffer
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(WORD) * 6;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        initData.pSysMem = indices;
        
        hr = device->CreateBuffer(&desc, &initData, mIndexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            LOG_ERROR("Mesh::CreateVertexBuffer - index僶僢僼傽偺嶌惉偵幐攕偟傑偟偨 (HRESULT = 0x" + std::to_string(hr) + ")");
        }
        
        //auto context = renderer.GetDeviceContext();
        context->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
    
    void Mesh::CreateVertexBuffer(ID3D11Device* device)
    {
      
    
    }
}
