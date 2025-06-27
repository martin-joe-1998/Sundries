#include "Triangle.h"
#include "Math.h"
#include <fstream>
#include <vector>

struct Vertex
{
	float x, y, z; // Position
	float u, v;    // UV
	float r, g, b; // Color
};

Triangle::Triangle(Renderer& renderer)
{
	CreateMesh(renderer);
	CreateShaders(renderer);
}

Triangle::~Triangle()
{
	if (m_vertexBuffer) m_vertexBuffer->Release();
	if (m_vertexShader) m_vertexShader->Release();
	if (m_pixelShader) m_pixelShader->Release();
	if (m_inputLayout) m_inputLayout->Release();
}

void Triangle::Draw(Renderer& renderer)
{
	auto deviceContext = renderer.GetDeviceContext();

	// Bind the triangle shader
	deviceContext->IASetInputLayout(m_inputLayout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	// Bind the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw the triangle
	deviceContext->Draw(3, 0); // 3 vertices, starting at index 0
}

void Triangle::CreateMesh(Renderer& renderer)
{
	// Define vertices for a triangle
	Vertex vertices[] = {
		{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f }, // Top vertex (Red)
		{  0.0f,  1.0f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f }, // Bottom right vertex (Green)
		{  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f }  // Bottom left vertex (Blue)
	};

	// Create vertex buffer
	auto vertexBufferDesc = CD3D11_BUFFER_DESC(
		sizeof(vertices),
		D3D11_BIND_VERTEX_BUFFER
	);
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;

	renderer.GetDevice()->CreateBuffer(
		&vertexBufferDesc,
		&vertexData,
		&m_vertexBuffer
	);
}

void Triangle::CreateShaders(Renderer& renderer)
{
	// Create and load compiled shader files
	std::ifstream vsFile("TriangleVertexShader.cso", std::ios::binary);
	std::ifstream psFile("TrianglePixelShader.cso", std::ios::binary);

	// not a good practice, but I'm assuming the files exist and are valid
	std::vector<char> vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	std::vector<char> psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

	// Create vertex shader
	renderer.GetDevice()->CreateVertexShader(
		vsData.data(),
		vsData.size(),
		nullptr,
		&m_vertexShader
	);
	// Create pixel shader
	renderer.GetDevice()->CreatePixelShader(
		psData.data(),
		psData.size(),
		nullptr,
		&m_pixelShader
	);

	// Create input layout
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	renderer.GetDevice()->CreateInputLayout(
		layout,
		ARRAYSIZE(layout),
		vsData.data(),
		vsData.size(),
		&m_inputLayout
	);
}