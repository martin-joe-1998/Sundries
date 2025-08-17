#include "Mesh.h"
#include <cstring>

#include "Utility/Logger.h"
#include "Utility/Math.h"

#include "Graphics/Renderer.h"

namespace sl
{

  void Mesh::CreateMesh(Renderer& renderer)
  {
    auto device = renderer.GetDevice();
    auto context = renderer.GetDeviceContext();


    Vertex vertices[] = {
      { Vector3(-0.5f,  0.5f, 0.0f), Vector2(0.0, 0.0), Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
      { Vector3(0.5f,  0.5f, 0.0f), Vector2(1.0, 0.0), Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
      { Vector3(0.5f, -0.5f, 0.0f), Vector2(1.0, 1.0), Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
      { Vector3(-0.5f, -0.5f, 0.0f), Vector2(0.0, 1.0), Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
    };

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(vertices);
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
  
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
  
    HRESULT hr = device->CreateBuffer(&desc, &initData, mVertexBuffer.GetAddressOf());
    if (FAILED(hr)) {
      LOG_ERROR("Mesh::CreateVertexBuffer - 頂点バッファの作成に失敗しました (HRESULT = 0x" + std::to_string(hr) + ")");
    }

    //UINT stride = sizeof(Vertex);
    //UINT offset = 0;
    //context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);

    WORD indices[] = {
      0, 1, 2, // 第1三角形
      0, 2, 3  // 第2三角形
    };

    // Create index buffer
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(WORD) * 6;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    initData.pSysMem = indices;

    hr = device->CreateBuffer(&desc, &initData, mIndexBuffer.GetAddressOf());
    if (FAILED(hr)) {
      LOG_ERROR("Mesh::CreateVertexBuffer - indexバッファの作成に失敗しました (HRESULT = 0x" + std::to_string(hr) + ")");
    }


    //context->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    //context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  }

  void Mesh::SetMeshBuffer(Renderer& renderer)
  {
      auto context = renderer.GetDeviceContext();

      // Set vertex buffer per actor
      UINT stride = sizeof(Vertex);
      UINT offset = 0;
      context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);

      // Set index buffer and topology
      context->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
      context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  }

  void Mesh::CreateVertexBuffer(ID3D11Device* device)
  {
    

  }

}
