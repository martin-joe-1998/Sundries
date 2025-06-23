#include "Triangle.h"
#include <fstream>
#include <vector>

struct Vertex
{
	float x, y; // Position
	float r, g, b; // Color
};

Triangle::Triangle(Renderer& renderer)
{
	CreateMesh(renderer);
	CreateShaders(renderer);
}

Triangle::~Triangle()
{
	m_vertexBuffer->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
}

void Triangle::Draw(Renderer& renderer)
{
	// Bind the triangle shader
	renderer.GetDeviceContext()->VSSetShader(m_vertexShader, nullptr, 0);
	renderer.GetDeviceContext()->PSSetShader(m_pixelShader, nullptr, 0);

	// Bind the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	renderer.GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	// renderer.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw the triangle
	renderer.GetDeviceContext()->Draw(3, 0); // 3 vertices, starting at index 0
}

void Triangle::CreateMesh(Renderer& renderer)
{
	// Define vertices for a triangle
	Vertex vertices[] = {
		{ -1.0f,  -1.0f, 1.0f, 0.0f, 0.0f }, // Top vertex (Red)
		{ 0.0f, 1.0f, 1.0f, 0.0f, 0.0f }, // Bottom right vertex (Green)
		{ 1.0f, -1.0f, 1.0f, 0.0f, 0.0f }  // Bottom left vertex (Blue)
	};

	// Create vertex buffer
	auto vertexBufferDesc = CD3D11_BUFFER_DESC(
		sizeof(vertices),
		D3D10_BIND_VERTEX_BUFFER
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
	std::ifstream vsFile("TriangleVertexShader.cso", std::ios::binary);
	std::ifstream psFile("TrianglePixelShader.cso", std::ios::binary);

	std::vector<char> vsData = {std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
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
}