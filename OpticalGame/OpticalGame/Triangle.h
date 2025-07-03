#pragma once
#include "Shape.h"

class Triangle : public Shape
{
public:
	Triangle(Renderer& renderer);
	~Triangle() override;
	void Draw(Renderer& renderer) override;
	void Update(float deltaTime) override;
	void ProcessInput(float deltaTime) override {}

private:
	void CreateMesh(Renderer& renderer) override;
	void CreateShaders(Renderer& renderer) override;

	// ID3D11Buffer* m_vertexBuffer = nullptr;
	// ID3D11VertexShader* m_vertexShader = nullptr;
	// ID3D11PixelShader* m_pixelShader = nullptr;
	// ID3D11InputLayout* m_inputLayout = nullptr;
};