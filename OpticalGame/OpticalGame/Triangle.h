#pragma once
#include "Shape.h"

class Triangle : public Shape
{
public:
	Triangle(Renderer& renderer);
	~Triangle() override;
	void Draw(Renderer& renderer) override;
	void Update(float deltaTime) override;
	void ProcessInput(float deltaTime, std::vector<KeyState>& keyStates) override {}

private:
	void CreateMesh(Renderer& renderer) override;
	void CreateShaders(Renderer& renderer) override;

};