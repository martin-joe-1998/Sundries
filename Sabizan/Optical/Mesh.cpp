#include "Mesh.h"

void Mesh::CreateMesh(Renderer& renderer)
{
    auto deviceContext = renderer.GetDeviceContext();

    // ----------------------------- Vertex -----------------------------
    // Define vertices for a rectangle
    Mesh::Vertex vertices[] = {
        { Vector3(0.0f,  0.0f, 0.0f), Vector2(0.0f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f) }, // Left Up
        { Vector3(1.0f,  0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f) }, // Right Up
        { Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) }, // Right Down
        { Vector3(0.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector4(0.3f, 0.4f, 0.5f, 1.0f) }, // Left Down
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
    //UINT stride = sizeof(Mesh::Vertex);
    //UINT offset = 0;
    //deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);


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
    //deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    // Set the primitive topology to triangle list
    //deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::CreateFullscreenVertex(Renderer& renderer)
{

}

void Mesh::SetMeshBuffer(Renderer& renderer)
{
    auto deviceContext = renderer.GetDeviceContext();
    
    // Set the Vertex buffer
    UINT stride = sizeof(Mesh::Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    // Set the primitive topology to triangle list
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}