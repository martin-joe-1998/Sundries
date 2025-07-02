#pragma once
#include "Shape.h"

class Rectangle : public Shape
{
public:
	Rectangle(Renderer& renderer);
	~Rectangle() override;
	void Draw(Renderer& renderer) override;
	void Update(float deltaTime) override;

	Mesh::Transform GetTransform() const { return m_transform; }
	void SetTransform(const Mesh::Transform& transform) { m_transform = transform; }
private:
	void CreateMesh(Renderer& renderer) override;
	void CreateShaders(Renderer& renderer) override;

	Matrix4 m_worldMatrix;
	Matrix4 m_viewMatrix;
	Matrix4 m_projectionMatrix;

	// Rotation is in degrees
	Mesh::Transform m_transform = { 
		Vector3(0.0f, 0.0f, 0.0f), // PositionWS
		Vector3(100.0f, 100.0f, 1.0f), // Scale
		Vector3(0.0f, 0.0f, 0.0f)  // Rotation
	};
};

