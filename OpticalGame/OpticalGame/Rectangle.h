#pragma once
#include "Shape.h"
#include <chrono>
#include <unordered_map>

class Rectangle : public Shape
{
public:
	Rectangle(Renderer& renderer);
	~Rectangle() override;
	void Draw(Renderer& renderer) override;
	void Update(float deltaTime) override;
	void ProcessInput() override;

private:
	struct KeyState {
		bool isPressd = false;
		std::chrono::steady_clock::time_point pressTime;
	};
	std::unordered_map<int, KeyState> keyStates;
	// The key that was pressd currently, 0 means no key pressed
	int activeKey = 0;
	float gridSize = 100.0f;
	void MoveInDirection(int key);

	void CreateMesh(Renderer& renderer) override;
	void CreateShaders(Renderer& renderer) override;

	Matrix4 m_worldMatrix;
	Matrix4 m_viewMatrix;
	Matrix4 m_projectionMatrix;
};

